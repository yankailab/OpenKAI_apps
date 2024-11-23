#include <OpenKAI/OpenKAI.h>
#include "_AP_droneBox.h"
#include "_AP_droneBoxJSON.h"

using namespace std;
using namespace kai;

OpenKAI *g_pOK = NULL;

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		printf("Usage: ./DroneBoxAir [kiss file]\n");
		return 0;
	}

	string argStr(argv[1]);
	if (argStr == "-h" || argStr == "--help")
	{
		printf("Usage: ./DroneBoxAir [kiss file]\n");
		return 0;
	}

	printf("Kiss file: %s\n", argStr.c_str());

	g_pOK = new OpenKAI();

	if (g_pOK->init() != OK_OK)
		return 1;

	if (g_pOK->addKiss(argStr) != OK_OK)
		return 1;

	// create OpenKAI core modules from Kiss config
	if (g_pOK->createAllModules() != OK_OK)
		return 1;

	// add user app modules
	_AP_droneBox *pAPdroneBox = new _AP_droneBox();
	if (g_pOK->addModule(pAPdroneBox, "apDroneBox") != OK_OK)
		return 1;

	_AP_droneBoxJSON *pAPdroneBoxJson = new _AP_droneBoxJSON();
	if (g_pOK->addModule(pAPdroneBoxJson, "apDroneBoxJSON") != OK_OK)
		return 1;

	// init all modules altogether
	if (g_pOK->initAllModules() != OK_OK)
		return 1;

	if (g_pOK->linkAllModules() != OK_OK)
		return 1;

	// start all modules
	if (g_pOK->startAllModules() != OK_OK)
		return 1;

	g_pOK->waitForComplete();

	return 0;
}
