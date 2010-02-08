/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id: sineTask.cpp 75 2009-02-24 16:47:20Z adeguet1 $ */

#include <cisstCommon/cmnConstants.h>
#include "sineTask.h"

double sineTask::SineAmplitude;

// required to implement the class services, see cisstCommon
CMN_IMPLEMENT_SERVICES(sineTask);

sineTask::sineTask(const std::string & taskName, double period):
    // base constructor, same task name and period.  Set the length of
    // state table to 5000
    mtsTaskPeriodic(taskName, period, false, 5000)
{
    // add SineData to the StateTable defined in mtsTask
    StateTable.AddData(SineData, "SineData");
    // add one interface, this will create an mtsTaskInterface
    mtsProvidedInterface * prov = AddProvidedInterface("MainInterface");
    if (prov) {
        // add command to access state table values to the interface
        prov->AddCommandReadState(StateTable, SineData, "GetData");
        // add command to modify the sine amplitude 
        prov->AddCommandWrite(&sineTask::SetAmplitude, this, "SetAmplitude");
        // add command to read data from state table while modifying the sine amplitude
        prov->AddCommandQualifiedRead(&sineTask::SetAndGetAmplitude, this,
            "SetAndGetAmplitude");
        // add command to generate a void event
        prov->AddCommandVoid(&sineTask::SendButtonClickEvent, this, "ButtonClicked");
        // define a void event
        EventVoid.Bind(prov->AddEventVoid("ButtonEventHandler"));
        // define a write event
        mtsDouble eventWriteData;
        EventWrite.Bind(prov->AddEventWrite("WriteEvent", eventWriteData));
    }
}

void sineTask::Startup(void) {
    sineTask::SineAmplitude = 1.0; // set the initial amplitude
}

void sineTask::Run(void) {
    // process the commands received, i.e. possible SetSineAmplitude
    ProcessQueuedCommands();
    // compute the new values based on the current time and amplitude
    SineData = sineTask::SineAmplitude
        * sin(2 * cmnPI * static_cast<double>(this->GetTick()) * Period / 10.0);
    SineData.SetTimestamp(StateTable.GetTic());
    SineData.SetValid(true);
    // send a write event
    static unsigned int count = 0;
    if (++count > 500) {
        EventWrite(SineData);
        count = 0;
    }
}

void sineTask::SetAndGetAmplitude(const mtsDouble &amplitude, mtsDouble & previousAmplitude) const
{
    previousAmplitude = SineAmplitude;

    sineTask::SineAmplitude = amplitude.Data;
}

/*
  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet, Min Yang Jung
  Created on: 2004-04-30

  (C) Copyright 2004-2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/
