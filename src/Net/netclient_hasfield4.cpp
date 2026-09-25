//============================================================================
//  0x6F6736D0 - Net::NetClient vtable slot 6 (networking wave 5).
//
//  `bool HasField4() const { return this->field4 != 0; }` - the same
//  `field4` AppendReceivedData reads as its own connection-record key
//  (netclient_recvappend.cpp) and PostEvent_0x1F's own family reads the
//  same way (netclient_notifyevent_0x1f.cpp). Four instructions, no
//  callees, `retn` bare (no stack args).
//============================================================================
struct NetClientHasField4Self
{
    bool HasField4() const;
};

bool NetClientHasField4Self::HasField4() const
{
    return *(int*)((const char*)this + 4) != 0;
}
