/*
 * _fastLioScan.h
 *
 *  Created on: July 17, 2024
 *      Author: yankai
 */

#ifndef OpenKAI_src_fastLioScan__fastLioScan_H_
#define OpenKAI_src_fastLioScan__fastLioScan_H_

#include <OpenKAI/Protocol/_JSONbase.h>
#include <OpenKAI/Vision/_VisionBase.h>
#include <OpenKAI/Navigation/_NavBase.h>
#include <OpenKAI/Utility/utilEvent.h>
#include <OpenKAI/Utility/utilFile.h>
#include "_fastLioScanRGB.h"

using namespace open3d;
using namespace open3d::geometry;
using namespace open3d::visualization;
using namespace Eigen;

namespace kai
{

	class _fastLioScan : public _JSONbase
	{
	public:
		_fastLioScan(void);
		virtual ~_fastLioScan();

		bool init(void *pKiss);
		bool link(void);
		bool start(void);
		int check(void);
		void console(void *pConsole);

		void scanStart(void);
		void scanShutter(void);
		void scanStop(void);
		void scanSave(void);
		void scanClear(void);

		void scanStart(picojson::object& jo);
		void scanStop(picojson::object& jo);
		void exportModel(picojson::object& jo);
		void setParam(picojson::object& jo);
		void saveParam(picojson::object& jo);
		void getParam(picojson::object& jo);
		void getParamSaved(picojson::object& jo);

	private:
		void updateScan(void);
		void update(void);
		static void *getUpdate(void *This)
		{
			((_fastLioScan *)This)->update();
			return NULL;
		}

		void handleMsg(const string &str);
		void updateR(void);
		static void *getUpdateR(void *This)
		{
			((_fastLioScan *)This)->updateR();
			return NULL;
		}

	protected:
		_VisionBase* m_pV;
		_NavBase* m_pNav;
		_fastLioScanRGB* m_pFLSrgb;

		int m_iTake;
		INTERVAL_EVENT m_ieShutter;
		picojson::array m_jArrCamTraj;

		string m_cmdROStrigger;
		string m_cmdOnSaved;

		bool m_bScanning;
		string m_dir;
		string m_projDir;
		bool m_bFlipRGB;
		vector<int> m_compress;
		int m_quality;
	};

}
#endif
