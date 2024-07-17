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
#include <OpenKAI/3D/PointCloud/_PCframe.h>
#include <OpenKAI/Navigation/_NavBase.h>
#include <OpenKAI/Utility/utilEvent.h>

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

		void scanShutter(void);
		void scanSave(void);
		void scanClear(void);

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
		_PCframe* m_pPC;
		_NavBase* m_pNav;

		int m_iTake;
		INTERVAL_EVENT m_ieShutter;
		picojson::object m_jo;
		picojson::array m_jArray;

		string m_cmdROStrigger;
		string m_cmdOnSaved;

		string m_dir;
		string m_subDir;
		bool m_bFlipRGB;
		vector<int> m_compress;
		int m_quality;
	};

}
#endif
