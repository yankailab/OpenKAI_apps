/*
 * _TGRSctrl.h
 *
 *  Created on: July 17, 2024
 *      Author: yankai
 */

#ifndef OpenKAI_src_TGRS__TGRSctrl_H_
#define OpenKAI_src_TGRS__TGRSctrl_H_

//#include <OpenKAI/Utility/util.h>
#include <OpenKAI/Protocol/_JSONbase.h>
#include <OpenKAI/IO/_WebSocketServer.h>
#include <OpenKAI/Actuator/_OrientalMotor.h>

namespace kai
{

	class _TGRSctrl : public _JSONbase
	{
	public:
		_TGRSctrl(void);
		virtual ~_TGRSctrl();

		int init(void *pKiss);
		int link(void);
		int start(void);
		int check(void);
		void console(void *pConsole);

		void move(picojson::object& jo);
		void stop(picojson::object& jo);

	protected:
		bool recvJson(string* pStr);
		void handleJson(const string &str);

	private:
		void updateTGRS(void);
		void update(void);
		static void *getUpdate(void *This)
		{
			((_TGRSctrl *)This)->update();
			return NULL;
		}

	private:
		_WebSocketServer* m_pWSserver;
		_OrientalMotor* m_pOM;

		vFloat2 m_vPos;
		vFloat2 m_vPtarget;
		vFloat2 m_vSpeed;
	};

}
#endif
