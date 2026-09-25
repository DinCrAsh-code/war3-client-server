//============================================================================
//  0x6F2F5EE0 - CGameUI::PostCameraEvents: tell everything that watches the
//  camera that `camera` is the one to look at now.
//
//  The same "one id, two ways to deliver it" shape CWidget::PostAgentEvent
//  (widget_postagentevent.cpp) already records, seven times over: with
//  `direct` set each event goes through an observer's own slot 2 with the
//  id twice - once as the low bound and once as the high bound of a
//  one-event range - and with it clear each is a plain post instead.
//
//  Three ids on the UI itself, one on the camera host, two on the selection
//  host and one attach/detach pair on the audio host.  The two shared calls
//  in the middle (the host's own camera slot and the controller
//  notification) really are shared: the shipped code has a copy of them in
//  each arm, which is /O2 duplicating a two-call tail across a branch it has
//  already taken, not two different sequences.
//
//  Own translation unit: nine real calls.
//============================================================================
#include "gameui.h"

void CGameUI::PostCameraEvents(CCameraWar3* camera, int direct)
{
    //  One if/else with the whole tail in both arms, not a shared tail
    //  after two smaller ones.  The shipped code has two complete bodies
    //  each ending in its own `retn 8` and tests `direct` exactly once,
    //  straight out of its stack slot; sharing the tail makes MSVC keep
    //  `direct` in a callee-saved register across nine calls and test it a
    //  second time, which costs both the register and the test.
    if (direct != 0)
    {
        Dispatch(0x40060064, 0x40060064, camera);
        Dispatch(0x40060065, 0x40060065, camera);
        Dispatch(0x40060066, 0x40060066, camera);
        m_pWorldFrameWar3->Dispatch(0x400500CD, 0x400500CD, camera);
        m_pWorldFrameWar3->SetCamera(camera);
        m_pWorldFrameWar3->NotifyControllerChanged(camera->m_pController);
        m_pSelectionHost->DispatchRange(0x40060064, 0x40060064, camera);
        m_pSelectionHost->DispatchRange(0x40060066, 0x40060066, camera);
        m_pPortrait->SetPortraitFrame(camera);
    }
    else
    {
        CObserver::PostEvent(0x40060064, camera);
        CObserver::PostEvent(0x40060065, camera);
        CObserver::PostEvent(0x40060066, camera);
        m_pWorldFrameWar3->CObserver::PostEvent(0x400500CD, camera);
        m_pWorldFrameWar3->SetCamera(camera);
        m_pWorldFrameWar3->NotifyControllerChanged(camera->m_pController);
        m_pSelectionHost->Post(0x40060064, camera);
        m_pSelectionHost->Post(0x40060066, camera);
        m_pPortrait->CancelControlTrack();
    }
}
