//============================================================================
//  The two literals `.\Texture.cpp` hands Storm on every allocation: the
//  module tag in the __FILE__ slot, and "HTEXTURE", the allocation tag every
//  texture object and every key copy carries.  Both are one string in the
//  shipped image, so they are one definition here rather than a static per
//  translation unit.
//
//  The module's three globals - the two tables and the empty key the solid
//  table's lookup is handed - are funcmap.DATA rows with real addresses, so
//  they are *declared* in texture.h and never defined: build_mix.py writes
//  the binding, and defining one here would quietly give the .mix a private
//  copy of a table the unhooked shipped code is still using.
//============================================================================
#include "texture.h"
#include "imagefile.h"

extern const char kTextureCpp[] = ".\\Texture.cpp";
extern const char kHTexture[]   = "HTEXTURE";
extern const char kHMaterial[]  = "HMATERIAL";

//  `.\\blp.cpp`, the module tag CBlpFile::Release hands Storm.
extern const char kBlpCpp[]     = ".\\blp.cpp";
