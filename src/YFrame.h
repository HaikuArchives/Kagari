// borderd frame.

#ifndef Y_FRAME_H
#define Y_FRAME_H

#include <Box.h>

class YFrame : public BBox
{
public:
  YFrame(const char *name, BView *child, border_style border = B_NO_BORDER);
  virtual ~YFrame(void);

  void SetMergin(BRect mergin);

  virtual void GetPreferredSize(float *width, float *height);

protected:
  virtual void FrameResized(float width, float height);

private:
  BView *mChild;
  int32 mX, mY;
  BRect mMergin, mInner;
  float mChildWidth, mChildHeight;
};



#endif
