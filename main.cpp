#include "SGR.h"
#include "iostream"

int main()
{
	SGR sgr_object1;
	
	sgrErrCode resultSGRInit = sgr_object1.init();
	if (resultSGRInit != sgrOK) {
		return resultSGRInit;
	}

	sgr_object1.addToFrameSimpleTestObject();

	while (sgr_object1.isSGRRunning()) {
		sgrErrCode resultDrawFrame = sgr_object1.drawFrame();
		if (resultDrawFrame != sgrOK)
			return resultDrawFrame;
	}

	sgrErrCode resultSGRDestroy = sgr_object1.destroy();
	return resultSGRDestroy;
}