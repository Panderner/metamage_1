// ATSTypes.h

#ifndef NITROGEN_ATSTYPES_H
#define NITROGEN_ATSTYPES_H

#ifndef __ATSTYPES__
#include <ATSTypes.h>
#endif
#ifndef NITROGEN_MACTYPES_H
#include "Nitrogen/MacTypes.h"
#endif
#ifndef NITROGEN_FILES_H
#include "Nitrogen/Files.h"
#endif
#ifndef NITROGEN_MIXEDMODE_H
#include "Nitrogen/MixedMode.h"
#endif
#ifndef NITROGEN_IDTYPE_H
#include "Nitrogen/IDType.h"
#endif

namespace Nitrogen
  {
   class FMFontFamilyTag {};
   typedef IDType< FMFontFamilyTag, ::FMFontFamily, 0 > FMFontFamily;
   typedef FMFontFamily FontID;
    
   class FMFontStyleTag {};
   typedef IDType< FMFontStyleTag, ::FMFontStyle, 0 > FMFontStyle;
  }

#endif
