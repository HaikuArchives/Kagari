//

#ifndef SP_GLVIEW_H
#define SP_GLVIEW_H

#include <GLView.h>
#include <Locker.h>

class SpBehavior;

class SpGLView : public BGLView {
public:
  SpGLView(BRect frame, uint32 type);
  virtual ~SpGLView(void);

  void SetV(float v);
  void SetR(float r);
  void Expand(float d);
  void Restart();

  virtual void AttachedToWindow(void);
  virtual void FrameResized(float newWidth, float newHeight);
  virtual void ErrorCallback(GLenum which);
 
  void Update(void);

  virtual void MessageReceived(BMessage *msg);
  virtual void GetPreferredSize(float *width, float *height);

private:
  BRect mInitFrame;
  SpBehavior *mBehavior;
  BLocker mLocker;
};

#endif
