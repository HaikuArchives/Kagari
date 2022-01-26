
#include "YAdaptor.h"
#include "YLayoutUtils.h"

YAdaptor::YAdaptor(void)
  : BView(BRect(0, 0, 0, 0),
	  "",
	  B_FOLLOW_NONE,
	  B_WILL_DRAW)
 , mChild(NULL)
{
}

void
YAdaptor::AdaptView(BView *view)
{
  if (mChild)
    {
      this->RemoveChild(mChild);
      delete mChild;
    }

  mChild = view;

  if (mChild)
    {
      mChild->MoveTo(0, 0);
      this->AddChild(mChild);
      this->SetResizingMode(mChild->ResizingMode());
    }
}

#include <iostream>

void
YAdaptor::GetPreferredSize(float *width, float *height)
{
  if (mChild)
    mChild->GetPreferredSize(width, height);
  else
    *width = *height = 0;
}

void
YAdaptor::FrameResized(float width, float height)
{
  if (mChild)
    {
      mChild->MoveTo(0, 0);
      mChild->ResizeTo(width, height);
      mChild->FrameResized(width, height);
    }
}
