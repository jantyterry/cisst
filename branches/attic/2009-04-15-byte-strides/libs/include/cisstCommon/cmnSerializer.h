/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
  Author(s):  Anton Deguet
  Created on: 2007-04-08

  (C) Copyright 2007-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


/*!
  \file
  \brief Declaration of cmnSerializer and functions cmnSerializeRaw
*/

#ifndef _cmnSerializer_h
#define _cmnSerializer_h

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnGenericObject.h>
#include <cisstCommon/cmnClassRegister.h>
#include <cisstCommon/cmnThrow.h>

#include <string>
#include <vector>
#include <fstream>

#include <cisstCommon/cmnExport.h>


/*! Serialization helper function for a basic type.  This function
  performs a cast to char pointer (<code>char *</code>) using
  <code>reinterpret_cast</code> and then writes the result to the
  output stream.  If the write operation fails, an exception is thrown
  (<code>std::runtime_error</code>).

  This function should be use to implement the SerializeRaw method of
  classes derived from cmnGenericObject. */
template <class _elementType>
inline void cmnSerializeRaw(std::ostream & outputStream, const _elementType & data)
    throw (std::runtime_error) 
{
    outputStream.write(reinterpret_cast<const char *>(&data), sizeof(_elementType));
    if (outputStream.fail()) {
        cmnThrow(std::runtime_error("cmnSerializer::SerializeRaw(_elementType): Error occured with std::ostream::write"));
    }
}


/*! Serialization helper function for an STL string.  This function
  first serializes the string size and then writes all the string
  characters (<code>char</code>) to the output stream.  If the write
  operation fails, an exception is thrown
  (<code>std::runtime_error</code>). */
inline void cmnSerializeRaw(std::ostream & outputStream, const std::string & data)
    throw (std::runtime_error)
{
    const std::string::size_type size = data.size();
    cmnSerializeRaw(outputStream, size);
    outputStream.write(data.c_str(), size * sizeof(std::string::value_type));
    if (outputStream.fail()) {
        cmnThrow(std::runtime_error("cmnSerializer::SerializeRaw(std::string): Error occured with std::ostream::write"));
    }        
}


/*!
  \brief Serialization utility class.
  \ingroup cisstCommon

  This class allows to serialize objects of different types (all
  derived from cmnGenericObject) as well as the information required
  to deserialize them (see cmnDeSerializer) to and from an STL output
  stream.

  The result is a sequence of class informations (i.e. class name as a
  string and unique identifier) and objects.  Each object is sent
  preceeded by its unique class identifier and class information is
  preceeded by a 0.  This allows cmnDeSerializer to determine what is
  currently deserialized.

  The unique identifer used is the address of the object
  cmnClassServices associated to each class.

  When an object needs to be serialized, the method Serialize will
  first verify that the class information (name and identifier) has
  already been serialized.  If this is not the case, it will serialize
  the class information and then the object itself.

  \sa cmnDeSerializer cmnGenericObject
*/
class CISST_EXPORT cmnSerializer: public cmnGenericObject {
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);

 public:
    /*! Constructor.

      \param outputStream any stream derived from
      <code>std::ostream</code>.  The output stream must be created
      with the open flag <code>std::ostream::binary</code>.
     */
    inline cmnSerializer(std::ostream & outputStream):
        OutputStream(outputStream)
    {
        if (!OutputStream) {
            CMN_LOG_CLASS(1) << "Output stream provided is not valid" << std::endl;
        } 
    }
    

    /*! Serialize an object.  This method will first verify that the
      information (cmnClassServices) related to the class of the
      object has been serialized using SerializeServices.  Once this
      is done, it will serialize the class identifier (pointer on the
      class information, see cmnClassServices and cmnClassRegister)
      and finally call the object's method <code>SerializeRaw</code>.
      
      \param object An object of a class derived from cmnGenericObject.  The
      class of the object must have the virtual method
      <code>SerializeRaw</code> re-defined to properly serialize the
      content of the object.

      \note As this method relies on cmnSerializeRaw, it might throw
      an exception.
     */
    inline void Serialize(const cmnGenericObject & object) {
        // get object services and send information if needed
        const cmnClassServicesBase* servicesPointer = object.Services();
        this->SerializeServices(servicesPointer);
        // serialize the object preceeded by its services pointer
        cmnSerializeRaw(this->OutputStream, servicesPointer);
        object.SerializeRaw(this->OutputStream);
    }


    /*! Serialize the class information if needed.  This method will
      write the class name (as an STL string) and the type identifier
      used on the serialization end to the output stream.  The type
      identifier used is the address of the class services object
      (cmnClassServices).  To avoid sending the same information
      multiple times, this class (cmnSerializer) maintains a list of
      the class identifier already sent.

      It is important to note that a regular user doesn't need to use
      this method as the Serialize method performs this task.  It is
      provided for advanced users who need or want to serialize all
      the class information first and then serialize their objects.
      This can be used to avoid time delays while serializing and
      de-serializing objects.

      \param servicesPointer Pointer on object cmnClassServices.  Use
      cmnClassRegister::FindClassServices,
      <code>className::Services()</code> or
      <code>object.Services()</code> to retrieve the correct pointer.

      \note As this method relies on cmnSerializeRaw, it might throw
      an exception.
    */
    inline void SerializeServices(const cmnClassServicesBase * servicesPointer) {
        // search for services pointer to see if the information has
        // been sent
        const const_iterator begin = ServicesContainer.begin();
        const const_iterator end = ServicesContainer.end();
        const_iterator found = std::find(begin, end, servicesPointer);
        // this "services" has not been sent 
        if (found == end) {
            CMN_LOG_CLASS(7) << "Sending information related to class " << servicesPointer->GetName() << std::endl; 
            // sent the info with null pointer so that reader can
            // differentiate from other services pointers
            const cmnClassServicesBase * invalidClassServices = 0;
            cmnSerializeRaw(this->OutputStream, invalidClassServices);
            cmnSerializeRaw(this->OutputStream, servicesPointer->GetName());
            cmnSerializeRaw(this->OutputStream, servicesPointer);
            ServicesContainer.push_back(servicesPointer);
        }
    }


 private:

    std::ostream & OutputStream;

    typedef std::list<const cmnClassServicesBase *> ServicesContainerType;
    typedef ServicesContainerType::const_iterator const_iterator;
    typedef ServicesContainerType::iterator iterator;

    ServicesContainerType ServicesContainer;
};


// Add services instantiation
CMN_DECLARE_SERVICES_INSTANTIATION(cmnSerializer)


#endif // _cmnSerialize_h
