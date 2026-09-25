//============================================================================
//  See stormnotifysingleton.cpp.
//============================================================================
#ifndef STORMNOTIFYSINGLETON_H
#define STORMNOTIFYSINGLETON_H

//  0x6F00D990 - get the Storm singleton, then forward (handle, value) to
//  its own vtable-less Terrain-rendering method (0x6F741AC0, out of
//  scope).  Own translation unit: the shipped code keeps this a real,
//  out-of-line call from its one caller (CItem::StoreAndNotifyHandle,
//  item.cpp) despite being small enough for /O2 to inline on its own if
//  it could see both bodies at once - address neighbourhoods put the two
//  in different modules (0x6F2Bxxxx vs 0x6F00xxxx), which this split
//  reproduces mechanically.
void __fastcall NotifyStormSingleton(int handle, int value);

#endif
