
#include <View.h>
#include "YLayoutUtils.h"

#define _rule_(r1, r2, r3, r4) ((r1 << 12) | (r2 << 8) | (r3 << 4) | r4)
    
void
YLayoutUtils::ZoomToFit(BView *view,
			BRect frame,
			float preferred_width,
			float preferred_height)
{
  float x, y, w, h;

  if (preferred_width >= frame.Width())
    {
      x = frame.left;
      w = frame.Width();
    }
  else
    {
      switch (view->ResizingMode() & _rule_(0, 7, 0, 7))
	{
	case B_FOLLOW_NONE:
	case B_FOLLOW_LEFT:
	default:
	  x = frame.left;
	  w = preferred_width;
	  break;

	case B_FOLLOW_H_CENTER:
	  x = frame.left + (frame.Width() - preferred_width) / 2;
	  w = preferred_width;
	  break;

	case B_FOLLOW_RIGHT:
	  x = frame.left + (frame.Width() - preferred_width);
	  w = preferred_width;
	  break;

	case B_FOLLOW_LEFT_RIGHT:
	  x = frame.left;
	  w = frame.Width();
	  break;
	}
    }

  if (preferred_height >= frame.Height())
    {
      y = frame.top;
      h = frame.Height();
    }
  else
    {
      switch (view->ResizingMode() & _rule_(7, 0, 7, 0))
	{
	case B_FOLLOW_NONE:
	case B_FOLLOW_TOP:
	default:
	  y = frame.top;
	  h = preferred_height;
	  break;

	case B_FOLLOW_V_CENTER:
	  y = frame.top + (frame.Height() - preferred_height) / 2;
	  h = preferred_height;
	  break;

	case B_FOLLOW_BOTTOM:
	  y = frame.top + (frame.Height() - preferred_height);
	  h = preferred_height;
	  break;

	case B_FOLLOW_TOP_BOTTOM:
	  y = frame.top;
	  h = frame.Height();
	  break;
	}
    }
  
  view->MoveTo(BPoint(x, y));
  view->FrameMoved(BPoint(x, y));
  view->ResizeTo(w, h);
  view->FrameResized(w, h);
}



