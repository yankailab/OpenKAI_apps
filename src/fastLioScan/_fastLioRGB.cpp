/*
 * _fastLioRGB.cpp
 *
 *  Created on: July 17, 2024
 *      Author: yankai
 */

#include "_fastLioRGB.h"

namespace kai
{

	_fastLioRGB::_fastLioRGB()
	{
		m_projDir = "";
		m_fModelIn = "scan.pcd";
		m_fModelOut = "scanRGB.pcd";
		m_fCamConfig = "camConfig.json";
		m_fCamConfigDefault = "camConfig.json";
		m_fCamTraj = "camTraj.json";
		m_vdSize = 0.01;

		m_pPC = nullptr;
	}

	_fastLioRGB::~_fastLioRGB()
	{
	}

	bool _fastLioRGB::init(void *pKiss)
	{
		IF_F(!this->_JSONbase::init(pKiss));
		Kiss *pK = (Kiss *)pKiss;

		pK->v("projDir", &m_projDir);
		pK->v("fModelIn", &m_fModelIn);
		pK->v("fModelOut", &m_fModelOut);
		pK->v("fCamConfig", &m_fCamConfig);
		pK->v("fCamConfigDefault", &m_fCamConfigDefault);
		pK->v("fCamTraj", &m_fCamTraj);
		pK->v("vdSize", &m_vdSize);

		return true;
	}

	bool _fastLioRGB::link(void)
	{
		IF_F(!this->_JSONbase::link());

		Kiss *pK = (Kiss *)m_pKiss;
		string n;

		n = "";
		pK->v("_PCframe", &n);
		m_pPC = (_PCframe *)(pK->findModule(n));
		IF_Fl(!m_pPC, n + ": not found");

		return true;
	}

	bool _fastLioRGB::start(void)
	{
		NULL_F(m_pT);
		NULL_F(m_pTr);

		IF_F(!m_pT->start(getUpdate, this));
		IF_F(!m_pTr->start(getUpdateR, this));

		return true;
	}

	int _fastLioRGB::check(void)
	{
		NULL__(m_pPC, -1);

		return this->_JSONbase::check();
	}

	void _fastLioRGB::update(void)
	{
		while (m_pT->bAlive())
		{
			m_pT->autoFPSfrom();

			m_pT->autoFPSto();
		}
	}

	bool _fastLioRGB::loadModel(string fModel)
	{
		IF_F(check() < 0);

		if (fModel.empty())
			fModel = m_projDir + m_fModelIn;

		clearModel();

		//	io::ReadPointCloudOption ro;
		IF_F(!io::ReadPointCloud(fModel, m_pcModelIn));
		LOG_I("Read point cloud: " + i2str(m_pcModelIn.points_.size()));

		return true;
	}

	void _fastLioRGB::updateModel(void)
	{
		IF_(check() < 0);

		updateCamIntrinsicMatrix();

		// merge colors with point cloud using current camera config
		m_pcModelOut.Clear();
		for (FASTLIO_SCAN_FRAME f : m_vFrame)
		{
			updateFrame(m_pcModelIn, &m_pcModelOut, f);
		}

		*m_pPC->getNextBuffer() = m_pcModelOut;
		m_pPC->swapBuffer();
	}

	void _fastLioRGB::updateCamIntrinsicMatrix(void)
	{
		m_mCi = Matrix4d::Identity();
		m_mCi(0, 0) = m_camIntrinsic.m_Fx;
		m_mCi(1, 1) = m_camIntrinsic.m_Fy;
		m_mCi(0, 1) = m_camIntrinsic.m_Gamma;
		m_mCi(0, 2) = m_camIntrinsic.m_Cx;
		m_mCi(1, 2) = m_camIntrinsic.m_Cy;

		m_mCS = Matrix4d::Identity();
		Vector3d eR(m_camIntrinsic.m_vCSr.x,
					m_camIntrinsic.m_vCSr.y,
					m_camIntrinsic.m_vCSr.z);
		m_mCS.block(0, 0, 3, 3) = Geometry3D::GetRotationMatrixFromAxisAngle(eR);
		m_mCS(0, 3) = m_camIntrinsic.m_vCSt.x;
		m_mCS(1, 3) = m_camIntrinsic.m_vCSt.y;
		m_mCS(2, 3) = m_camIntrinsic.m_vCSt.z;
	}

	void _fastLioRGB::updateFrame(const PointCloud &pcRaw, PointCloud *pPC, const FASTLIO_SCAN_FRAME &f)
	{
		NULL_(pPC);

		// camera translation and rotation matrices for the frame
		Matrix3d mCWr;
		mCWr = Eigen::Quaterniond(
				   f.m_vQ.w,
				   f.m_vQ.x,
				   f.m_vQ.y,
				   f.m_vQ.z)
				   .toRotationMatrix();

		Matrix4d mCW = Matrix4d::Identity();
		mCW.block(0, 0, 3, 3) = mCWr;
		mCW(0, 3) = f.m_vT.x;
		mCW(1, 3) = f.m_vT.y;
		mCW(2, 3) = f.m_vT.z;

		mCW = mCW * m_mCS;

		Matrix4d mWC = mCW.inverse();
		//		Matrix4d mWS = m_mCi * mWC * m_mCS;
		Matrix4d mWS = mWC;

		for (int i = 0; i < pcRaw.points_.size(); i++)
		{
			Vector3d vPraw = pcRaw.points_[i];
			Vector4d vPw = {vPraw[0], vPraw[1], vPraw[2], 1};
			// Vector4d vScr = mWS * vPw;

			Vector4d vPc = mWC * vPw;
			Vector4d vPcs =
				{
					-vPc[1],
					-vPc[2],
					vPc[0],
					1};
			IF_CONT(vPcs[2] < 0);

			Vector4d vPs = m_mCi * vPcs;
			vPs *= 1.0 / vPcs[2];

			vFloat2 vScr = {0, 1};
			IF_CONT(!vScr.bInside(vPs[0]));
			IF_CONT(!vScr.bInside(vPs[1]));

			int px = vPs[0] * f.m_mRGB.cols;
			int py = vPs[1] * f.m_mRGB.rows;
			cv::Vec3b vCol = f.m_mRGB.at<cv::Vec3b>(py, px);

			Vector3d vC = {vCol[2], vCol[1], vCol[0]};
			vC *= 1.0 / 255.0;

			pPC->points_.push_back(vPraw);
			pPC->colors_.push_back(vC);
		}

		LOG_I("Point Cloud Model Out: " + i2str(pPC->points_.size()));
	}

	bool _fastLioRGB::saveModel(string fModel)
	{
		IF_F(check() < 0);

		if (fModel.empty())
			fModel = m_projDir + m_fModelOut;

		IF_F(!io::WritePointCloud(fModel, m_pcModelOut));
		LOG_I("Write point cloud: " + i2str(m_pcModelOut.points_.size()));

		PointCloud pcVD = *m_pcModelOut.VoxelDownSample(m_vdSize);
		IF_F(!io::WritePointCloud(fModel + ".vd.pcd", pcVD));
		LOG_I("Write voxel down point cloud: " + i2str(pcVD.points_.size()));

		return true;
	}

	void _fastLioRGB::clearModel(void)
	{
		m_pcModelIn.Clear();
	}

	bool _fastLioRGB::loadCamConfig(string fConfig)
	{
		if (fConfig.empty())
			fConfig = m_projDir + m_fCamConfig;

		string s;
		if (!readFile(fConfig, &s))
		{
			LOG_I("Cannot open: " + fConfig);

			if (!readFile(m_fCamConfigDefault, &s))
			{
				LOG_I("Cannot open: " + m_fCamConfigDefault);
				return false;
			}
		}

		value v;
		string err = parse(v, s);
		IF_Fl(!err.empty(), err);

		object &obj = v.get<object>();
		value vi = obj["camIntrinsic"];
		IF_F(!vi.is<object>());
		object ci = vi.get<object>();

		m_camIntrinsic.m_Fx = ci["Fx"].get<double>();
		m_camIntrinsic.m_Fy = ci["Fy"].get<double>();
		m_camIntrinsic.m_Gamma = ci["Gamma"].get<double>();
		m_camIntrinsic.m_Cx = ci["Cx"].get<double>();
		m_camIntrinsic.m_Cy = ci["Cy"].get<double>();
		m_camIntrinsic.m_vCSt.x = ci["vCStX"].get<double>();
		m_camIntrinsic.m_vCSt.y = ci["vCStY"].get<double>();
		m_camIntrinsic.m_vCSt.z = ci["vCStZ"].get<double>();
		m_camIntrinsic.m_vCSr.x = ci["vCSrX"].get<double>();
		m_camIntrinsic.m_vCSr.y = ci["vCSrY"].get<double>();
		m_camIntrinsic.m_vCSr.z = ci["vCSrZ"].get<double>();

		return true;
	}

	bool _fastLioRGB::saveCamConfig(string fConfig)
	{
		if (fConfig.empty())
			fConfig = m_projDir + m_fCamConfig;

		picojson::object o;
		o.insert(make_pair("Fx", value(m_camIntrinsic.m_Fx)));
		o.insert(make_pair("Fy", value(m_camIntrinsic.m_Fy)));
		o.insert(make_pair("Gamma", value(m_camIntrinsic.m_Gamma)));
		o.insert(make_pair("Cx", value(m_camIntrinsic.m_Cx)));
		o.insert(make_pair("Cy", value(m_camIntrinsic.m_Cy)));
		o.insert(make_pair("vCStX", value(m_camIntrinsic.m_vCSt.x)));
		o.insert(make_pair("vCStY", value(m_camIntrinsic.m_vCSt.y)));
		o.insert(make_pair("vCStZ", value(m_camIntrinsic.m_vCSt.z)));
		o.insert(make_pair("vCSrX", value(m_camIntrinsic.m_vCSr.x)));
		o.insert(make_pair("vCSrY", value(m_camIntrinsic.m_vCSr.y)));
		o.insert(make_pair("vCSrZ", value(m_camIntrinsic.m_vCSr.z)));

		picojson::object jo;
		jo.insert(make_pair("camIntrinsic", o));

		string f = picojson::value(jo).serialize();

		return writeFile(fConfig, f);
	}

	bool _fastLioRGB::loadCamTraj(string fTraj)
	{
		if (fTraj.empty())
			fTraj = m_projDir + m_fCamTraj;

		string s;
		if (!readFile(fTraj, &s))
		{
			LOG_I("Cannot open: " + fTraj);
			return false;
		}

		value v;
		string err = parse(v, s);
		IF_Fl(!err.empty(), err);

		object &obj = v.get<object>();
		IF_F(!obj["imgTraj"].is<picojson::array>());
		picojson::array &ary = obj["imgTraj"].get<picojson::array>();
		for (value &rec : ary)
		{
			object &o = rec.get<object>();
			IF_CONT(!o["fImg"].is<string>());
			IF_CONT(!o["vQ"].is<value::array>());
			IF_CONT(!o["vT"].is<value::array>());

			string fImg = o["fImg"].get<string>();
			Mat m = imread(fImg);
			IF_CONT(m.empty());

			FASTLIO_SCAN_FRAME f;
			f.m_mRGB = m;

			value::array vQ = o["vQ"].get<value::array>();
			f.m_vQ.x = vQ[0].get<double>();
			f.m_vQ.y = vQ[1].get<double>();
			f.m_vQ.z = vQ[2].get<double>();
			f.m_vQ.w = vQ[3].get<double>();

			value::array vT = o["vT"].get<value::array>();
			f.m_vT.x = vT[0].get<double>();
			f.m_vT.y = vT[1].get<double>();
			f.m_vT.z = vT[2].get<double>();

			m_vFrame.push_back(f);
		}

		return true;
	}

	void _fastLioRGB::updateR(void)
	{
		while (m_pTr->bAlive())
		{
			m_pTr->autoFPSfrom();

			if (recv())
			{
				handleMsg(m_strB);
				m_strB.clear();
			}

			m_pTr->autoFPSto();
		}
	}

	void _fastLioRGB::handleMsg(const string &str)
	{
		value json;
		IF_(!str2JSON(str, &json));

		object &jo = json.get<object>();
		IF_(!jo["cmd"].is<string>());
		string cmd = jo["cmd"].get<string>();

		if (cmd == "loadProj")
		{
			loadProj(jo);
		}
		else if (cmd == "exportModel")
		{
			exportModel(jo);
		}
		else if (cmd == "setCamConfig")
		{
			setParam(jo);
			updateModel();
		}
		else if (cmd == "saveCamConfig")
		{
			saveParam(jo);
		}
		else if (cmd == "getCamConfig")
		{
			getParam(jo);
		}
		else if (cmd == "getCamConfigSaved")
		{
			getParamSaved(jo);
		}
	}

	void _fastLioRGB::loadProj(picojson::object &jo)
	{
		IF_(!jo["projDir"].is<string>());
		string projDir = jo["projDir"].get<string>();

		m_projDir = projDir;

		string r = "";

		if (!loadModel(projDir + m_fModelIn))
			r += "Load model failed; ";

		if (!loadCamConfig(projDir + m_fCamConfig))
			r += "Load camera config failed; ";

		if (!loadCamTraj(projDir + m_fCamTraj))
			r += "Load camera trajectory failed; ";

		if (r.empty())
		{
			updateModel();
			r = "Load model suceeded";
		}

		object jR;
		JO(jR, "cmd", "loadProj");
		JO(jR, "r", r);
		sendMsg(jR);
	}

	void _fastLioRGB::exportModel(picojson::object &jo)
	{
		saveModel();
	}

	void _fastLioRGB::setParam(picojson::object &jo)
	{
		IF_(check() < 0);

		FASTLIO_SCAN_CAM_INTRINSIC ci;

		IF_(!jo["Fx"].is<double>());
		ci.m_Fx = jo["Fx"].get<double>();
		IF_(!jo["Fy"].is<double>());
		ci.m_Fy = jo["Fy"].get<double>();
		IF_(!jo["Gamma"].is<double>());
		ci.m_Gamma = jo["Gamma"].get<double>();
		IF_(!jo["Cx"].is<double>());
		ci.m_Cx = jo["Cx"].get<double>();
		IF_(!jo["Cy"].is<double>());
		ci.m_Cy = jo["Cy"].get<double>();

		IF_(!jo["vCStX"].is<double>());
		ci.m_vCSt.x = jo["vCStX"].get<double>();
		IF_(!jo["vCStY"].is<double>());
		ci.m_vCSt.y = jo["vCStY"].get<double>();
		IF_(!jo["vCStZ"].is<double>());
		ci.m_vCSt.z = jo["vCStZ"].get<double>();

		IF_(!jo["vCSrX"].is<double>());
		ci.m_vCSr.x = jo["vCSrX"].get<double>();
		IF_(!jo["vCSrY"].is<double>());
		ci.m_vCSr.y = jo["vCSrY"].get<double>();
		IF_(!jo["vCSrZ"].is<double>());
		ci.m_vCSr.z = jo["vCSrZ"].get<double>();

		// IF_(!jo["vCSoffset"].is<value::array>());
		// value::array vCS = jo["vCSoffset"].get<value::array>();
		// ci.m_vCSoffset.x = vCS[0].get<double>();
		// ci.m_vCSoffset.y = vCS[1].get<double>();
		// ci.m_vCSoffset.z = vCS[2].get<double>();

		m_camIntrinsic = ci;
	}

	void _fastLioRGB::saveParam(picojson::object &jo)
	{
		IF_(check() < 0);

		saveCamConfig();
	}

	void _fastLioRGB::getParam(picojson::object &jo)
	{
		IF_(check() < 0);

		object r;
		JO(r, "cmd", "getParam");
		JO(r, "Fx", m_camIntrinsic.m_Fx);
		JO(r, "Fy", m_camIntrinsic.m_Fy);
		JO(r, "Gamma", m_camIntrinsic.m_Gamma);
		JO(r, "Cx", m_camIntrinsic.m_Cx);
		JO(r, "Cy", m_camIntrinsic.m_Cy);
		JO(r, "vCStX", m_camIntrinsic.m_vCSt.x);
		JO(r, "vCStY", m_camIntrinsic.m_vCSt.y);
		JO(r, "vCStZ", m_camIntrinsic.m_vCSt.z);
		JO(r, "vCSrX", m_camIntrinsic.m_vCSr.x);
		JO(r, "vCSrY", m_camIntrinsic.m_vCSr.y);
		JO(r, "vCSrZ", m_camIntrinsic.m_vCSr.z);

		sendMsg(r);
	}

	void _fastLioRGB::getParamSaved(picojson::object &jo)
	{
		IF_(check() < 0);

		loadCamConfig();
		getParam(jo);
	}

	void _fastLioRGB::console(void *pConsole)
	{
		IF_(check() < 0);
		this->_JSONbase::console(pConsole);

		_Console *pC = (_Console *)pConsole;
		pC->addMsg("NpModelIn = " + i2str(m_pcModelIn.points_.size()));
		pC->addMsg("NpModelOut = " + i2str(m_pcModelOut.points_.size()));
		pC->addMsg("Fx = " + f2str(m_camIntrinsic.m_Fx));
		pC->addMsg("Fy = " + f2str(m_camIntrinsic.m_Fy));
		pC->addMsg("Gamma = " + f2str(m_camIntrinsic.m_Gamma));
		pC->addMsg("Cx = " + f2str(m_camIntrinsic.m_Cx));
		pC->addMsg("Cy = " + f2str(m_camIntrinsic.m_Cy));
		pC->addMsg("vCSt = (" + f2str(m_camIntrinsic.m_vCSt.x) + ", " + f2str(m_camIntrinsic.m_vCSt.y) + ", " + f2str(m_camIntrinsic.m_vCSt.z) + ")");
		pC->addMsg("vCSr = (" + f2str(m_camIntrinsic.m_vCSr.x) + ", " + f2str(m_camIntrinsic.m_vCSr.y) + ", " + f2str(m_camIntrinsic.m_vCSr.z) + ")");
	}

}
