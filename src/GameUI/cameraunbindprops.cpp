//============================================================================
//  0x6F306870 - CCameraWar3::UnbindControllerProps: the same nine slots
//  again with a null callback and a null context, which is what clears a
//  binding, plus two more on the second sink at +0x5C.
//
//  No mode test here: the conditional slot 6 of BindControllerProps has
//  nothing to be conditional about when both arguments are zero.
//
//  Own translation unit: eleven real calls, and SetControllerMode reaches
//  this one with a real call.
//============================================================================
#include "gameui.h"
#include "cameracontrollerbind.h"

void CCameraWar3::UnbindControllerProps()
{
    BindControllerSlotDirect(m_pDefaultController, 8, 0, 0, 0.0f);
    BindControllerSlotRamp(m_pDefaultController, 0, 0, 0, 0.0f);
    BindControllerSlotRamp(m_pDefaultController, 1, 0, 0, 0.0f);
    BindControllerSlotRamp(m_pDefaultController, 2, 0, 0, 0.0f);
    BindControllerSlotRamp(m_pDefaultController, 3, 0, 0, 0.0f);
    BindControllerSlotRamp(m_pDefaultController, 4, 0, 0, 0.0f);
    BindControllerSlotRamp(m_pDefaultController, 5, 0, 0, 0.0f);
    BindControllerSlotRamp(m_pDefaultController, 6, 0, 0, 0.0f);
    BindControllerSlotDirect(m_pDefaultController, 7, 0, 0, 0.0f);

    //  Two on the event sink rather than on the controller, and with the
    //  camera as context: these are the ones the mode switch puts back.
    BindControllerSlotDirect(m_pEventSink, 1,
                             (void*)CameraOnModeEnter, this, 0.0f);
    BindControllerSlotEvent(m_pEventSink, 4,
                            (void*)CameraOnModeLeave, this, 0.0f);
}
