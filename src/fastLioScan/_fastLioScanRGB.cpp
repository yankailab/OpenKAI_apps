/*
 * _fastLioScanRGB.cpp
 *
 *  Created on: July 17, 2024
 *      Author: yankai
 */

#include "_fastLioScanRGB.h"

namespace kai
{

	_fastLioScanRGB::_fastLioScanRGB()
	{
		m_fModel = "";
		m_fCamConfig = "";
		m_fCamTraj = "";

		m_pPC = nullptr;

		clearModel();
	}

	_fastLioScanRGB::~_fastLioScanRGB()
	{
	}

	bool _fastLioScanRGB::init(void *pKiss)
	{
		IF_F(!this->_ModuleBase::init(pKiss));
		Kiss *pK = (Kiss *)pKiss;

		pK->v("fModel", &m_fModel);
		pK->v("fCamConfig", &m_fCamConfig);
		pK->v("fCamTraj", &m_fCamTraj);

		return true;
	}

	bool _fastLioScanRGB::link(void)
	{
		IF_F(!this->_ModuleBase::link());

		Kiss *pK = (Kiss *)m_pKiss;
		string n;

		n = "";
		pK->v("_PCframe", &n);
		m_pPC = (_PCframe *)(pK->findModule(n));
		IF_Fl(!m_pPC, n + ": not found");

		return true;
	}

	bool _fastLioScanRGB::start(void)
	{
		NULL_F(m_pT);
		IF_F(!m_pT->start(getUpdate, this));

		return true;
	}

	int _fastLioScanRGB::check(void)
	{
		NULL__(m_pPC, -1);

		return this->_ModuleBase::check();
	}

	void _fastLioScanRGB::update(void)
	{
		while (m_pT->bAlive())
		{
			m_pT->autoFPSfrom();

			//			updateModel();

			m_pT->autoFPSto();
		}
	}

	bool _fastLioScanRGB::openModel(string fModel)
	{
		IF_F(check() < 0);

		if (fModel.empty())
			fModel = m_fModel;

		clearModel();

		//	io::ReadPointCloudOption ro;
		IF_F(!io::ReadPointCloud(fModel, m_pcModel));
		LOG_I("Read point cloud: " + i2str(m_pcModel.points_.size()));

		updateModel();

		return true;
	}

	void _fastLioScanRGB::updateModel(void)
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

	void _fastLioScanRGB::updateFrame(const PointCloud& pcRaw, PointCloud *pPC, const FASTLIO_SCAN_FRAME &f)
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

	bool _fastLioScanRGB::saveModel(string fModel)
	{
		IF_F(check() < 0);

		if (fModel.empty())
			fModel = m_fModel;

		IF_F(!io::WritePointCloud(fModel, m_pcModelRGB));
		LOG_I("Write point cloud: " + i2str(m_pcModelRGB.points_.size()));

		return true;
	}

	void _fastLioScanRGB::clearModel(void)
	{
		m_pcModel.Clear();
	}

	bool _fastLioScanRGB::loadCamConfig(string fConfig)
	{
		if (fConfig.empty())
			fConfig = m_fCamConfig;

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

	void _fastLioScanRGB::updateCamConfig(const FASTLIO_SCAN_CAM_INTRINSIC &c)
	{
		m_mCamInt = Matrix4d::Zero();
		m_mCamInt(0, 0) = m_camIntrinsic.m_Fx;
		m_mCamInt(1, 1) = m_camIntrinsic.m_Fy;
		m_mCamInt(0, 1) = m_camIntrinsic.m_Gamma;
		m_mCamInt(0, 2) = m_camIntrinsic.m_Cx;
		m_mCamInt(1, 2) = m_camIntrinsic.m_Cy;
	}

	bool _fastLioScanRGB::saveCamConfig(string fConfig)
	{
		if (fConfig.empty())
			fConfig = m_fCamConfig;

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

	bool _fastLioScanRGB::loadCamTraj(string fTraj)
	{
		if (fTraj.empty())
			fTraj = m_fCamTraj;

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

	void _fastLioScanRGB::setCamIntrinsic(FASTLIO_SCAN_CAM_INTRINSIC &ci)
	{
		m_camIntrinsic = ci;
	}

	FASTLIO_SCAN_CAM_INTRINSIC _fastLioScanRGB::getCamIntrinsic(void)
	{
		return m_camIntrinsic;
	}

	void _fastLioScanRGB::console(void *pConsole)
	{
		NULL_(pConsole);
		this->_ModuleBase::console(pConsole);
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
