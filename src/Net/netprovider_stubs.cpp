//============================================================================
//  Net::NetProvider - the base interface's plain constant-answer slots:
//  22, 24, 26, 28, 29, 30, 32 and 37 each ignore every argument and return
//  a fixed value (0xF except slot 37's `2`), and slot 23 is the one
//  "compare against a cached word, tell vtable+0x90 if it changed" stub in
//  the group.  None of the base class's own overriders touch any of them
//  (the derived-provider `missing_methods` lists in vtable_classes.json
//  share every one of these addresses verbatim), so the base body is the
//  whole story.
//============================================================================

namespace Net
{

class NetProvider
{
public:
    //  0x6F656C60 - vtable slot 22.  `this` and the first stack argument
    //  are both unused; the second stack argument is an out-pointer that
    //  always comes back zeroed.
    void GetZero(void* unused, int* out);

    //  0x6F657050 - vtable slot 23.  Compare the stack word against the
    //  object's own cached +0x2C0 field; on a change, tell vtable+0x90
    //  (code 6) before caching the new value.  Always reports success.
    int SetCachedWord(unsigned short newValue);

    //  0x6F657090 / 0x6F6571B0 / 0x6F6571C0 / 0x6F6571D0 / 0x6F6571E0 /
    //  0x6F6571F0 - vtable slots 24, 26, 28, 29, 30, 32.  Each is
    //  `mov eax, 0Fh` ahead of its own `retn <n>`; the argument count is
    //  the only thing that differs between them, read off each one's own
    //  `retn`.
    int ConstAnswer_Slot24(int, int, int);
    int ConstAnswer_Slot26(int, int);
    int ConstAnswer_Slot28(int, int, int);
    int ConstAnswer_Slot29(int, int);
    int ConstAnswer_Slot30(int, int);
    int ConstAnswer_Slot32(int, int, int, int, int);

    //  0x6F657370 - vtable slot 37.  `mov al, 2 / retn`: a one-byte
    //  constant answer, no stack arguments at all.
    unsigned char ConstAnswer_Slot37();
};

void NetProvider::GetZero(void*, int* out)
{
    *out = 0;
}

int NetProvider::SetCachedWord(unsigned short newValue)
{
    unsigned short* cached = (unsigned short*)((char*)this + 0x2C0);
    if (newValue != *cached)
    {
        typedef void (__thiscall *Method0x90Fn)(void*, int, int, int);
        ((Method0x90Fn)(*(void***)this)[0x90 / 4])(this, 6, 0, 0);
        *cached = newValue;
    }
    return 1;
}

int NetProvider::ConstAnswer_Slot24(int, int, int) { return 0xF; }
int NetProvider::ConstAnswer_Slot26(int, int) { return 0xF; }
int NetProvider::ConstAnswer_Slot28(int, int, int) { return 0xF; }
int NetProvider::ConstAnswer_Slot29(int, int) { return 0xF; }
int NetProvider::ConstAnswer_Slot30(int, int) { return 0xF; }
int NetProvider::ConstAnswer_Slot32(int, int, int, int, int) { return 0xF; }
unsigned char NetProvider::ConstAnswer_Slot37() { return 2; }

}  // namespace Net
