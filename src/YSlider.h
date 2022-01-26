// YSlider.
//

#ifndef Y_SLIDER_H
#define Y_SLIDER_H

#include <Slider.h>

class YSlider : public BSlider
{
public:
  YSlider(BRect frame, const char *name,
	  const char *label,
	  BMessage *message,
	  int32 minValue, int32 maxValue,
	  thumb_style thumbType = B_BLOCK_THUMB,
	  uint32 resizingMode = B_FOLLOW_LEFT | B_FOLLOW_TOP,
	  uint32 flags = B_FRAME_EVENTS|B_WILL_DRAW | B_NAVIGABLE);
  
protected:
  virtual void GetPreferredSize(float *width, float *height);
  virtual void ResizeToPreferred(void);
  virtual void FrameResized(float width, float height);

public:
  BRect mDefaultFrame;
};

#endif
