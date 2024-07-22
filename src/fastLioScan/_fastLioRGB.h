/*
 * _fastLioRGB.h
 *
 *  Created on: July 17, 2024
 *      Author: yankai
 */

#ifndef OpenKAI_src_fastLioScan__fastLioRGB_H_
#define OpenKAI_src_fastLioScan__fastLioRGB_H_

#include <OpenKAI/Protocol/_JSONbase.h>
#include <OpenKAI/3D/PointCloud/_PCframe.h>
#include <OpenKAI/Utility/utilEvent.h>

using namespace picojson;
using namespace open3d;
using namespace open3d::geometry;
using namespace open3d::visualization;
using namespace Eigen;

namespace kai
{
	struct FASTLIO_RGB_CAM_INTRINSIC
	{
		double m_Fx = 0;
		double m_Fy = 0;
		double m_Gamma = 0;
		double m_Cx = 0.5;
		double m_Cy = 0.5;
		vDouble3 m_vCSt = {0,0,0};
		vDouble3 m_vCSr = {0,0,0};
	};

	struct FASTLIO_RGB_FRAME
	{
		Mat m_mRGB;
		vFloat4 m_vQ = {0,0,0,0};
		vFloat3 m_vT = {0,0,0};
	};

	struct FASTLIO_RGB_PC
	{
		float m_d = -0.1;
	};

	class _fastLioRGB : public _JSONbase
	{
	public:
		_fastLioRGB(void);
		virtual ~_fastLioRGB();

		bool init(void *pKiss);
		bool link(void);
		bool start(void);
		int check(void);
		void console(void *pConsole);

		bool loadModel(string fModel = "");
		void updateModel(void);
		bool saveModel(string fModel = "");
		void clearModel(void);

		bool loadCamConfig(string fConfig = "");
		bool saveCamConfig(string fConfig = "");
		bool loadCamTraj(string fTraj = "");

		void loadProj(picojson::object& jo);
		void exportModel(picojson::object& jo);
		void setParam(picojson::object& jo);
		void saveParam(picojson::object& jo);
		void getParam(picojson::object& jo);
		void getParamSaved(picojson::object& jo);

	private:
		void updateCamIntrinsicMatrix(void);
		void updateFrame(const PointCloud& pcRaw, PointCloud* pPC, const FASTLIO_RGB_FRAME& f);
		void update(void);
		static void *getUpdate(void *This)
		{
			((_fastLioRGB *)This)->update();
			return NULL;
		}

		void handleMsg(const string &str);
		void updateR(void);
		static void *getUpdateR(void *This)
		{
			((_fastLioRGB *)This)->updateR();
			return NULL;
		}

	protected:
		string m_projDir;
		string m_fModelIn;
		string m_fModelOut;
		string m_fCamConfig;
		string m_fCamConfigDefault;
		string m_fCamTraj;
		float m_vdSize;

		PointCloud m_pcModelIn;
		PointCloud m_pcModelOut;
		_PCframe* m_pPC;
		FASTLIO_RGB_PC* m_pPCrgb;

		FASTLIO_RGB_CAM_INTRINSIC m_camIntrinsic;
		Matrix4d m_mCi;	// cam intrinsic
		Matrix4d m_mCS;	// cam to slam

		vector<FASTLIO_RGB_FRAME> m_vFrame;

	};

}
#endif
