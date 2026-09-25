//============================================================================
//  0x6F306750 - CCameraWar3::BindControllerProps: hand the live controller
//  one callback per camera field.
//
//  Nine registrations, every one of them with a zero blend time except the
//  last, which uses -1.0.  Slot 6 is the only conditional one: in controller
//  mode 2 it goes to the camera's own locked-rotation callback rather than
//  to the generic property publisher, which is why the mode test sits in the
//  middle of an otherwise straight-line function.
//
//  Which property each slot drives is this function and nothing else - the
//  context argument is the property's own address.
//
//  Own translation unit: nine real calls, and CCameraWar3::SetControllerMode
//  reaches this one with a real call of its own.
//============================================================================
#include "gameui.h"
#include "cameracontrollerbind.h"

void CCameraWar3::BindControllerProps()
{
    BindControllerSlotDirect(m_pDefaultController, 8,
                             (void*)CameraOnTransformSlot, this, 0.0f);
    BindControllerSlotRamp(m_pDefaultController, 0,
                           (void*)PublishSlotToProp, &m_angleOfAttack, 0.0f);
    BindControllerSlotRamp(m_pDefaultController, 1,
                           (void*)PublishSlotToProp, &m_targetDistance, 0.0f);
    BindControllerSlotRamp(m_pDefaultController, 3,
                           (void*)PublishSlotToProp, &m_nearZ, 0.0f);
    BindControllerSlotRamp(m_pDefaultController, 2,
                           (void*)CameraOnTargetSlot, this, 0.0f);
    BindControllerSlotRamp(m_pDefaultController, 4,
                           (void*)PublishSlotToProp, &m_fieldOfView, 0.0f);
    BindControllerSlotRamp(m_pDefaultController, 5,
                           (void*)PublishSlotToProp, &m_roll, 0.0f);

    if (m_controllerMode == 2)
        BindControllerSlotRamp(m_pDefaultController, 6,
                               (void*)CameraOnRotationSlotLocked, this, 0.0f);
    else
        BindControllerSlotRamp(m_pDefaultController, 6,
                               (void*)PublishSlotToProp, &m_rotation, 0.0f);

    BindControllerSlotDirect(m_pDefaultController, 7,
                             (void*)CameraOnZoomSlot, this, -1.0f);
}
