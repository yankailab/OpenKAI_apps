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

		m_vPos.clear();
		m_vPtarget.set(-1);
		m_vSpeed.set(0);
		m_vRx.set(0, 1);
		m_vRy.set(0, 1);
		m_posDZ = 0.01;
	}

	_TGRSctrl::~_TGRSctrl()
	{
	}

	int _TGRSctrl::init(void *pKiss)
	{
		CHECK_(this->_JSONbase::init(pKiss));
		Kiss *pK = (Kiss *)pKiss;

		pK->v("vPtarget", &m_vPtarget);
		pK->v("vSpeed", &m_vSpeed);
		pK->v("vRx", &m_vRx);
		pK->v("vRy", &m_vRy);
		pK->v("posDZ", &m_posDZ);

		int nFl;
		nFl = 5;
		pK->v("nMedFilter", &nFl);
		m_flMedX.init(nFl);
		m_flMedY.init(nFl);

		nFl = 3;
		pK->v("nAvrFilter", &nFl);
		m_flAvrX.init(nFl);
		m_flAvrY.init(nFl);

		return OK_OK;
	}

	int _TGRSctrl::link(void)
	{
		CHECK_(this->_JSONbase::link());

		Kiss *pK = (Kiss *)m_pKiss;
		string n;

		n = "";
		pK->v("_DistSensorBaseX", &n);
		m_pDSx = (_DistSensorBase *)(pK->findModule(n));
		NULL__(m_pDSx, OK_ERR_NOT_FOUND);

		n = "";
		pK->v("_DistSensorBaseY", &n);
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
			sendUpdate();

			m_pT->autoFPSto();
		}
	}

	void _TGRSctrl::updateTGRS(void)
	{
		IF_(check() != OK_OK);

		float pX = m_pDSx->d(0);
		float pY = m_pDSy->d(0);

		if (m_flAvrX.update(m_flMedX.update(&pX)))
			m_vPos.x = m_flAvrX.v();
		else
			m_vPos.x = pX;

		if (m_flAvrY.update(m_flMedY.update(&pY)))
			m_vPos.y = m_flAvrY.v();
		else
			m_vPos.y = pY;


		// X axis
		if (m_vPtarget.x > 0)
		{
			float dP = m_vPtarget.x - m_vPos.x;
			if (abs(dP) > m_posDZ)
				setMotSpdX(m_vSpeed.x * sign(dP));
			else
				setMotSpdX(0);
		}
		else
		{
			setMotSpdX(m_vSpeed.x);
		}

		// Y axis
		if (m_vPtarget.y > 0)
		{
			float dP = m_vPtarget.y - m_vPos.y;
			if (abs(dP) > m_posDZ)
			{
				setMotSpdY(m_vSpeed.y * sign(dP));
			}
			else
				setMotSpdY(0);
		}
		else
		{
			setMotSpdY(m_vSpeed.y);
		}
	}

	void _TGRSctrl::sendUpdate(void)
	{
		IF_(check() != OK_OK);

		object r;
		JO(r, "cmd", "updatePos");
		JO(r, "pX", m_vPos.x);
		JO(r, "pY", m_vPos.y);
		sendMsg(r);
	}

	void _TGRSctrl::setMotSpdX(float s)
	{
		for (_DDSM *pD : m_vMotX)
			pD->setStarget(s);
	}

	void _TGRSctrl::setMotSpdY(float s)
	{
		for (_DDSM *pD : m_vMotY)
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

		if (cmd == "move")
			move(jo);
		else if (cmd == "stop")
			stop(jo);
	}

	void _TGRSctrl::move(picojson::object &jo)
	{
		IF_(!jo["axis"].is<string>());
		string axis = jo["axis"].get<string>();

		IF_(!jo["direction"].is<double>());
		float direction = jo["direction"].get<double>();

		IF_(!jo["speed"].is<double>());
		float speed = jo["speed"].get<double>();

		IF_(!jo["pTarget"].is<double>());
		float pTarget = jo["pTarget"].get<double>();

		if (axis == "x")
		{
			m_vSpeed.x = direction * speed;
			m_vPtarget.x = pTarget;
			//			m_vPtarget.x = constrain<float>(pTarget, m_vRx.x, m_vRx.y);
		}
		else if (axis == "y")
		{
			m_vSpeed.y = direction * speed;
			m_vPtarget.y = pTarget;
			//			m_vPtarget.y = constrain<float>(pTarget, m_vRy.x, m_vRy.y);
		}
	}

	void _TGRSctrl::stop(picojson::object &jo)
	{
		m_vPtarget.set(-1);
		m_vSpeed.set(0);
	}

	void _TGRSctrl::console(void *pConsole)
	{
		NULL_(pConsole);
		this->_JSONbase::console(pConsole);
		IF_(check() < 0);

		_Console *pC = (_Console *)pConsole;
		pC->addMsg("vPos = (" + f2str(m_vPos.x) + ", " + f2str(m_vPos.y) + ")");
		pC->addMsg("vPtarget = (" + f2str(m_vPtarget.x) + ", " + f2str(m_vPtarget.y) + ")");
		pC->addMsg("vSpeed = (" + f2str(m_vSpeed.x) + ", " + f2str(m_vSpeed.y) + ")");
		pC->addMsg("posDZ = " + f2str(m_posDZ));
	}

}
