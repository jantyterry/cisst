/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2010-05-05

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstVector/vctPlot2DOpenGLQtWidget.h>

#include <QMouseEvent>
#include <QMenu>

vctPlot2DOpenGLQtWidget::vctPlot2DOpenGLQtWidget(QWidget * parent):
    QGLWidget(parent),
    vctPlot2DOpenGL()
{
}


void vctPlot2DOpenGLQtWidget::initializeGL(void)
{
    vctPlot2DOpenGL::RenderInitialize();
}


void vctPlot2DOpenGLQtWidget::resizeGL(int width, int height)
{
    vctPlot2DOpenGL::RenderResize(width, height);
}


void vctPlot2DOpenGLQtWidget::paintGL(void)
{
    vctPlot2DOpenGL::Render();
}


void vctPlot2DOpenGLQtWidget::mouseReleaseEvent(QMouseEvent * event)
{
    if (event->button() == Qt::RightButton) {
        // local QMenu will be deleted
        QMenu menu;

        QAction * freeze = new QAction("Freeze", this);
        freeze->setCheckable(true);
        freeze->setChecked(this->GetFreeze());
        menu.addAction(freeze);
        connect(freeze, SIGNAL(toggled(bool)), this, SLOT(FreezeSlot(bool)));

        menu.addSeparator();

        QAction * fitXnow = new QAction("Fit X now", this);
        QAction * fitXalways = new QAction("Fit X always", this);
        fitXalways->setCheckable(true);
        fitXalways->setChecked(this->GetContinuousFitX());
        menu.addAction(fitXnow);
        menu.addAction(fitXalways);
        connect(fitXnow, SIGNAL(triggered()), this, SLOT(FitXSlot()));
        connect(fitXalways, SIGNAL(toggled(bool)), this, SLOT(SetContinuousFitXSlot(bool)));

        menu.addSeparator();

        QAction * fitYnow = new QAction("Fit Y now", this);
        QAction * fitYalways = new QAction("Fit Y always", this);
        fitYalways->setCheckable(true);
        fitYalways->setChecked(this->GetContinuousFitY());
        menu.addAction(fitYnow);
        menu.addAction(fitYalways);
        connect(fitYnow, SIGNAL(triggered()), this, SLOT(FitYSlot()));
        connect(fitYalways, SIGNAL(toggled(bool)), this, SLOT(SetContinuousFitYSlot(bool)));

        menu.exec(mapToGlobal(event->pos()));
    }
}


void vctPlot2DOpenGLQtWidget::FreezeSlot(bool checked)
{
    this->Freeze(checked);
}


void vctPlot2DOpenGLQtWidget::FitXSlot(void)
{
    this->SetContinuousFitX(false);
    this->FitX();
}


void vctPlot2DOpenGLQtWidget::FitYSlot(void)
{
    this->SetContinuousFitY(false);
    this->FitY();
}


void vctPlot2DOpenGLQtWidget::SetContinuousFitXSlot(bool checked)
{
    this->SetContinuousFitX(checked);
}


void vctPlot2DOpenGLQtWidget::SetContinuousFitYSlot(bool checked)
{
    this->SetContinuousFitY(checked);
}

