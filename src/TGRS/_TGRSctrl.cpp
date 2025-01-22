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
		m_pWSserver = nullptr;
		m_pOM = nullptr;

		m_vPos.clear();
		m_vPtarget.set(0);
		m_vSpeed.set(0);
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

		return OK_OK;
	}

	int _TGRSctrl::link(void)
	{
//		CHECK_(this->_JSONbase::link());
		Kiss *pK = (Kiss *)m_pKiss;
		string n;

		n = "";
		pK->v("_WebSocketServer", &n);
		m_pWSserver = (_WebSocketServer *)(pK->findModule(n));
		NULL__(m_pWSserver, OK_ERR_NOT_FOUND);

		n = "";
		pK->v("_OrientalMotor", &n);
		m_pOM = (_OrientalMotor *)(pK->findModule(n));
		NULL__(m_pOM, OK_ERR_NOT_FOUND);

		return true;
	}

	int _TGRSctrl::start(void)
	{
		NULL__(m_pT, OK_ERR_NULLPTR);
		return m_pT->start(getUpdate, this);
	}

	int _TGRSctrl::check(void)
	{
		NULL__(m_pWSserver, OK_ERR_NULLPTR);
		NULL__(m_pOM, OK_ERR_NULLPTR);

		return OK_OK;
	}

	void _TGRSctrl::update(void)
	{
        string strR = "";

        while (m_pT->bAlive())
        {
			m_pT->autoFPS();

            IF_CONT(!recvJson(&strR));

			m_pT->skipSleep();

            handleJson(strR);
            strR.clear();
			m_nCMDrecv++;
        }
	}

    bool _TGRSctrl::recvJson(string* pStr)
    {
        IF_F(check() != OK_OK);
        NULL_F(pStr);

		if (m_nRead == 0)
		{
			_WebSocket* pWS = m_pWSserver->getWS(0);
			NULL_F(pWS);

			m_nRead = pWS->read(m_pBuf, JB_N_BUF);
			IF_F(m_nRead <= 0);
			m_iRead = 0;
		}

        unsigned int nStrFinish = m_msgFinishRecv.length();

		while (m_iRead < m_nRead)
		{
			*pStr += m_pBuf[m_iRead++];
			if (m_iRead == m_nRead)
			{
				m_iRead = 0;
				m_nRead = 0;
			}

            IF_CONT(pStr->length() <= nStrFinish);

            string lstr = pStr->substr(pStr->length() - nStrFinish, nStrFinish);
            IF_CONT(lstr != m_msgFinishRecv);

            pStr->erase(pStr->length() - nStrFinish, nStrFinish);
            LOG_I("Received: " + *pStr);
            return true;
		}

        return false;
    }

	void _TGRSctrl::handleJson(const string &str)
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

	void _TGRSctrl::updateTGRS(void)
	{
		IF_(check() != OK_OK);

	// 	IF_(check() != OK_OK);

	// 	object r;
	// 	JO(r, "cmd", "updatePos");
	// 	JO(r, "pX", m_vPos.x);
	// 	JO(r, "pY", m_vPos.y);
	// 	sendMsg(r);

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

			ACTUATOR_CHAN* pChanX = m_pOM->getChan(0);
			NULL_(pChanX);
			pChanX->pos()->setTarget(m_vPtarget.x);
			pChanX->speed()->setTarget(abs(m_vSpeed.x));
		}
		else if (axis == "y")
		{
			m_vSpeed.y = direction * speed;
			m_vPtarget.y = pTarget;

			ACTUATOR_CHAN* pChanY = m_pOM->getChan(1);
			NULL_(pChanY);
			pChanY->pos()->setTarget(m_vPtarget.y);
			pChanY->speed()->setTarget(abs(m_vSpeed.y));
		}
	}

	void _TGRSctrl::stop(picojson::object &jo)
	{
		m_vPtarget.set(0);
		m_vSpeed.set(0);
	}

	void _TGRSctrl::console(void *pConsole)
	{
		NULL_(pConsole);
//		this->_JSONbase::console(pConsole);
		IF_(check() < 0);

		_Console *pC = (_Console *)pConsole;
		pC->addMsg("vPos = (" + f2str(m_vPos.x) + ", " + f2str(m_vPos.y) + ")");
		pC->addMsg("vPtarget = (" + f2str(m_vPtarget.x) + ", " + f2str(m_vPtarget.y) + ")");
		pC->addMsg("vSpeed = (" + f2str(m_vSpeed.x) + ", " + f2str(m_vSpeed.y) + ")");
	}

}
