/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Praneeth Sadda, Anton Deguet
  Created on: 2011-11-11

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstMultiTask/mtsQtWidgetInterfaceRequired.h>
#include <cisstMultiTask/mtsQtWidgetFunction.h>
#include <cisstMultiTask/mtsQtWidgetEvent.h>

#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstMultiTask/mtsFunctionVoid.h>
#include <cisstMultiTask/mtsFunctionVoidReturn.h>
#include <cisstMultiTask/mtsFunctionWrite.h>
#include <cisstMultiTask/mtsFunctionWriteReturn.h>
#include <cisstMultiTask/mtsFunctionRead.h>
#include <cisstMultiTask/mtsFunctionQualifiedRead.h>

#include <QVBoxLayout>
#include <QToolBox>


mtsQtWidgetInterfaceRequired::mtsQtWidgetInterfaceRequired(mtsInterfaceProvided * interface,
                                                           mtsInterfaceRequired * executionInterface):
    QWidget(),
    ExecutionInterface(executionInterface)
{
    FunctionsWidget = new QToolBox();
    QLayout* layout = new QVBoxLayout();
    layout->addWidget(FunctionsWidget);
    setLayout(layout);
    CreateWidgets(*interface, *ExecutionInterface);
}


void mtsQtWidgetInterfaceRequired::CreateWidgets(mtsInterfaceProvided & interface, mtsInterfaceRequired & executionInterface)
{
    typedef std::vector<std::string> NamesVector;
    NamesVector names;
    NamesVector::const_iterator name;
    NamesVector::const_iterator end;
    std::string stdName;

    // keep track of functions added to retrieve argument prototype after connection
    mtsQtWidgetFunction * functionWidget;
    typedef std::list<mtsQtWidgetFunction *> FunctionWidgetsList;
    FunctionWidgetsList functionWidgets;

    // keep track of events added to retrieve argument prototype after connection
    typedef std::list<mtsQtWidgetEventWrite *> EventWidgetsList;
    EventWidgetsList eventWidgets;

    // void functions
    mtsFunctionVoid * functionVoid;
    names = interface.GetNamesOfCommandsVoid();
    end = names.end();
    for (name = names.begin(); name != end; name++) {
        stdName= *name;
        functionVoid = new mtsFunctionVoid;
        executionInterface.AddFunction(stdName, *functionVoid, MTS_REQUIRED);
        functionWidget = new mtsQtWidgetFunctionVoid(functionVoid);
        FunctionsWidget->addItem(functionWidget, stdName.c_str());
        functionWidgets.push_back(functionWidget);
    }

    // void return functions
    mtsFunctionVoidReturn * functionVoidReturn;
    names = interface.GetNamesOfCommandsVoidReturn();
    end = names.end();
    for (name = names.begin(); name != end; name++) {
        stdName= *name;
        functionVoidReturn = new mtsFunctionVoidReturn;
        executionInterface.AddFunction(stdName, *functionVoidReturn, MTS_REQUIRED);
        functionWidget = new mtsQtWidgetFunctionVoidReturn(functionVoidReturn);
        FunctionsWidget->addItem(functionWidget, stdName.c_str());
        functionWidgets.push_back(functionWidget);
    }

    // write functions
    mtsFunctionWrite * functionWrite;
    names = interface.GetNamesOfCommandsWrite();
    end = names.end();
    for (name = names.begin(); name != end; name++) {
        stdName= *name;
        functionWrite = new mtsFunctionWrite;
        executionInterface.AddFunction(stdName, *functionWrite, MTS_REQUIRED);
        functionWidget = new mtsQtWidgetFunctionWrite(functionWrite);
        FunctionsWidget->addItem(functionWidget, stdName.c_str());
        functionWidgets.push_back(functionWidget);
    }

    // write return functions
    mtsFunctionWriteReturn * functionWriteReturn;
    names = interface.GetNamesOfCommandsWriteReturn();
    end = names.end();
    for (name = names.begin(); name != end; name++) {
        stdName= *name;
        functionWriteReturn = new mtsFunctionWriteReturn;
        executionInterface.AddFunction(stdName, *functionWriteReturn, MTS_REQUIRED);
        functionWidget = new mtsQtWidgetFunctionWriteReturn(functionWriteReturn);
        FunctionsWidget->addItem(functionWidget, stdName.c_str());
        functionWidgets.push_back(functionWidget);
    }

    // read functions
    mtsFunctionRead * functionRead;
    names = interface.GetNamesOfCommandsRead();
    end = names.end();
    for (name = names.begin(); name != end; name++) {
        stdName= *name;
        functionRead = new mtsFunctionRead;
        executionInterface.AddFunction(stdName, *functionRead, MTS_REQUIRED);
        functionWidget = new mtsQtWidgetFunctionRead(functionRead);
        FunctionsWidget->addItem(functionWidget, stdName.c_str());
        functionWidgets.push_back(functionWidget);
    }

    // qualified read functions
    mtsFunctionQualifiedRead * functionQualifiedRead;
    names = interface.GetNamesOfCommandsQualifiedRead();
    end = names.end();
    for (name = names.begin(); name != end; name++) {
        stdName= *name;
        functionQualifiedRead = new mtsFunctionQualifiedRead;
        executionInterface.AddFunction(stdName, *functionQualifiedRead, MTS_REQUIRED);
        functionWidget = new mtsQtWidgetFunctionQualifiedRead(functionQualifiedRead);
        FunctionsWidget->addItem(functionWidget, stdName.c_str());
        functionWidgets.push_back(functionWidget);
    }

    // void events
    mtsQtWidgetEventVoid * eventVoidWidget;
    names = interface.GetNamesOfEventsVoid();
    end = names.end();
    for (name = names.begin(); name != end; name++) {
        stdName= *name;
        eventVoidWidget = new mtsQtWidgetEventVoid;
        executionInterface.AddEventHandlerVoid(&mtsQtWidgetEventVoid::EventHandler, eventVoidWidget, stdName);
        FunctionsWidget->addItem(eventVoidWidget, stdName.c_str());
    }

    // write events
    mtsCommandWriteBase * eventCommand;
    mtsQtWidgetEventWrite * eventWriteWidget;
    names = interface.GetNamesOfEventsWrite();
    end = names.end();
    for (name = names.begin(); name != end; name++) {
        stdName= *name;
        eventWriteWidget = new mtsQtWidgetEventWrite;
        eventCommand = executionInterface.AddEventHandlerWriteGeneric(&mtsQtWidgetEventWrite::EventHandler, eventWriteWidget, stdName);
        eventWriteWidget->SetCommand(eventCommand);
        FunctionsWidget->addItem(eventWriteWidget, stdName.c_str());
        eventWidgets.push_back(eventWriteWidget);
    }

    // connect all
    executionInterface.ConnectTo(&interface);

    // refresh all widgets based on argument prototypes
    FunctionWidgetsList::iterator widgetFunctionIterator = functionWidgets.begin();
    FunctionWidgetsList::iterator widgetFunctionIteratorEnd = functionWidgets.end();
    for (; widgetFunctionIterator != widgetFunctionIteratorEnd; ++widgetFunctionIterator) {
        (*widgetFunctionIterator)->CreateArgumentsWidgets();
    }

    EventWidgetsList::iterator widgetEventIterator = eventWidgets.begin();
    EventWidgetsList::iterator widgetEventIteratorEnd = eventWidgets.end();
    for (; widgetEventIterator != widgetEventIteratorEnd; ++widgetEventIterator) {
        (*widgetEventIterator)->CreateArgumentsWidgets();
    }

}
