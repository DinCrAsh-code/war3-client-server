//============================================================================
//  Everything `JASS_PauseGame`'s CGameUI slice calls that is *not* CGameUI's
//  own.
//
//  Each entry is a `mov eax, <addr>` / `jmp eax` redirect to the real,
//  unhooked shipped body (gameuithunks.cpp), so calls out of this batch
//  reach the game's own code and the `.mix` still links.  They are
//  placeholders and nothing more: the frame framework (0x6F60xxxx), the
//  text and texture engines (0x6F4Bxxxx-0x6F52xxxx) and the CNetData game
//  state (0x6F53xxxx) are separate batches of the same closure and their
//  real bodies replace these one file at a time.
//
//  Batch B has landed, so fifteen of them are gone.  Three declarations it
//  did not move stay here because their real definitions have exactly this
//  spelling and moving them would churn four call sites for nothing:
//  GetGameString (Config/gamestrings.cpp - included below rather than
//  re-declared, so that its one declaration stays the definition's own),
//  IsAgentHandleLive
//  (Agent/agenthandlelive.cpp) and QueryNearestObstruction
//  (Storm/stormobstructionpick.cpp), plus SPathField::Set
//  (Misc/pathfieldset.cpp).  Everything else batch B reconstructed now has
//  a class of its own and is declared where that class lives -
//  Net/netdata.h, Agent/agenttypedslots.h, Widget/widget.h, GameUI/gameui.h
//  and Config/CMiscCustom.h.
//
//  The class names are recovered, not invented, wherever the shipped code
//  says one: the Storm allocation tag ".\\CScreenFrame.cpp" inside
//  0x6F60A6E0 names CScreenFrame, and the vtable stores name CSimpleFrame
//  (0x6F609C70), CSimpleFontString (0x6F60D580) and RCStaticString
//  (0x6F61D5A0).
//
//  **The declared argument bytes are load-bearing.**  Every callee below
//  cleans its own stack, so a declaration that pushes the wrong number of
//  dwords displaces the caller's frame from the call onwards - the crash
//  class tools/thunk_abi_audit.py exists to catch.  The shipped `retn <n>`
//  is written next to every one of them.
//============================================================================
#ifndef GAMEUITHUNKS_H
#define GAMEUITHUNKS_H

#include "renderdevice.h"
#include "spriteattach.h"

#include "fmatrix.h"   //  0x6F4B5450 - Math/matrix44transform.cpp
#include "uispan.h"    //  0x6F4C6480 - Math/uispan.cpp

#include "gamestrings.h"

#include "frame.h"

//  0x6F4E8260 is reconstructed now, in Render/spriteslotvec3.cpp;
//  Render/sprite.h declares it and this file includes it above.

//----------------------------------------------------------------------------
//  The frame framework.  Its own batch (src/Frame/) has landed, so the two
//  opaque classes this file used to declare - a "CScreenFrame" carrying nine
//  methods and a CSimpleFontString carrying five - are gone: every one of
//  those addresses is reconstructed there, on the class whose vtable the
//  shipped constructor actually stamps, and the call sites name it directly.
//  Three of them are still redirects and are declared on CSimpleFrame in
//  frame.h: ClearFlagBit (0x6F608930), SetOwnedFrame (0x6F608550) and
//  EnsureRenderObject (0x6F60A6E0).
//----------------------------------------------------------------------------
//  0x6F611D40 and 0x6F61D5A0 are reconstructed now - CTextFrame::SetString
//  in Frame/textframesetstring.cpp and CBackdropGenerator's own constructor
//  in Frame/backdropctor.cpp - and both classes are declared on frame.h,
//  which this header already includes.  The two opaque one-method classes
//  that stood here are gone.

//  0x6F62B170 is reconstructed in Frame/eventkeyquery.cpp as
//  IsEventKeyRegistered - the body answers a membership question about this
//  thread's event table, not a resource lookup.

//----------------------------------------------------------------------------
//  Game strings and the config store.
//----------------------------------------------------------------------------
//  0x6F5C9650 (`retn 4`) - key in ecx, destination buffer in edx, buffer
//  size on the stack; answers whether the key resolved.  Reconstructed:
//  Config/gamestrings.cpp.

//  Storm ordinal 503 - append into a bounded buffer.  Declared as ordinary
//  C++ and not `extern "C"` for the reason Config/config.h records: the
//  pipeline recovers a readable name from the comment MASM writes beside a
//  mangled EXTRN, and an undecorated C symbol has none.
void __stdcall Storm_503(char* dest, const char* src, unsigned int size);

//----------------------------------------------------------------------------
//  Sprites and textures - `src/Render`'s batch.
//----------------------------------------------------------------------------
//  0x6F4D3100, 0x6F4D32E0, 0x6F4E8260 and 0x6F4E82B0 are reconstructed now,
//  in src/Render/; sprite.h declares all four and this file includes it
//  above.
//  0x6F732360 is reconstructed now, in Render/loadtexturebypath.cpp;
//  Render/texture.h declares it and this file reaches it through
//  spriteattach.h above.
//  0x6F4B5450 is reconstructed now, in Math/matrix44transform.cpp; fmatrix.h
//  declares it, and this file includes it above.
//  0x6F4C6480 is reconstructed now, in Math/uispan.cpp, and named for what
//  it does: the two constants are the UI coordinate space's own extents
//  (0.8 across, 0.6 down), not angles.  uispan.h declares it.
//  0x6F526A40 and 0x6F5266D0 are reconstructed now, in
//  Render/viewprojectionmatrix.cpp and Render/viewportrect.cpp;
//  Render/renderdevice.h declares both and this file includes it above.

//----------------------------------------------------------------------------
//  Agents, widgets and handles - `src/Net`/`src/Misc`'s batch.
//----------------------------------------------------------------------------
//  0x6F0418A0 (`retn 0`) - is this agent's own handle still live?
//  Reconstructed: Agent/agenthandlelive.cpp.
int   __fastcall IsAgentHandleLive(const void* agent);
//  0x6F012790 (`retn 8`) - the nearest obstruction along the segment
//  `from` -> `to`, written through `outDistance`; 0 when nothing was hit and
//  the distance is then not written at all.  `from` in ecx, `to` in edx.
//  Reconstructed: Storm/stormobstructionpick.cpp.
int   __fastcall QueryNearestObstruction(const float* from, const float* to,
                                         float* outDistance, int flags);
//  0x6F01FCB0 (`retn 4`) - assign a path string into a counted-string field,
//  answering `this`.  Reconstructed: Misc/pathfieldset.cpp.
struct SPathField { SPathField* __thiscall Set(const char* path); };

#endif
