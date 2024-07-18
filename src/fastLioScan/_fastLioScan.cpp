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

		m_dir = "/home/";
		m_subDir = "";
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
		pK->v("subDir", &m_subDir);
		if (m_subDir.empty())
			m_subDir = m_dir + tFormat() + "/";
		else
			m_subDir = m_dir + m_subDir;

		string cmd;
		cmd = "mkdir " + m_subDir;
		system(cmd.c_str());

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

		return 0;
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

		if (m_iTake > 10)
		{
			scanSave();
			scanClear();
			exit(0);
		}
	}

	void _fastLioScan::scanShutter(void)
	{
		IF_(check() < 0);

		string fName;
		Frame fBGR = *m_pV->getFrameRGB();
		if (m_bFlipRGB)
			fBGR = fBGR.flip(-1);
		Mat mBGR;
		fBGR.m()->copyTo(mBGR);
		IF_(mBGR.empty());

		Matrix4f mT = m_pNav->mT();

		fName = m_subDir + i2str(m_iTake) + ".jpg";
		cv::imwrite(fName, mBGR, m_compress);
		// cmd = "exiftool -overwrite_original -GPSLongitude=\"" + lon + "\" -GPSLatitude=\"" + lat + "\" " + fName;
		// system(cmd.c_str());

		picojson::object o;
		o.insert(make_pair("fName", fName));

		picojson::array vT;
		vT.push_back(value(mT(0, 0)));
		vT.push_back(value(mT(0, 1)));
		vT.push_back(value(mT(0, 2)));
		vT.push_back(value(mT(0, 3)));

		vT.push_back(value(mT(1, 0)));
		vT.push_back(value(mT(1, 1)));
		vT.push_back(value(mT(1, 2)));
		vT.push_back(value(mT(1, 3)));

		vT.push_back(value(mT(2, 0)));
		vT.push_back(value(mT(2, 1)));
		vT.push_back(value(mT(2, 2)));
		vT.push_back(value(mT(2, 3)));

		vT.push_back(value(mT(3, 0)));
		vT.push_back(value(mT(3, 1)));
		vT.push_back(value(mT(3, 2)));
		vT.push_back(value(mT(3, 3)));

		o.insert(make_pair("mT", value(vT)));
		m_jArray.push_back(value(o));

		LOG_I("Take: " + i2str(m_iTake));
		m_iTake++;
	}

	void _fastLioScan::scanSave(void)
	{
		if (!m_cmdROStrigger.empty())
			system(m_cmdROStrigger.c_str());

		string k = picojson::value(m_jArray).serialize();
		string fName = m_subDir + "config.json";
		writeFile(fName, k);

		if (!m_cmdOnSaved.empty())
			system(m_cmdOnSaved.c_str());
	}

	void _fastLioScan::scanClear(void)
	{
		m_iTake = 0;
		m_jo.clear();
		m_jArray.clear();
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

		// if (cmd == "updateGrid")
		// 	updateGrid(jo);
		// else if (cmd == "setGrid")
		// 	setGrid(jo);
	}

	void _fastLioScan::console(void *pConsole)
	{
		NULL_(pConsole);
		this->_JSONbase::console(pConsole);
		IF_(check() < 0);

		_Console *pC = (_Console *)pConsole;
		//		pC->addMsg("Uploading: " + m_fName);
	}

}
