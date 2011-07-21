/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: ui3VideoInterfaceFilter.cpp 1218 2010-02-22 16:08:09Z adeguet1 $

  Author(s):	Daniel Mirota
  Created on:	2008-06-10

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstStereoVision/svlFilterCallback.h>


svlFilterCallback::svlFilterCallback() :
    svlFilterBase(),
	myCallback(0),
        myCallbackData(0),
        OutputData(0)
{

    AddInput("input", true);
    AddInputType("input", svlTypeImageRGB);
    AddInputType("input", svlTypeImageMono8);
    AddInputType("input", svlTypeImageRGBStereo);
    AddInputType("input", svlTypeImageMono8Stereo);
}

svlFilterCallback::~svlFilterCallback()
{
}

int svlFilterCallback::Initialize(svlSample* syncInput, svlSample* &syncOutput)
{
    OutputData = syncInput;

    syncOutput = OutputData;

    return SVL_OK;
}

int svlFilterCallback::Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput)
{
	//The callback call should be single threaded just in case
        svlSampleImageRGB* img = dynamic_cast<svlSampleImageRGB*>(syncInput);
        vctDynamicMatrixRef<unsigned char> imageData = img->GetMatrixRef();

        syncOutput = OutputData;

    _OnSingleThread(procInfo) {
                if(myCallback && IsRunning()){
			if((*myCallback)(&imageData,myCallbackData) != SVL_OK){
				return SVL_FAIL;
			}
		}	
    }
    return SVL_OK;
}

unsigned int svlFilterCallback::GetWidth(unsigned int videoch)
{
    if (!OutputData) return 0;
    return dynamic_cast<svlSampleImage*>(OutputData)->GetWidth(videoch);
}

unsigned int svlFilterCallback::GetHeight(unsigned int videoch)
{
    if (!OutputData) return 0;
    return dynamic_cast<svlSampleImage*>(OutputData)->GetHeight(videoch);
}

int svlFilterCallback::SetCallback(CALLBACK_FUNC callback_in,void * callbackData_in)
{
	if(callback_in && callbackData_in){
		myCallback = callback_in;
		myCallbackData = callbackData_in;
		return SVL_OK;
	}else{
		return SVL_FAIL;
	}
}
