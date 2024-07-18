/*
 * _fastLioScanRGB.h
 *
 *  Created on: July 17, 2024
 *      Author: yankai
 */

#ifndef OpenKAI_src_fastLioScan__fastLioScanRGB_H_
#define OpenKAI_src_fastLioScan__fastLioScanRGB_H_

#include <OpenKAI/Base/_ModuleBase.h>
#include <OpenKAI/3D/PointCloud/_PCframe.h>
#include <OpenKAI/Utility/utilEvent.h>

using namespace picojson;
using namespace open3d;
using namespace open3d::geometry;
using namespace open3d::visualization;
using namespace Eigen;

namespace kai
{
	struct FASTLIO_SCAN_CAM_INTRINSIC
	{
		double m_Fx = 0;
		double m_Fy = 0;
		double m_Gamma = 0;
		double m_Cx = 0.5;
		double m_Cy = 0.5;
	};

	struct FASTLIO_SCAN_FRAME
	{
		Mat m_mRGB;
		Matrix4d m_mT;
	};

	class _fastLioScanRGB : public _ModuleBase
	{
	public:
		_fastLioScanRGB(void);
		virtual ~_fastLioScanRGB();

		bool init(void *pKiss);
		bool link(void);
		bool start(void);
		int check(void);
		void console(void *pConsole);

		bool openModel(const string& fModel);
		void updateModel(void);
		bool saveModel(const string& fModel);
		void clearModel(void);

		bool loadCamConfig(const string& fConfig);
		void updateCamConfig(const FASTLIO_SCAN_CAM_INTRINSIC& c);
		bool saveCamConfig(const string& fConfig);

		bool loadCamTraj(const string& fTraj);

	private:
		void update(void);
		static void *getUpdate(void *This)
		{
			((_fastLioScanRGB *)This)->update();
			return NULL;
		}

	protected:
		string m_fModel;
		string m_fCamConfig;
		string m_fCamTraj;

		PointCloud m_pcModel;
		_PCframe* m_pPC;

		FASTLIO_SCAN_CAM_INTRINSIC m_camIntrinsic;
		Matrix4d m_mCamInt;
		Matrix4d m_mCSoffset;	// cam sensor offset
		Matrix4d m_mW2C;
		Matrix4d m_mW2S;

		picojson::array m_jArray;
		vector<FASTLIO_SCAN_FRAME> m_vFrame;

	};

}
#endif
