/*
 * _fastLioScan.cpp
 *
 *  Created on: July 17, 2024
 *      Author: yankai
 */

#include "_fastLioScan.h"

namespace kai
{

	_fastLioScan::_fastLioScan()
	{
		m_pV = nullptr;
		m_pNav = nullptr;

		m_cmdROStrigger = "";
		m_cmdOnSaved = "";

		m_bScanning = false;
		m_dir = "/home/";
		m_projDir = "";
		m_bFlipRGB = false;
		m_quality = 100;

		scanClear();
	}

	_fastLioScan::~_fastLioScan()
	{
	}

	bool _fastLioScan::init(void *pKiss)
	{
		IF_F(!this->_JSONbase::init(pKiss));
		Kiss *pK = (Kiss *)pKiss;

		pK->v("cmdROStrigger", &m_cmdROStrigger);
		pK->v("cmdOnSaved", &m_cmdOnSaved);

		float tInt = 0.5;
		pK->v("tInt", &tInt);
		m_ieShutter.init(tInt * (float)USEC_1SEC);

		pK->v("dir", &m_dir);
		pK->v("bFlipRGB", &m_bFlipRGB);
		pK->v("quality", &m_quality);
		m_compress.push_back(IMWRITE_JPEG_QUALITY);
		m_compress.push_back(m_quality);

		return true;
	}

	bool _fastLioScan::link(void)
	{
		IF_F(!this->_JSONbase::link());

		Kiss *pK = (Kiss *)m_pKiss;
		string n;

		n = "";
		pK->v("_VisionBase", &n);
		m_pV = (_VisionBase *)(pK->findModule(n));
		IF_Fl(!m_pV, n + ": not found");

		n = "";
		pK->v("_NavBase", &n);
		m_pNav = (_NavBase *)(pK->findModule(n));
		IF_Fl(!m_pNav, n + ": not found");

		return true;
	}

	bool _fastLioScan::start(void)
	{
		NULL_F(m_pT);
		NULL_F(m_pTr);

		IF_F(!m_pT->start(getUpdate, this));
		IF_F(!m_pTr->start(getUpdateR, this));

		return true;
	}

	int _fastLioScan::check(void)
	{
		NULL__(m_pV, -1);
		NULL__(m_pNav, -1);

		return this->_JSONbase::check();
	}

	void _fastLioScan::update(void)
	{
		while (m_pT->bAlive())
		{
			m_pT->autoFPSfrom();

			updateScan();

			m_pT->autoFPSto();
		}
	}

	void _fastLioScan::updateScan(void)
	{
		IF_(check() < 0);

		uint64_t tNow = getTbootUs();

		if (m_ieShutter.update(tNow))
		{
			scanShutter();
		}
	}

	void _fastLioScan::scanStart(void)
	{
		m_projDir = m_dir + tFormat() + "/";
		string cmd;
		cmd = "mkdir " + m_projDir;
		system(cmd.c_str());

		scanClear();
		m_bScanning = true;
	}

	void _fastLioScan::scanShutter(void)
	{
		IF_(check() < 0);
		IF_(!m_bScanning);

		string fName;
		Frame fBGR = *m_pV->getFrameRGB();
		if (m_bFlipRGB)
			fBGR = fBGR.flip(-1);
		Mat mBGR;
		fBGR.m()->copyTo(mBGR);
		IF_(mBGR.empty());

		vFloat4 vQ = m_pNav->q();
		vFloat3 vT = m_pNav->t();

		fName = m_projDir + i2str(m_iTake) + ".jpg";
		cv::imwrite(fName, mBGR, m_compress);
		// cmd = "exiftool -overwrite_original -GPSLongitude=\"" + lon + "\" -GPSLatitude=\"" + lat + "\" " + fName;
		// system(cmd.c_str());

		picojson::object o;
		o.insert(make_pair("fImg", fName));

		picojson::array jQ;
		jQ.push_back(value(vQ.x));
		jQ.push_back(value(vQ.y));
		jQ.push_back(value(vQ.z));
		jQ.push_back(value(vQ.w));
		o.insert(make_pair("vQ", jQ));

		picojson::array jT;
		jT.push_back(value(vT.x));
		jT.push_back(value(vT.y));
		jT.push_back(value(vT.z));
		o.insert(make_pair("vT", jT));

		m_jArrCamTraj.push_back(value(o));

		LOG_I("Take: " + i2str(m_iTake));
		m_iTake++;
	}

	void _fastLioScan::scanStop(void)
	{
		m_bScanning = false;
	}

	void _fastLioScan::scanSave(void)
	{
		if (!m_cmdROStrigger.empty())
			system(m_cmdROStrigger.c_str());

		picojson::object o;
		o.insert(make_pair("imgTraj", m_jArrCamTraj));

		string k = picojson::value(o).serialize();
		string fName = m_projDir + "traj.json";
		writeFile(fName, k);

		if (!m_cmdOnSaved.empty())
		{
			string cmd = replace(m_cmdOnSaved, "[dirProj]", m_projDir);
			system(cmd.c_str());
		}
	}

	void _fastLioScan::scanClear(void)
	{
		m_iTake = 0;
		m_jArrCamTraj.clear();
	}

	void _fastLioScan::updateR(void)
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

	void _fastLioScan::handleMsg(const string &str)
	{
		value json;
		IF_(!str2JSON(str, &json));

		object &jo = json.get<object>();
		IF_(!jo["cmd"].is<string>());
		string cmd = jo["cmd"].get<string>();

		if (cmd == "scanStart")
			scanStart(jo);
		else if (cmd == "scanStop")
			scanStop(jo);
	}

	void _fastLioScan::scanStart(picojson::object &jo)
	{
		scanStart();
	}

	void _fastLioScan::scanStop(picojson::object &jo)
	{
		scanStop();
		scanSave();
	}

	void _fastLioScan::console(void *pConsole)
	{
		NULL_(pConsole);
		this->_JSONbase::console(pConsole);
		IF_(check() < 0);

		_Console *pC = (_Console *)pConsole;
		pC->addMsg("iTake: " + i2str(m_iTake));
	}

}
