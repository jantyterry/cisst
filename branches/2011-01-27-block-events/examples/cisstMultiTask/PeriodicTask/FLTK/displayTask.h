/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/*
  $Id$

  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet
  Created on: 2004-04-30

  (C) Copyright 2004-2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _displayTask_h
#define _displayTask_h

#include <cisstMultiTask.h>
#include "displayUI.h"

class displayTask: public mtsTaskPeriodic {
    // set log level to "Run Error"
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

 protected:

    MTS_DECLARE_COMPONENT(displayTask, mtsTaskPeriodic);

    MTS_INTERFACE_REQUIRED_BEGIN(DataGenerator);
    MTS_FUNCTION_READ(GetData, "GetData");
    MTS_FUNCTION_WRITE(SetAmplitude, "SetAmplitude");
    MTS_FUNCTION_WRITE(SetTriggerThreshold, "SetTriggerThreshold");
    MTS_FUNCTION_VOID(ResetTrigger, "ResetTrigger");
    MTS_EVENT_HANDLER_NONQUEUED_VOID(HandleTrigger, "TriggerEvent");
    MTS_INTERFACE_REQUIRED_END(DataGenerator);

    MTS_INTERFACE_REQUIRED_BEGIN(TimeGenerator);
    MTS_FUNCTION_READ(GetTime, "GetTime");
    MTS_INTERFACE_REQUIRED_END(TimeGenerator);

    // local copy of data used in commands
    mtsDouble Data;
    mtsDouble AmplitudeData;
    mtsDouble Time;

    // user interface generated by FTLK/fluid
    displayUI UI;

    // event callback for trigger event
    void HandleTrigger(void);

    // thread signal used to block the task
    osaThreadSignal ThreadSignal;

 public:
    // see sineTask.h documentation
    displayTask(const std::string & taskName, double period);
    ~displayTask() {};
    void Configure(const std::string & CMN_UNUSED(filename) = "");
    void Startup(void);
    void Run(void);
    void Cleanup(void) {};
    void UpdateUI(void);
};

CMN_DECLARE_SERVICES_INSTANTIATION(displayTask);

#endif // _displayTask_h
