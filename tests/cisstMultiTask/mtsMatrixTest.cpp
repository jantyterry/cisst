/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsMatrixTest.cpp 456 2009-06-13 03:11:44Z adeguet1 $
  
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

#include "mtsMatrixTest.h"
#include <cisstVector/vctRandomDynamicMatrix.h>

template <class _elementType>
void mtsMatrixTest::TestSetSizeFrom(void)
{
    typedef _elementType value_type;
    typedef mtsMatrix<value_type> MatrixType;
    typedef typename MatrixType::nsize_type nsize_type;
    const nsize_type size(10, 12);
    MatrixType original(size);
    vctRandom(original, static_cast<value_type>(0), static_cast<value_type>(10));
    CPPUNIT_ASSERT(original.sizes() == size);
    MatrixType copyByConstructor(original);
    CPPUNIT_ASSERT(copyByConstructor.Equal(original));
    MatrixType copy;
    CPPUNIT_ASSERT(copy.sizes() == nsize_type(0));
    new(&copy) MatrixType(original);
    CPPUNIT_ASSERT(copy.Equal(original));
}

void mtsMatrixTest::TestSetSizeFromDouble(void)
{
    TestSetSizeFrom<double>();
}

void mtsMatrixTest::TestSetSizeFromInt(void)
{
    TestSetSizeFrom<int>();
}
