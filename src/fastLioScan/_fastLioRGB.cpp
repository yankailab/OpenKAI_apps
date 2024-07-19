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
		m_fCamTraj = "camTraj.json";

		m_pPC = nullptr;

		clearModel();
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
		pK->v("fCamTraj", &m_fCamTraj);

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
		IF_F(!io::ReadPointCloud(fModel, m_pcModel));
		LOG_I("Read point cloud: " + i2str(m_pcModel.points_.size()));

		updateModel();

		return true;
	}

	void _fastLioRGB::updateModel(void)
	{
		IF_(check() < 0);

		// update camera intrinsic matrices

		// merge colors with point cloud using current camera config
		m_pcModelRGB.Clear();
		for (FASTLIO_SCAN_FRAME f : m_vFrame)
		{
			updateFrame(m_pcModel, &m_pcModelRGB, f);
		}

		*m_pPC->getNextBuffer() = m_pcModelRGB;
		m_pPC->swapBuffer();
	}

	void _fastLioRGB::updateFrame(const PointCloud& pcRaw, PointCloud *pPC, const FASTLIO_SCAN_FRAME &f)
	{
		NULL_(pPC);

		// camera translation and rotation matrices for the frame
		Matrix3d mR;
		mR = Eigen::Quaterniond(
				 f.m_vQ.w,
				 f.m_vQ.x,
				 f.m_vQ.y,
				 f.m_vQ.z)
				 .toRotationMatrix();

		Matrix4d mT = Matrix4d::Identity();
		mT.block(0, 0, 3, 3) = mR;
		mT(0, 3) = f.m_vT.x;
		mT(1, 3) = f.m_vT.y;
		mT(2, 3) = f.m_vT.z;

		Matrix4d mW2C = mT.inverse();
		Matrix4d mW2S = m_mCamInt * mW2C * m_mCSoffset;

		for (int i = 0; i < pcRaw.points_.size(); i++)
		{
			Vector3d vPraw = pcRaw.points_[i];
			Vector4d vPw = {vPraw[0], vPraw[1], vPraw[2], 1};
			Vector4d vScr = mW2S * vPw;

			Vector3d vP;
			Vector3d vC;

			pPC->points_.push_back(vP);
			pPC->colors_.push_back(vC);
		}
	}

	bool _fastLioRGB::saveModel(string fModel)
	{
		IF_F(check() < 0);

		if (fModel.empty())
			fModel = m_projDir + m_fModelOut;

		IF_F(!io::WritePointCloud(fModel, m_pcModelRGB));
		LOG_I("Write point cloud: " + i2str(m_pcModelRGB.points_.size()));

		return true;
	}

	void _fastLioRGB::clearModel(void)
	{
		m_pcModel.Clear();
	}

	bool _fastLioRGB::loadCamConfig(string fConfig)
	{
		if (fConfig.empty())
			fConfig = m_projDir + m_fCamConfig;

		string s;
		if (!readFile(fConfig, &s))
		{
			LOG_I("Cannot open: " + fConfig);
			return false;
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
		m_camIntrinsic.m_vCSoffset.x = ci["vCSoffsetX"].get<double>();
		m_camIntrinsic.m_vCSoffset.y = ci["vCSoffsetY"].get<double>();
		m_camIntrinsic.m_vCSoffset.z = ci["vCSoffsetZ"].get<double>();

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
		o.insert(make_pair("vCSoffsetX", value(m_camIntrinsic.m_vCSoffset.x)));
		o.insert(make_pair("vCSoffsetY", value(m_camIntrinsic.m_vCSoffset.y)));
		o.insert(make_pair("vCSoffsetZ", value(m_camIntrinsic.m_vCSoffset.z)));

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
			loadProj(jo);
		else if (cmd == "exportModel")
			exportModel(jo);
		else if (cmd == "setCamConfig")
			setParam(jo);
		else if (cmd == "saveCamConfig")
			saveParam(jo);
		else if (cmd == "getCamConfig")
			getParam(jo);
		else if (cmd == "getCamConfigSaved")
			getParamSaved(jo);
	}

	void _fastLioRGB::loadProj(picojson::object &jo)
	{
		loadModel();
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

		IF_(!jo["OfsX"].is<double>());
		ci.m_vCSoffset.x = jo["OfsX"].get<double>();
		IF_(!jo["OfsY"].is<double>());
		ci.m_vCSoffset.y = jo["OfsY"].get<double>();
		IF_(!jo["OfsZ"].is<double>());
		ci.m_vCSoffset.z = jo["OfsZ"].get<double>();

		// IF_(!jo["vCSoffset"].is<value::array>());
		// value::array vCS = jo["vCSoffset"].get<value::array>();
		// ci.m_vCSoffset.x = vCS[0].get<double>();
		// ci.m_vCSoffset.y = vCS[1].get<double>();
		// ci.m_vCSoffset.z = vCS[2].get<double>();

		m_camIntrinsic = ci;
		
		m_mCamInt = Matrix4d::Zero();
		m_mCamInt(0, 0) = m_camIntrinsic.m_Fx;
		m_mCamInt(1, 1) = m_camIntrinsic.m_Fy;
		m_mCamInt(0, 1) = m_camIntrinsic.m_Gamma;
		m_mCamInt(0, 2) = m_camIntrinsic.m_Cx;
		m_mCamInt(1, 2) = m_camIntrinsic.m_Cy;
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
		JO(r, "OfsX", m_camIntrinsic.m_vCSoffset.x);
		JO(r, "OfsY", m_camIntrinsic.m_vCSoffset.y);
		JO(r, "OfsZ", m_camIntrinsic.m_vCSoffset.z);

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
		NULL_(pConsole);
		this->_JSONbase::console(pConsole);
		IF_(check() < 0);

		_Console *pC = (_Console *)pConsole;
		pC->addMsg("Fx = " + f2str(m_camIntrinsic.m_Fx));
		pC->addMsg("Fy = " + f2str(m_camIntrinsic.m_Fy));
		pC->addMsg("Gamma = " + f2str(m_camIntrinsic.m_Gamma));
		pC->addMsg("Cx = " + f2str(m_camIntrinsic.m_Cx));
		pC->addMsg("Cy = " + f2str(m_camIntrinsic.m_Cy));
		pC->addMsg("vCSoffset = (" + f2str(m_camIntrinsic.m_vCSoffset.x) + ", " + f2str(m_camIntrinsic.m_vCSoffset.y) + ", " + f2str(m_camIntrinsic.m_vCSoffset.z) + ")");
	}

}
