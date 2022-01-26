// table.

#include <iostream>
#include "YLayoutUtils.h"
#include "YFrame.h"

YFrame::YFrame(const char *name, BView *child, border_style border)
  : BBox(BRect(0, 0, 0, 0), 
	 name,
	 B_FOLLOW_NONE, 
	 B_WILL_DRAW | /* B_FRAME_EVENTS | */ B_NAVIGABLE_JUMP,
	 border)
  , mChild(child)
{
  this->AddChild(child);
  mMergin.left = mMergin.top = mMergin.right = mMergin.bottom = 0;
}

YFrame::~YFrame(void)
{
}

void
YFrame::SetMergin(BRect mergin)
{
  mMergin = mergin;
}

void
YFrame::GetPreferredSize(float *width, float *height)
{
  font_height fontHeight;

  mInner.left = mMergin.left + this->PenSize();
  mInner.right = mMergin.right + this->PenSize();
  mInner.top = mMergin.top + this->PenSize();
  mInner.bottom = mMergin.bottom + this->PenSize();

  if (this->Label())
    {
      this->GetFontHeight(&fontHeight);
      mInner.top += 
	fontHeight.ascent + fontHeight.descent + fontHeight.leading;
    }
  else if (this->LabelView())
    {
      this->LabelView()->GetFontHeight(&fontHeight);
      mInner.top +=
	fontHeight.ascent + fontHeight.descent + fontHeight.leading;
    }

  mChild->GetPreferredSize(&mChildWidth, &mChildHeight);

  *width = mChildWidth + mInner.left + mInner.right;
  *height = mChildHeight + mInner.top + mInner.bottom;
}


void
YFrame::FrameResized(float width, float height)
{
  BBox::FrameResized(width, height);

  YLayoutUtils::ZoomToFit(mChild,
			  BRect(mInner.left,
				mInner.top,
				width - mInner.right,
				height - mInner.bottom),
			  mChildWidth,
			  mChildHeight);

}





