//============================================================================
//  0x6F34FD50 - CEscMenu's vtable slot 1, the scalar deleting destructor
//  (override of CObserver::DeleteSelf).  See escmenu.h.
//
//  Own translation unit for the same reason observer_deleteself.cpp's own
//  header comment gives for CObserver: ~CEscMenu (escmenudtor.cpp,
//  0x6F34FC90) must not be inlined into it.  The shipped body makes a real
//  `call sub_6F34FC90`; compiled beside that destructor's definition this
//  build inlines the whole five-field release chain instead.
//============================================================================
#include "escmenu.h"

//  Storm.dll ordinal 403.
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                        unsigned int flags);

static const char kDelete[] = "delete";

CEscMenu* CEscMenu::DeleteSelf(int flags)
{
    this->CEscMenu::~CEscMenu();
    if ((flags & 1) && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}
