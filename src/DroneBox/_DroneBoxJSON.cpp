#include "_DroneBoxJSON.h"

namespace kai
{

    _DroneBoxJSON::_DroneBoxJSON()
    {
        m_pDB = NULL;
    }

    _DroneBoxJSON::~_DroneBoxJSON()
    {
        DEL(m_pTr);
    }

    int _DroneBoxJSON::init(void *pKiss)
    {
        CHECK_(this->_JSONbase::init(pKiss));
        Kiss *pK = (Kiss *)pKiss;

        return OK_OK;
    }

    int _DroneBoxJSON::link(void)
    {
        CHECK_(this->_JSONbase::link());
        CHECK_(m_pTr->link());

        Kiss *pK = (Kiss *)m_pKiss;

        string n;
        n = "";
        pK->v("_DroneBox", &n);
        m_pDB = (_DroneBox *)(pK->findModule(n));
        NULL__(m_pDB, OK_ERR_NOT_FOUND);

        return OK_OK;
    }

    int _DroneBoxJSON::start(void)
    {
        NULL__(m_pT, OK_ERR_NULLPTR);
        NULL__(m_pTr, OK_ERR_NULLPTR);
        CHECK_(m_pT->start(getUpdateW, this));
        return m_pTr->start(getUpdateR, this);
    }

    int _DroneBoxJSON::check(void)
    {
        NULL__(m_pDB, OK_ERR_NULLPTR);

        return this->_JSONbase::check();
    }

    void _DroneBoxJSON::updateW(void)
    {
        while (m_pT->bAlive())
        {
            m_pT->autoFPS();

            send();
        }
    }

    void _DroneBoxJSON::send(void)
    {
        IF_(check() != OK_OK);

        if (m_ieSendHB.update(m_pT->getTfrom()))
        {
            sendHeartbeat();
        }

        sendStat();
    }

    void _DroneBoxJSON::sendHeartbeat(void)
    {
        vDouble2 vP = m_pDB->getPos();

        object o;
        JO(o, "id", (double)m_pDB->getID());
        JO(o, "cmd", "heartbeat");
        JO(o, "t", li2str(m_pT->getTfrom()));
        JO(o, "lat", lf2str(vP.x, 10));
        JO(o, "lng", lf2str(vP.y, 10));

        sendJson(o);
    }

    void _DroneBoxJSON::sendStat(void)
    {
        object o;
        JO(o, "id", (double)m_pDB->getID());
        JO(o, "cmd", "stat");
        JO(o, "stat", m_pDB->getState());
        sendJson(o);
    }

    void _DroneBoxJSON::updateR(void)
    {
        string strR = "";

        while (m_pTr->bAlive())
        {
            IF_CONT(!recvJson(&strR));

            handleJson(strR);
            strR.clear();
			m_nCMDrecv++;
        }
    }

    void _DroneBoxJSON::handleJson(const string &str)
    {
        value json;
        IF_(!str2JSON(str, &json));

        object &jo = json.get<object>();
        IF_(!jo["cmd"].is<string>());
        string cmd = jo["cmd"].get<string>();

        if (cmd == "heartbeat")
            heartbeat(jo);
        else if (cmd == "stat")
            stat(jo);
        else if (cmd == "req")
            req(jo);
    }

    void _DroneBoxJSON::heartbeat(picojson::object &o)
    {
        IF_(check() != OK_OK);
    }

    void _DroneBoxJSON::stat(picojson::object &o)
    {
        IF_(check() != OK_OK);
        IF_(!o["id"].is<double>());
        IF_(!o["stat"].is<string>());

        int vID = o["id"].get<double>();
        string stat = o["stat"].get<string>();

        if (stat == "RECOVER" ||
            stat == "STANDBY" ||
            stat == "AIRBORNE" ||
            stat == "LANDED"
        )
        {
            m_pDB->setState(stat);
        }
    }

    void _DroneBoxJSON::req(picojson::object &o)
    {
        IF_(check() != OK_OK);
        IF_(!o["id"].is<double>());
        IF_(!o["do"].is<string>());

        int vID = o["id"].get<double>();
        string d = o["do"].get<string>();

        object jo;
        JO(jo, "id", (double)m_pDB->getID());
        JO(jo, "cmd", "ack");
        JO(jo, "do", d);

        if (d == "takeoff")
        {
            if (m_pDB->takeoffRequest())
            {
                JO(jo, "r", "ok");
            }
            else
            {
                JO(jo, "r", "denied");
            }
        }
        else if (d == "landing")
        {
            if (m_pDB->landingRequest())
            {
                JO(jo, "r", "ok");
            }
            else
            {
                JO(jo, "r", "denied");
            }
        }

        sendJson(jo);
    }

    void _DroneBoxJSON::console(void *pConsole)
    {
        NULL_(pConsole);
        this->_JSONbase::console(pConsole);

        NULL_(m_pTr);
        m_pTr->console(pConsole);
    }

}
