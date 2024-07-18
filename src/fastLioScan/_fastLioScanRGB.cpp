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

			updateModel();

			m_pT->autoFPSto();
		}
	}

	void _fastLioScanRGB::updateModel(void)
	{
		IF_(check() < 0);
	}

	bool _fastLioScanRGB::openModel(const string &fModel)
	{
		IF_F(fModel.empty());

		clearModel();

		//	io::ReadPointCloudOption ro;
		IF_F(!io::ReadPointCloud(fModel, m_pcModel));
		LOG_I("Read point cloud: " + i2str(m_pcModel.points_.size()));

		return true;
	}

	void _fastLioScanRGB::clearModel(void)
	{
		m_jArray.clear();
	}

	bool _fastLioScanRGB::loadCamConfig(const string &fConfig)
	{
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
		m_camIntrinsic.m_Cx = ci["Cx"].get<double>();

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

	bool _fastLioScanRGB::saveCamConfig(const string &fConfig)
	{
		picojson::object o;
		o.insert(make_pair("name", "camIntrinsic"));
		o.insert(make_pair("Fx", value(m_camIntrinsic.m_Fx)));
		o.insert(make_pair("Fy", value(m_camIntrinsic.m_Fy)));
		o.insert(make_pair("Gamma", value(m_camIntrinsic.m_Gamma)));
		o.insert(make_pair("Cx", value(m_camIntrinsic.m_Cx)));
		o.insert(make_pair("Cy", value(m_camIntrinsic.m_Cy)));

		string f = picojson::value(o).serialize();

		return writeFile(fConfig, f);
	}

	bool _fastLioScanRGB::loadCamTraj(const string &fTraj)
	{
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
		picojson::array &ary = obj["camTraj"].get<picojson::array>();
		for (value &rec : ary)
		{
			object &o = rec.get<object>();
			IF_CONT(o["fImg"].is<string>());
			IF_CONT(o["vmT"].is<value::array>());

			string fImg = o["fImg"].get<string>();
			Mat m = imread(fImg);
			IF_CONT(m.empty());

			FASTLIO_SCAN_FRAME f;
			f.m_mRGB = m;

			f.m_mT = Matrix4d::Zero();
			value::array amT = o["vmT"].get<value::array>();
			int i = 0;
			f.m_mT(0, 0) = amT[i++].get<double>();
			f.m_mT(0, 1) = amT[i++].get<double>();
			f.m_mT(0, 2) = amT[i++].get<double>();
			f.m_mT(0, 3) = amT[i++].get<double>();

			f.m_mT(1, 0) = amT[i++].get<double>();
			f.m_mT(1, 1) = amT[i++].get<double>();
			f.m_mT(1, 2) = amT[i++].get<double>();
			f.m_mT(1, 3) = amT[i++].get<double>();

			f.m_mT(2, 0) = amT[i++].get<double>();
			f.m_mT(2, 1) = amT[i++].get<double>();
			f.m_mT(2, 2) = amT[i++].get<double>();
			f.m_mT(2, 3) = amT[i++].get<double>();

			f.m_mT(3, 0) = amT[i++].get<double>();
			f.m_mT(3, 1) = amT[i++].get<double>();
			f.m_mT(3, 2) = amT[i++].get<double>();
			f.m_mT(3, 3) = amT[i++].get<double>();

			m_vFrame.push_back(f);
		}

		return true;
	}

	void _fastLioScanRGB::console(void *pConsole)
	{
		NULL_(pConsole);
		this->_ModuleBase::console(pConsole);
		IF_(check() < 0);

		_Console *pC = (_Console *)pConsole;
		//		pC->addMsg("Uploading: " + m_fName);
	}

}
