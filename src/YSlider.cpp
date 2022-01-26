// YSlider
//

#include <algorithm>
#include "YSlider.h"


YSlider::YSlider(BRect frame, const char *name,
		 const char *label,
		 BMessage *message,
		 int32 minValue, int32 maxValue,
		 thumb_style thumbType,
		 uint32 resizingMode,
		 uint32 flags)
  : BSlider(frame, name,
	    label,
	    message,
	    minValue, maxValue,
	    thumbType,
	    resizingMode,
	    flags)
  , mDefaultFrame(frame)
{
}

#include <iostream>

void
YSlider::GetPreferredSize(float *width, float *height)
{
  BSlider::GetPreferredSize(width, height);
  *width = mDefaultFrame.Width();
  return;

  //  *width = mDefaultFrame.Width();
  //  *width = this->Bounds().Width());
  *height = std::max(*height, this->Bounds().Height() - 1);
}

void
YSlider::ResizeToPreferred(void)
{
  BView::ResizeToPreferred();
}

#include <iostream>

void
YSlider::FrameResized(float width, float height)
{
  BSlider::FrameResized(width, height);
}
