/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):	Rajesh Kumar, Anton Deguet
  Created on:	2008-03-12

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*! 
  \file 
  \brief Cartesian Velocity move parameters.
*/


#ifndef _prmVelocityCartesianSet_h
#define _prmVelocityCartesianSet_h

#include <cisstVector/vctFixedSizeVectorTypes.h>
#include <cisstParameterTypes/prmTransformationManager.h>
#include <cisstParameterTypes/prmMotionBase.h>

// Always include last
#include <cisstParameterTypes/prmExport.h>


/*! Cartesian velocity motion command arguments - default motion is a
  line between the current position and the goal
*/
class CISST_EXPORT prmVelocityCartesianSet: public prmMotionBase
{
	CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);
 protected:
    /*! The controllable frame node in the transformation tree this
      command will act on */
    prmTransformationBasePtr MovingFrame;

    /*! The controllable/reference frame node with respect to which
      the goal is specified */
    prmTransformationBasePtr ReferenceFrame;
    
    /*! Linear goal (time derivative of magnitude of the vector to
      goal. */
    vctDouble3 Velocity;

    /*! Vector of rate of orientation change */
    vctDouble3 VelocityAngular;
    
    /*! desired time derivative of linear velocity - six vectors*/
    vctDouble3 Acceleration;

    /*! desired time derivative of angualr velocity - six vectors*/
    vctDouble3 AccelerationAngular;
    
    /*!  probably also a 6-vector -- do we really need this or just set vel to 0? */
    vctBool6 Mask;

 public:
	/*! default constructor */
    inline prmVelocityCartesianSet():
        MovingFrame(NULL),
        ReferenceFrame(NULL)
            {}
    
    /*! constructor with all parameters */
    prmVelocityCartesianSet(const prmTransformationBasePtr & movingFrame, 
                            const prmTransformationBasePtr & referenceFrame,
                            const vctDouble3 & velocity,
                            const vctDouble3 & velocityAngular,
                            const vctDouble3 & acceleration,
                            const vctDouble3 & accelerationAngular,
                            const vctBool6 & mask):
        MovingFrame(movingFrame),
        ReferenceFrame(referenceFrame),  
        Velocity(velocity),
        VelocityAngular(velocityAngular),
        Acceleration(acceleration),
        AccelerationAngular(accelerationAngular),
        Mask(mask)
        {}

    /*! destructor */
    virtual ~prmVelocityCartesianSet();

	/*! Set translation and angular velocities only
      \param velocities (3 translational and 3 angular velocities)
      \return void
    */
    inline void SetGoal(const vctDouble6 & velocities)
    { 
        this->Velocity[0] = velocities[0];
        this->Velocity[1] = velocities[1];
        this->Velocity[2] = velocities[2];   
        this->VelocityAngular[0] = velocities[3];
        this->VelocityAngular[1] = velocities[4];
        this->VelocityAngular[2] = velocities[5];
    }

    /*Set Translational Velocities only
      \param velocity translational velocity
      \return void
    */
    inline void SetTranslationGoal(const vctDouble3 & velocity)
    {
        this->Velocity.Assign(velocity);
    }
	
    /*! Set angular velocity only
      \param velocityAngular angular velocity
      \return void
    */
    inline void SetRotationGoal(const vctDouble3 & velocityAngular)
    {
        this->VelocityAngular.Assign(velocityAngular);
    } 
	
    /*! Get current goal 
      \return vctDouble3 current goal velocity
    */
    inline vctDouble3 GetGoal(void) const
    {
        return this->Velocity;
    }
    
    /*! Set the reference frame for current move
      \param referenceFrame frame node in the tree
      \return void
    */
    inline void SetReferenceFrame(const prmTransformationBasePtr & referenceFrame)
    {
        this->ReferenceFrame = referenceFrame;
    } 
    
    /*! Get the reference frame for current move
      \return prmTransformationBasePtr reference frame node in the tree
    */
    inline prmTransformationBasePtr GetReferenceFrame(void) const
    {
        return this->ReferenceFrame;
    }

    /*! Set the moving frame for current move
      \param movingFrame frame node in the tree
      \return void
    */
    inline void SetMovingFrame(const prmTransformationBasePtr & movingFrame)
    {
        this->MovingFrame =  movingFrame;
    }
    
    /*! Get the moving frame for current move
      \return prmTransformationBasePtr moving frame node in the tree
    */
    inline prmTransformationBasePtr GetMovingFrame(void) const
    {
        return this->MovingFrame;
    }
    
    /*! Set the velocity parameter
      \param velocity planned velocity
      \return void
    */
    inline void SetVelocity(const vctDouble3 & velocity)
    {
        this->Velocity = velocity;
    } 
    
    /*! Get the velocity parameter
      \return  vctDouble3 current planned velocity
    */
    inline vctDouble3 GetVelocity(void) const
    {
        return this->Velocity;
    }

    /*! Set the angular velocity parameter
      \param velocityAngular planned angular velocity
      \return void
    */
    inline void SetAngularVelocity(const vctDouble3 & velocityAngular)
    {
        this->VelocityAngular = velocityAngular;
    } 
    
    /*! Get the angular velocity parameter
      \return  vctDouble3 current planned angular velocity
    */
    inline vctDouble3 GetAngularVelocity(void) const
    {
        return this->VelocityAngular;
    }
    
    /*! Set the acceleration parameters only
      \param acceleration target acceleration vector
      \return void
    */
    inline void SetAcceleration(const vctDouble3 & acceleration)
    {
        this->Acceleration = acceleration;
    }

    /*! Get the current acceleration parameters
      \return vctDouble3 acceleration vector
    */
	inline vctDouble3 GetAcceleration(void) const
    {
        return this->Acceleration;
    }
    
    /*! Set the deceleration parameters only
      \param acceleration target angular acceleration vector
      \return void 
    */
    inline void SetAngularAcceleration(const vctDouble3 & accelerationAngular)
    {
        this->AccelerationAngular = accelerationAngular;
    }
    
    /*! Get the current angular acceleration parameters
      \return vctDouble3 angular acceleration vector
    */
	inline vctDouble3 GetAngularAcceleration(void) const
    {
        return this->AccelerationAngular;
    }

    /*! Set the mask only
      \param mask mask for moves
      \return void
    */
    inline void SetMask(const vctBool6 & mask)
    {
        this->Mask = mask;
    }

    /*! Get the current mask parameter
      \return prmBoolVec current mask
    */
	inline vctBool6 GetMask(void) const
    {
        return this->Mask;
    }

}; // _prmVelocityCartesianSet_h


CMN_DECLARE_SERVICES_INSTANTIATION(prmVelocityCartesianSet);


#endif

