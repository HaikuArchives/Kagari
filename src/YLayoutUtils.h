//
//


#ifndef Y_LAYOUTUTILS_H
#define Y_LAYOUTUTILS_H

#include <Rect.h>
#include <View.h>

class  YLayoutUtils {
 public:
  enum
  {
    H_LEFT    = 0x01,
    H_CENTER  = 0x02,
    H_RIGHT   = 0x04,
    H_FILL    = 0x08,
    H_MASK    = 0x0f,
    V_TOP     = 0x10,
    V_CENTER  = 0x20,
    V_BOTTOM  = 0x40,
    V_FILL    = 0x80,
    V_MASK    = 0xf0,
  };

  static void ZoomToFit(BView *view,
			BRect frame,
			float preferred_width,
			float preferred_heigh);
};


#endif
