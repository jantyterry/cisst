/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet, Ali Uneri
  Created on: 2009-10-26

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _clientTask_h
#define _clientTask_h

#include <cisstMultiTask.h>

#include "ui_client.h"
#include <QObject>
#include <QMainWindow>


class clientTask : public QObject, public mtsDevice
{
    Q_OBJECT;
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    clientTask(const std::string & taskName);
    ~clientTask(void) {};

    void Configure(const std::string & CMN_UNUSED(filename)) {};

protected:
    void EventVoidHandler(void);
    void EventWriteHandler(const mtsInt & value);

    mtsFunctionVoid Void;
    mtsFunctionWrite Write;
    mtsFunctionRead Read;
    mtsFunctionQualifiedRead QualifiedRead;

    int EventCounter;

    QMainWindow MainWindow;
    Ui::ClientWindow ClientWindow;

signals:
    void ReadQtSignal(int newValue);
    void QualifiedReadQtSignal(int newValue);
    void EventQtSignal(int newValue);

public slots:
    void VoidQtSlot(void);
    void WriteQtSlot(int newValue);
    void ReadQtSlot(void);
    void QualifiedReadQtSlot(int newValue);
};

CMN_DECLARE_SERVICES_INSTANTIATION(clientTask);

#endif  // _clientTask_h
