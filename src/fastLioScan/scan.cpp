#include <OpenKAI/OpenKAI.h>
#include "_fastLioScan.h"
#include "_fastLioRGB.h"

using namespace std;
using namespace kai;

OpenKAI *g_pOK = NULL;

int main(int argc, char* argv[])
{
	if(argc < 2)
	{
		printf("Usage: ./fastLioScan [kiss file]\n");
		return 0;
	}

	string argStr(argv[1]);
	if(argStr == "-h" || argStr == "--help")
	{
		printf("Usage: ./fastLioScan [kiss file]\n");
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

	// add user app modules
	_fastLioScan* pFLscan = new _fastLioScan();
	if(!g_pOK->addModule(pFLscan, "fastLioScan"))
		return 1;

    // init all modules altogether
    if (!g_pOK->initAllModules())
		return 1;

    if (!g_pOK->linkAllModules())
		return 1;

	// start all modules
    if(!g_pOK->startAllModules())
		return 1;

	while(1)
	{
		sleep(1);
	}
	
	return 0;
}
