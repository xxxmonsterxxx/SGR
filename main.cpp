#include "SGR.h"
#include "iostream"

int main()
{
	SGR sgr_object1;
	
	if (sgr_object1.init() != sgrOK) {
		throw std::runtime_error("failed to create SGR object");
	}

	while (sgr_object1.isSGRRunning()) {
		sgr_object1.drawFrame();
	}

	sgrErrCode resultSGRDestroy;
	resultSGRDestroy = sgr_object1.destroy();
	return resultSGRDestroy;
}