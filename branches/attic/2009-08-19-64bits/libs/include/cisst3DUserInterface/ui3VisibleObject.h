/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):	Balazs Vagvolgyi, Simon DiMaio, Anton Deguet
  Created on:	2008-05-23

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _ui3VisibleObject_h
#define _ui3VisibleObject_h

#include <cisstCommon/cmnGenericObject.h>
#include <cisstCommon/cmnClassServices.h>
#include <cisstCommon/cmnClassRegisterMacros.h>
#include <cisstCommon/cmnAccessorMacros.h>

#include <cisstVector/vctFixedSizeVectorTypes.h>
#include <cisstVector/vctTransformationTypes.h>

#include <cisst3DUserInterface/ui3ForwardDeclarations.h>
#include <cisst3DUserInterface/ui3VTKForwardDeclarations.h>
#include <cisst3DUserInterface/ui3SceneManager.h>

// Always include last!
#include <cisst3DUserInterface/ui3Export.h>

/*!
 Provides a base class for all visible objects.
*/
class CISST_EXPORT ui3VisibleObject: public cmnGenericObject
{
    friend class ui3SceneManager;
    friend class ui3VisibleList;

public:

    ui3VisibleObject(const std::string & name = "Unnamed");

    /*!
     Destructor
    */
    virtual ~ui3VisibleObject(void) {};

    /*! This method needs to be overload by the user to create the
      actual VTK objects */
    virtual bool CreateVTKObjects(void) = 0;

    virtual vtkProp3D * GetVTKProp(void);

    void Show(void);

    void Hide(void);

    void SetPosition(const vctDouble3 & position);

    void SetOrientation(const vctDoubleMatRot3 & rotationMatrix);

    void SetScale(const double & scale);

    template <bool _storageOrder>
    void SetTransformation(const vctFrameBase<vctMatrixRotation3<double, _storageOrder> > & frame) {
        this->SetPosition(frame.Translation());
        this->SetOrientation(frame.Rotation());
    }

    vctDoubleFrm3 GetTransformation(void) const;

    void SetVTKMatrix(vtkMatrix4x4 * matrix);

    void Lock(void);

    void Unlock(void);

    void AddPart(vtkProp3D * part);

    bool IsAddedToScene(void) const;

    void WaitForCreation(void) const;


 protected:
    typedef ui3SceneManager::VTKHandleType VTKHandleType;

    void SetVTKHandle(VTKHandleType handle) {
        this->VTKHandle = handle;
    }

 private:
    // make assembly private to control access
    vtkAssembly * Assembly;
    std::vector<vtkProp3D *> Parts;

 protected:
    vtkMatrix4x4 * Matrix;
    ui3SceneManager * SceneManager;
    VTKHandleType VTKHandle;
    ui3VisibleList * ParentList;


 private:
	bool IsSceneList;

 protected:

    virtual void PropagateVisibility(bool visible);

    CMN_DECLARE_MEMBER_AND_ACCESSORS(bool, Created);

    CMN_DECLARE_MEMBER_AND_ACCESSORS(bool, Visible);

    CMN_DECLARE_MEMBER_AND_ACCESSORS(std::string, Name);
    
};


#endif // _ui3VisibleObject_h