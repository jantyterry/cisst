/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s): Gorkem Sevinc, Anton Deguet
  Created on: 2009-09-04

  (C) Copyright 2009-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

// Sensable headers
#include <cisstDevices/devNovintHDLMasterSlave.h>
#include <cisstVector/vctTypes.h>

CMN_IMPLEMENT_SERVICES(devNovintHDLMasterSlave);

devNovintHDLMasterSlave::devNovintHDLMasterSlave(const std::string & taskName,
                                                 const std::string & firstDeviceName,
                                                 const std::string & secondDeviceName):
    devNovintHDL(taskName, firstDeviceName, secondDeviceName)
{
    // Initialize the struct and class vectors
    RobotPair.resize(1);
    DevicePair.resize(1);
    DevicePair[0] = new DevData;
    RobotPair[0] = new robCollaborativeControlForce(0.5, 
                                                    0.5, 
                                                    20.0, 
                                                    robCollaborativeControlForce::ParameterType::RATCHETED,
                                                    1.0,
                                                    1.0);
    
    // Assign device numbers
    if(DevicesVector[0]->Name == firstDeviceName) {
        DevicePair[0]->MasterDeviceNo = DevicesVector(0)->DeviceNumber;
        DevicePair[0]->SlaveDeviceNo = DevicesVector(1)->DeviceNumber;
    } else {
        DevicePair[0]->MasterDeviceNo = DevicesVector(1)->DeviceNumber;
        DevicePair[0]->SlaveDeviceNo = DevicesVector(0)->DeviceNumber;
    }

    PairNumber = 0;
    PairCount = 1;
    // Initialize provided interfaces
    SetupTeleoperationInterfaces(firstDeviceName, secondDeviceName, PairNumber);
}


void devNovintHDLMasterSlave::SetupTeleoperationInterfaces(const std::string & firstDeviceName, 
                                                           const std::string & secondDeviceName,
                                                           int pair)
{
    
    // Create the provided interface
    mtsInterfaceProvided * providedInterface;
    std::string providedInterfaceName = "TeleoperationParameters" + firstDeviceName + secondDeviceName;
    providedInterface = AddInterfaceProvided(providedInterfaceName);

    // Initialize Values
    DevData * pairData;
    pairData = DevicePair[pair];
    pairData->Parameter.PositionLimit() = 40.0;
    pairData->Parameter.LinearGainMaster() = 0.3;
    pairData->Parameter.LinearGainSlave() = 0.3;
    pairData->Parameter.ForceFeedbackRatio() = 1.0;
    pairData->Parameter.ForceMode() = prmCollaborativeControlForce::RATCHETED;
    pairData->MasterClutchGUI = false;
    pairData->SlaveClutchGUI = false;
    pairData->MasterSlaveClutchGUI = false;

    // Add values to the state table
    StateTable.AddData(pairData->MasterClutchGUI, "MasterClutch");
    StateTable.AddData(pairData->SlaveClutchGUI, "SlaveClutch");
    StateTable.AddData(pairData->MasterSlaveClutchGUI, "MasterSlaveClutch");
    StateTable.AddData(pairData->Parameter, "CollaborativeControlParameter");


    // Add read functions to the interface
    providedInterface->AddCommandReadState(StateTable, pairData->MasterClutchGUI, "GetMasterClutch");
    providedInterface->AddCommandReadState(StateTable, pairData->SlaveClutchGUI, "GetSlaveClutch");
    providedInterface->AddCommandReadState(StateTable, pairData->MasterSlaveClutchGUI, "GetMasterSlaveClutch");
    providedInterface->AddCommandReadState(StateTable, pairData->Parameter, "GetCollaborativeControlParameter");

    // Add write functions to the interface
    providedInterface->AddCommandWrite(&devNovintHDLMasterSlave::SetMasterClutch, 
                                       this, "SetMasterClutch", pairData->MasterClutchGUI);
    providedInterface->AddCommandWrite(&devNovintHDLMasterSlave::SetSlaveClutch, 
                                       this, "SetSlaveClutch", pairData->SlaveClutchGUI);
    providedInterface->AddCommandWrite(&devNovintHDLMasterSlave::SetMasterSlaveClutch, 
                                       this, "SetMasterSlaveClutch", pairData->MasterSlaveClutchGUI);
    providedInterface->AddCommandWrite(&devNovintHDLMasterSlave::SetCollaborativeControlParameter, 
                                       this, "SetCollaborativeControlParameter", pairData->Parameter);

}


void devNovintHDLMasterSlave::UserControl(void)
{   
    DevData * pairData;
    int index = 0;
    for(index; index < PairCount; index++)
    {
        // Get the device pair data
        pairData = DevicePair[index];

        // Call robCollaborativeControlForce Update function, which carries out the 
        // teleoperation process with the given positions and clutches, then returns
        // two forces
        RobotPair[index]->Update(DevicesVector(pairData->MasterDeviceNo)->PositionCartesian.Position().Translation(), 
                                 DevicesVector(pairData->SlaveDeviceNo)->PositionCartesian.Position().Translation(), 
                                 DevicesVector(pairData->MasterDeviceNo)->Clutch, 
                                 DevicesVector(pairData->SlaveDeviceNo)->Clutch, 
                                 DevicesVector(pairData->MasterDeviceNo)->ForceCartesian.Force(), 
                                 DevicesVector(pairData->SlaveDeviceNo)->ForceCartesian.Force());

        // If the pair has the GUI attached
        if(index == PairNumber) {
            // Update the values through the GUI
            RobotPair[index]->SetParameter(pairData->Parameter);
            RobotPair[index]->SetApplicationMasterClutch(pairData->MasterClutchGUI);
            RobotPair[index]->SetApplicationSlaveClutch(pairData->SlaveClutchGUI);
            RobotPair[index]->SetApplicationMasterSlaveClutch(pairData->MasterSlaveClutchGUI);
        }
    }
}

void devNovintHDLMasterSlave::SetLinearGainMaster(const mtsDouble & Scale)
{
    DevicePair[PairNumber]->Parameter.LinearGainMaster() = Scale.Data;
}

void devNovintHDLMasterSlave::SetLinearGainSlave(const mtsDouble & Scale)
{
    DevicePair[PairNumber]->Parameter.LinearGainSlave() = Scale.Data;
}

void devNovintHDLMasterSlave::SetForceLimit(const mtsDouble& FLimit)
{
    DevicePair[PairNumber]->Parameter.PositionLimit() = FLimit.Data;
}

void devNovintHDLMasterSlave::SetForceMode(const mtsInt& Mode)
{
    if(Mode.Data == 0) {
        DevicePair[PairNumber]->Parameter.ForceMode() = prmCollaborativeControlForce::RATCHETED;
    } else if (Mode.Data == 1) {
        DevicePair[PairNumber]->Parameter.ForceMode() = prmCollaborativeControlForce::CAPPED;
    } else if (Mode.Data == 2) {
        DevicePair[PairNumber]->Parameter.ForceMode() = prmCollaborativeControlForce::RAW;
    }
}

void devNovintHDLMasterSlave::SetForceCoefficient(const mtsDouble& commandedCoefficient)
{
    DevicePair[PairNumber]->Parameter.ForceFeedbackRatio() = commandedCoefficient.Data;
}

void devNovintHDLMasterSlave::SetMasterClutch(const mtsBool& commandedClutch)
{
    DevicePair[PairNumber]->MasterClutchGUI = commandedClutch;
}

void devNovintHDLMasterSlave::SetSlaveClutch(const mtsBool& commandedClutch)
{
    DevicePair[PairNumber]->SlaveClutchGUI = commandedClutch;
}

void devNovintHDLMasterSlave::SetMasterSlaveClutch(const mtsBool& commandedClutch)
{
    DevicePair[PairNumber]->MasterSlaveClutchGUI = commandedClutch;
}

void devNovintHDLMasterSlave::SetCollaborativeControlParameter(const prmCollaborativeControlForce & parameter)
{
    DevicePair[PairNumber]->Parameter = parameter;
}
