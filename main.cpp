#include "SGR.h"
#include "iostream"

int main()
{
	SGR sgr_object1;
	
	SgrErrCode resultSGRInit = sgr_object1.init();
	if (resultSGRInit != sgrOK) {
		return resultSGRInit;
	}

	SgrErrCode resultInitDraw = sgr_object1.addToFrameSimpleTestObject();
	if (resultInitDraw != sgrOK)
		return resultInitDraw;

	while (sgr_object1.isSGRRunning()) {
		SgrErrCode resultDrawFrame = sgr_object1.drawFrame();
		if (resultDrawFrame != sgrOK)
			return resultDrawFrame;
	}

	SgrErrCode resultSGRDestroy = sgr_object1.destroy();
	return resultSGRDestroy;
}