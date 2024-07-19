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
		vDouble3 m_vCSoffset = {0,0,0};
	};

	struct FASTLIO_SCAN_FRAME
	{
		Mat m_mRGB;
		vFloat4 m_vQ = {0,0,0,0};
		vFloat3 m_vT = {0,0,0};
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

		bool openModel(string fModel = "");
		void updateModel(void);
		bool saveModel(string fModel = "");
		void clearModel(void);

		bool loadCamConfig(string fConfig = "");
		void updateCamConfig(const FASTLIO_SCAN_CAM_INTRINSIC& c);
		bool saveCamConfig(string fConfig = "");

		bool loadCamTraj(string fTraj = "");

		void setCamIntrinsic(FASTLIO_SCAN_CAM_INTRINSIC& ci);
		FASTLIO_SCAN_CAM_INTRINSIC getCamIntrinsic(void);

	private:
		void updateFrame(const PointCloud& pcRaw, PointCloud* pPC, const FASTLIO_SCAN_FRAME& f);
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
		PointCloud m_pcModelRGB;
		_PCframe* m_pPC;

		FASTLIO_SCAN_CAM_INTRINSIC m_camIntrinsic;
		Matrix4d m_mCamInt;
		Matrix4d m_mCSoffset;	// cam sensor offset

		vector<FASTLIO_SCAN_FRAME> m_vFrame;

	};

}
#endif
