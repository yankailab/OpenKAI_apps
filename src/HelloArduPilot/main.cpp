#include <OpenKAI/OpenKAI.h>
#include <OpenKAI/Autopilot/ArduPilot/_AP_base.h>
#include <OpenKAI/Autopilot/ArduPilot/_AP_move.h>

using namespace std;
using namespace kai;

#define OK_MODULE_MAV "apMavlink"
#define OK_MODULE_CONSOLE "console"
#define OK_MODULE_AP "apBase"
#define OK_MODULE_AP_MOVE "apMove"

OpenKAI *g_pOK = NULL;
_Mavlink *g_pMav = NULL;
_AP_base *g_pAP = NULL;
_AP_move *g_pAPmove = NULL;
_Console *g_pConsole = NULL;

int main(int argc, char* argv[])
{
	if(argc < 2)
	{
		printf("Usage: ./HelloArduPilot [kiss file]\n");
		return 0;
	}

	string argStr(argv[1]);
	if(argStr == "-h" || argStr == "--help")
	{
		printf("Usage: ./HelloArduPilot [kiss file]\n");
		return 0;
	}

	printf("Kiss file: %s\n", argStr.c_str());

    g_pOK = new OpenKAI();

    if (!g_pOK->init())
		return 1;

    if (!g_pOK->addKiss(argStr))
		return 1;

    // create OpenKAI core modules from Kiss config
    if (!g_pOK->createAllModules())
		return 1;

    // acquire pointers to OpenKAI core modules
    g_pConsole = (_Console *)g_pOK->findModule(OK_MODULE_CONSOLE);
    if (!g_pConsole)
		return 1;

    g_pMav = (_Mavlink *)g_pOK->findModule(OK_MODULE_MAV);
    if (!g_pMav)
		return 1;

    g_pAP = (_AP_base *)g_pOK->findModule(OK_MODULE_AP);
    if (!g_pAP)
		return 1;

    g_pAPmove = (_AP_move *)g_pOK->findModule(OK_MODULE_AP_MOVE);
    if (!g_pAPmove)
		return 1;

    // init all modules altogether
    if (!g_pOK->initAllModules())
		return 1;

    if (!g_pOK->linkAllModules())
		return 1;

	// start all modules
    if(!g_pOK->startAllModules())
		return 1;


	g_pAP->setMode(AP_COPTER_GUIDED);

	g_pAP->setArm(true);

	g_pMav->clNavTakeoff(10.0);

	vDouble4 gpsPos;
	gpsPos.x = 0; // lon
	gpsPos.y = 0; // lat
	gpsPos.z = 10; // alt
	g_pAPmove->setPglobal(gpsPos);

	sleep(5);

	g_pAP->setMode(AP_COPTER_LAND);


	while(1)
	{
		sleep(1);
	}
	
	return 0;
}
