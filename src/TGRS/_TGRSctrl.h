/*
 * _TGRSctrl.h
 *
 *  Created on: July 17, 2024
 *      Author: yankai
 */

#ifndef OpenKAI_src_TGRS__TGRSctrl_H_
#define OpenKAI_src_TGRS__TGRSctrl_H_

#include <OpenKAI/Utility/util.h>
#include <OpenKAI/Protocol/_JSONbase.h>
#include <OpenKAI/Sensor/Distance/_DistSensorBase.h>
#include <OpenKAI/Actuator/Motor/_DDSM.h>

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

		void moveStep(picojson::object& jo);
		void moveStop(picojson::object& jo);

	private:
		void setMotSpdX(float s);
		void setMotSpdY(float s);

		void updateTGRS(void);
		void update(void);
		static void *getUpdate(void *This)
		{
			((_TGRSctrl *)This)->update();
			return NULL;
		}

		void handleMsg(const string &str);
		void updateR(void);
		static void *getUpdateR(void *This)
		{
			((_TGRSctrl *)This)->updateR();
			return NULL;
		}

	private:
		_DistSensorBase* m_pDSx;
		_DistSensorBase* m_pDSy;
		vector<_DDSM*> m_vMotX;
		vector<_DDSM*> m_vMotY;

		bool m_bMove;

		vFloat2 m_vPos;
		vFloat2 m_vPtarget;
		float m_posDZ;	// pos dead zone
		float m_motSpd;

		// region
		vFloat2 m_vRx;
		vFloat2 m_vRy;

	};

}
#endif
