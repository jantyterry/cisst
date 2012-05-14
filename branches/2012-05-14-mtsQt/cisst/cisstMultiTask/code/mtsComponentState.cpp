/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2010-09-14

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

  --- begin cisst license - do not edit ---

  This software is provided "as is" under an open source license, with
  no warranty.  The complete license can be found in license.txt and
  http://www.cisst.org/cisst/license.txt.

  --- end cisst license ---
*/

#include <cisstMultiTask/mtsComponentState.h>
#include <iostream>


mtsComponentState::mtsComponentState(void)
{
    this->Value = CONSTRUCTED;
}


mtsComponentState::mtsComponentState(const Enum & value)
{
    this->Value = value;
}


const mtsComponentState & mtsComponentState::operator = (const Enum & value)
{
    this->Value = value;
    return *this;
}


void mtsComponentState::ToStream(std::ostream & outputStream) const
{
    outputStream << mtsComponentState::ToString(this->Value);
}


const std::string & mtsComponentState::ToString(const Enum & value)
{
    static const std::string stateNames[] = { "constructed", "initializing", "ready", "active", "finishing", "finished", "unknown" };
    if ((value < CONSTRUCTED) || (value > FINISHED)) {
        return stateNames[UNKNOWN];
    } else {
        return stateNames[value];
    }
}


void mtsComponentState::SerializeRaw(std::ostream & outputStream) const
{
    cmnSerializeRaw(outputStream, static_cast<int>(this->Value));
}

void mtsComponentState::DeSerializeRaw(std::istream & inputStream)
{
    int state;
    cmnDeSerializeRaw(inputStream, state);
    this->Value = static_cast<Enum>(state);
}

bool mtsComponentState::operator == (const mtsComponentState & state) const
{
    return (this->Value == state.Value);
}


bool mtsComponentState::operator != (const mtsComponentState & state) const
{
    return !(*this == state);
}


bool mtsComponentState::operator >= (const mtsComponentState & state) const
{
    return (this->Value >= state.Value);
}


bool mtsComponentState::operator > (const mtsComponentState & state) const
{
    return (this->Value > state.Value);
}


bool mtsComponentState::operator <= (const mtsComponentState & state) const
{
    return (this->Value <= state.Value);
}


bool mtsComponentState::operator < (const mtsComponentState & state) const
{
    return (this->Value < state.Value);
}
