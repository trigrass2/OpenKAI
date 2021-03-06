/*
 * Module.cpp
 *
 *  Created on: Nov 22, 2016
 *      Author: root
 */

#include "Module.h"

namespace kai
{

BASE* Module::createInstance(Kiss* pK)
{
	CHECK_N(!pK);

	ADD_MODULE(_Camera);
	ADD_MODULE(_Automaton);
	ADD_MODULE(_Mavlink);
	ADD_MODULE(_Canbus);
	ADD_MODULE(_RC);
	ADD_MODULE(_AutoPilot);
	ADD_MODULE(_AprilTags);
	ADD_MODULE(_Bullseye);
	ADD_MODULE(_Flow);
	ADD_MODULE(_Lightware_SF40);
	ADD_MODULE(_server);
	ADD_MODULE(_socket);
	ADD_MODULE(Window);
	ADD_MODULE(_ImageNet);
	ADD_MODULE(_Obstacle);
	ADD_MODULE(_DetectNet);

#ifndef USE_OPENCV4TEGRA
	ADD_MODULE(_ROITracker);
#endif
#ifdef USE_ZED
	ADD_MODULE(_ZED);
#endif
#ifdef USE_SSD
	ADD_MODULE(_SSD);
#endif
#ifdef USE_FCN
	ADD_MODULE(_FCN);
#endif

    return NULL;
}

template <typename T> BASE* Module::createInst(Kiss* pKiss)
{
	CHECK_N(!pKiss);

	T* pInst = new T();
	if(!pInst->init(pKiss))
	{
		delete pInst;
		return NULL;
	}
    return pInst;
}


Module::Module()
{
}

Module::~Module()
{
}

} /* namespace kai */
