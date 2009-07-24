/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: prmPositionCartesianGetTest.cpp 456 2009-06-13 03:11:44Z adeguet1 $
  
  Author(s):  Anton Deguet
  Created on: 2009-04-29
  
  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "prmPositionCartesianGetTest.h"
#include "prmSetAndTestGenericObjectSerialization.h"

#include <cisstVector/vctRandom.h>
#include <cisstParameterTypes/prmPositionCartesianGet.h>


void prmPositionCartesianGetTest::TestSerialize(void)
{
    prmPositionCartesianGet initial, final;
    // test part inherited from mtsGenericObject
    prmSetAndTestGenericObjectSerialization(initial);

    vctRandom(initial.Position().Translation(), -10.0, 10.0);
    vctRandom(initial.Position().Rotation());
    std::stringstream serializationStream;
    initial.SerializeRaw(serializationStream);
    final.DeSerializeRaw(serializationStream);

    CPPUNIT_ASSERT(final.Position().Equal(initial.Position()));
}
