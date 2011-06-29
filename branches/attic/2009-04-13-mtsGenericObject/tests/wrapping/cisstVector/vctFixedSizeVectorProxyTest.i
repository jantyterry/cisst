/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctFixedSizeVectorProxyTest.i,v 1.6 2007/04/26 20:12:05 anton Exp $

  Author(s):  Anton Deguet
  Created on: 2005-06-23

  (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


%module vctFixedSizeVectorProxyTestPython

%header %{
    #include "cisstVector/cisstVector.i.h"
    #include "vctFixedSizeVectorProxyTest.h"
%}


%include "std_string.i"

%import "cisstCommon/cisstCommon.i"
%import "cisstVector/cisstVector.i"

%include "vctFixedSizeVectorProxyTest.h"
