/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlFilterImageOpenGL.cpp -1   $

  Author(s):  Marcin Balicki
  Created on: 2011-02-18

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstStereoVision/svlFilterImageOpenGL.h>

#if (CISST_OS == CISST_WINDOWS)
  #include <windows.h>
#endif

#if (CISST_OS == CISST_DARWIN)
  #include <OpenGL/gl.h>
#else
  #include <GL/gl.h>
#endif


CMN_IMPLEMENT_SERVICES(svlFilterImageOpenGL);


svlFilterImageOpenGL::svlFilterImageOpenGL(void):
    svlFilterBase(),
    Image(0)
{
    AddInput("input", true);
    AddInputType("input", svlTypeImageRGB);
    AddInputType("input", svlTypeImageRGBStereo);

    AddOutput("output", true);
    SetAutomaticOutputType(true);
}


svlFilterImageOpenGL::~svlFilterImageOpenGL()
{
    Release();
}


void svlFilterImageOpenGL::RenderInitialize(void)
{
    glEnable(GL_NORMALIZE);
    CheckGLError(__FUNCTION__);
}


void svlFilterImageOpenGL::RenderResize(double width, double height)
{
    this->Width = static_cast<int>(width);
    this->Height = static_cast<int>(height);
    glViewport(0, 0, this->Width, this->Height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, Width, 0.0, Height, 0.0, 1000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    CheckGLError(__FUNCTION__);
}


void svlFilterImageOpenGL::Render(void)
{
    std::cerr << "render" << std::endl;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.6f, 0.0f, 0.0f); // provide a green background by default

    if (!Image) {
        return;
    }

    int iw = Image->GetWidth();
    int ih = Image->GetHeight();

    glEnable (GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 8);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, iw, ih, 0, GL_RGB, GL_UNSIGNED_BYTE, Image->GetUCharPointer());

    glPushMatrix();
    glRotatef(-90.0, 0.0, 0.0, 1.0);
    glTranslatef(-this->Height, 0.0, 0.0);

    glBegin (GL_QUADS);
    {
        glTexCoord2f(0.0, 0.0);
        glVertex3f(0.0, 0.0, 0.0);

        glTexCoord2f(0.0, 1.0);
        glVertex3f(this->Height, 0.0, 0.0);

        glTexCoord2f(1.0, 1.0);
        glVertex3f(this->Height, this->Width, 0.0);

        glTexCoord2f(1.0, 0.0);
        glVertex3f(0.0, this->Width, 0.0);
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    CheckGLError(__FUNCTION__);
}


int svlFilterImageOpenGL::Initialize(svlSample * syncInput, svlSample* &syncOutput)
{
    Release();

    Image = dynamic_cast<svlSampleImage*>(svlSampleImage::GetNewFromType(syncInput->GetType()));
    svlSampleImage * imagePointer = dynamic_cast<svlSampleImage*>(syncInput);

    for (size_t i = 0; i < Image->GetVideoChannels(); i++) {
        Image->SetSize(i, imagePointer->GetWidth(), imagePointer->GetHeight());
    }

    syncOutput = syncInput;

    return SVL_OK;
}


int svlFilterImageOpenGL::Process(svlProcInfo * procInfo, svlSample * syncInput, svlSample* &syncOutput)
{
    syncOutput = syncInput;
    _SkipIfAlreadyProcessed(syncInput, syncOutput);
    _SkipIfDisabled();

    svlSampleImage * imagePointer = dynamic_cast<svlSampleImage*>(syncInput);
    const size_t videochannels = imagePointer->GetVideoChannels();

    _OnSingleThread(procInfo)
    {
        for (size_t i = 0; i < videochannels; i++) {
            memcpy(Image->GetUCharPointer(i), imagePointer->GetUCharPointer(i), Image->GetDataSize(i));
        }
        std::cerr << "end of svl process" << std::endl;
        PostProcess();
    }

    return SVL_OK;
}


int svlFilterImageOpenGL::Release(void)
{
    return SVL_OK;
}


void svlFilterImageOpenGL::CheckGLError(const std::string & functionName)
{
    for (GLenum gl_error = glGetError();
         (gl_error);
         gl_error = glGetError()) {
        switch (gl_error) {
        case (GL_INVALID_ENUM):
            CMN_LOG_CLASS_INIT_ERROR << "CheckGLError: invalid OpenGL ENUM: "
                                     << gl_error << " for function " << functionName << std::endl;
            break;
        case (GL_INVALID_VALUE):
            CMN_LOG_CLASS_INIT_ERROR << "CheckGLError: invalid OpenGL VALUE: "
                                     << gl_error << " for function " << functionName << std::endl;
            break;
        case (GL_INVALID_OPERATION):
            CMN_LOG_CLASS_INIT_ERROR << "CheckGLError: invalid OpenGL OPERATION: "
                                     << gl_error << " for function " << functionName << std::endl;
            printf("%s: %u caught: Invalid OPERATION\n", __FUNCTION__,gl_error);
            break;
        case (GL_STACK_OVERFLOW):
            CMN_LOG_CLASS_INIT_ERROR << "CheckGLError: OpenGL GL_STACK_OVERFLOW: "
                                     << gl_error << " for function " << functionName << std::endl;
            break;
        case (GL_STACK_UNDERFLOW):
            CMN_LOG_CLASS_INIT_ERROR << "CheckGLError: OpenGL GL_STACK_UNDERFLOW: "
                                     << gl_error << " for function " << functionName << std::endl;
            break;
        case (GL_OUT_OF_MEMORY):
            CMN_LOG_CLASS_INIT_ERROR << "CheckGLError: OpenGL GL_OUT_OF_MEMORY: "
                                     << gl_error << " for function " << functionName << std::endl;
            break;
        default:
            CMN_LOG_CLASS_INIT_ERROR << "CheckGLError: invalid OpenGL something: "
                                     << gl_error << " for function " << functionName << std::endl;
        }
    }
}