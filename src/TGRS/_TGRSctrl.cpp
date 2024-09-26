/*
 * _TGRSctrl.cpp
 *
 *  Created on: July 17, 2024
 *      Author: yankai
 */

#include "_TGRSctrl.h"

namespace kai
{

	_TGRSctrl::_TGRSctrl()
	{
		m_pDSx = nullptr;
		m_pDSy = nullptr;
		m_bMove = false;

		m_vPos.clear();
		m_vPtarget.clear();
		m_posDZ = 0.01;
		m_motSpd = 10;	// rpm
		m_vRx.set(0, 2);
		m_vRy.set(0, 2);
	}

	_TGRSctrl::~_TGRSctrl()
	{
	}

	int _TGRSctrl::init(void *pKiss)
	{
		CHECK_(this->_JSONbase::init(pKiss));
		Kiss *pK = (Kiss *)pKiss;

		pK->v("posDZ", &m_posDZ);
		pK->v("vPtarget", &m_vPtarget);
		pK->v("motSpd", &m_motSpd);
		pK->v("vRx", &m_vRx);
		pK->v("vRy", &m_vRy);
		pK->v("bMove", &m_bMove);

		return OK_OK;
	}

	int _TGRSctrl::link(void)
	{
		CHECK_(this->_JSONbase::link());

		Kiss *pK = (Kiss *)m_pKiss;
		string n;

		n = "";
		pK->v("_DistSensorBase", &n);
		m_pDSx = (_DistSensorBase *)(pK->findModule(n));
		NULL__(m_pDSx, OK_ERR_NOT_FOUND);

		n = "";
		pK->v("_DistSensorBase", &n);
		m_pDSy = (_DistSensorBase *)(pK->findModule(n));
		NULL__(m_pDSy, OK_ERR_NOT_FOUND);

		vector<string> vStr;
		pK->a("vDDSMx", &vStr);
		for (string m : vStr)
		{
			_DDSM *pM = (_DDSM *)pK->findModule(m);
			if (!pM)
			{
				LOG_I("Motor not found: " + m);
				continue;
			}

			m_vMotX.push_back(pM);
		}

		vStr.clear();
		pK->a("vDDSMy", &vStr);
		for (string m : vStr)
		{
			_DDSM *pM = (_DDSM *)pK->findModule(m);
			if (!pM)
			{
				LOG_I("Motor not found: " + m);
				continue;
			}

			m_vMotY.push_back(pM);
		}

		return true;
	}

	int _TGRSctrl::start(void)
	{
		NULL__(m_pT, OK_ERR_NULLPTR);
		NULL__(m_pTr, OK_ERR_NULLPTR);

		CHECK_(m_pT->start(getUpdate, this));
		return m_pTr->start(getUpdateR, this);
	}

	int _TGRSctrl::check(void)
	{
		NULL__(m_pDSx, OK_ERR_NULLPTR);
		NULL__(m_pDSy, OK_ERR_NULLPTR);

		return this->_JSONbase::check();
	}

	void _TGRSctrl::update(void)
	{
		while (m_pT->bAlive())
		{
			m_pT->autoFPSfrom();

			updateTGRS();

			m_pT->autoFPSto();
		}
	}

	void _TGRSctrl::updateTGRS(void)
	{
		IF_(check() < 0);

		if (!m_bMove)
		{
			setMotSpdX(0);
			setMotSpdY(0);
			return;
		}

		float dP;

		dP = m_vPtarget.x - m_vPos.x;
		if(abs(dP) > m_posDZ)
			setMotSpdX(m_motSpd * sign(dP));
		else
			setMotSpdX(0);

		dP = m_vPtarget.y - m_vPos.y;
		if(abs(dP) > m_posDZ)
			setMotSpdY(m_motSpd * sign(dP));
		else
			setMotSpdY(0);

	}

	void _TGRSctrl::setMotSpdX(float s)
	{
		for(_DDSM* pD : m_vMotX)
			pD->setStarget(s);
	}

	void _TGRSctrl::setMotSpdY(float s)
	{
		for(_DDSM* pD : m_vMotY)
			pD->setStarget(s);
	}

	void _TGRSctrl::updateR(void)
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

	void _TGRSctrl::handleMsg(const string &str)
	{
		value json;
		IF_(!str2JSON(str, &json));

		object &jo = json.get<object>();
		IF_(!jo["cmd"].is<string>());
		string cmd = jo["cmd"].get<string>();

		if (cmd == "moveStep")
			moveStep(jo);
		else if (cmd == "moveStop")
			moveStop(jo);
	}

	void _TGRSctrl::moveStep(picojson::object &jo)
	{
		IF_(!jo["pTargetX"].is<double>());
		m_vPtarget.x = constrain<float>(jo["pTargetX"].get<double>(), m_vRx.x, m_vRx.y);

		IF_(!jo["pTargetY"].is<double>());
		m_vPtarget.y = constrain<float>(jo["pTargetY"].get<double>(), m_vRy.x, m_vRy.y);
	}

	void _TGRSctrl::moveStop(picojson::object &jo)
	{
		IF_(!jo["bMove"].is<double>());
		m_bMove = jo["pTargetX"].get<double>();
	}

	void _TGRSctrl::console(void *pConsole)
	{
		NULL_(pConsole);
		this->_JSONbase::console(pConsole);
		IF_(check() < 0);

		_Console *pC = (_Console *)pConsole;
		pC->addMsg("vPos = (" + f2str(m_vPos.x) + ", " + f2str(m_vPos.y) + ")");
		pC->addMsg("vPtarget = (" + f2str(m_vPtarget.x) + ", " + f2str(m_vPtarget.y) + ")");
		pC->addMsg("motSpd = " + f2str(m_motSpd) + ")");
		pC->addMsg("vRx = (" + f2str(m_vRx.x) + ", " + f2str(m_vRx.y) + ")");
		pC->addMsg("vRy = (" + f2str(m_vRy.x) + ", " + f2str(m_vRy.y) + ")");
	}

}
