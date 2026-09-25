//============================================================================
//  SVersionGate - the three-word descriptor the item load path builds on the
//  heap for every field whose presence depends on the save-stream version.
//
//  Each site keeps a file-scope pointer to one of these, allocates it once
//  through Storm (12 bytes, tagged with the header's own file and line 0x9D
//  - a *different* file from `.\CItem.cpp`, so the allocation really is
//  inside an inline helper in a header), and then overwrites all three
//  fields before every use.  Two sites in CItem::Load share one gate, which
//  is what makes these per-*helper* statics rather than per-call-site ones.
//
//  Which fields they are: `since` and `until` bound the stream versions the
//  field exists in, and `target` is where to put it - or null, meaning
//  "this version has the field but this object does not want it".
//============================================================================
#ifndef VERSIONGATE_H
#define VERSIONGATE_H

struct SVersionGate
{
    //  {0, -1, 0} - "every version, nowhere".  Every user overwrites all
    //  three immediately; the constructor's values only ever survive if the
    //  allocation is used before it is filled in, which nothing does.
    SVersionGate() : m_since(0), m_until(-1), m_target(0) {}

    int   m_since;      // +0x00
    int   m_until;      // +0x04
    void* m_target;     // +0x08
};

#endif
