#include "SGR.h"
#include "iostream"

int main()
{
	SGR sgr_object1;
	
	sgrErrCode resultSGRInit = sgr_object1.init();
	if (resultSGRInit != sgrOK) {
		return resultSGRInit;
	}

	while (sgr_object1.isSGRRunning()) {
		sgr_object1.drawFrame();
	}

	sgrErrCode resultSGRDestroy = sgr_object1.destroy();
	return resultSGRDestroy;
}