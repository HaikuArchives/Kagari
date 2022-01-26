//

#ifndef Y_ADAPTOR_H
#define Y_ADAPTOR_H

#include <View.h>

class YAdaptor : public BView
{
public:
  YAdaptor(void);

  void AdaptView(BView *view = NULL);

  virtual void GetPreferredSize(float *width, float *height);
  virtual void FrameResized(float width, float height);

private: 

  BView *mChild;
};


#endif

