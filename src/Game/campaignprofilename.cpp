//============================================================================
//  0x6F3D9FC0 and 0x6F3D9FD0 - the campaign profile's two accessors, two
//  instructions each.  Neither is inlined anywhere in the image, which is
//  what gives each its own address.
//============================================================================
#include "campaignprofile.h"

const char* CampaignProfile::Name()
{
    return m_name;
}

char* CampaignProfile::PathBuffer()
{
    return m_path;
}
