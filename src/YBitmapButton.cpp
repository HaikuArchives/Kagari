// Kagari Launcher Button.

#include "YApplication.h"
#include "YBitmapButton.h"
#include <iostream>
#include <Region.h>


YBitmapButton::YBitmapButton(BRect frame,
			     const char *name,
			     const char *label,
			     BMessage *msg,
			     const BBitmap *bitmap,
			     uint32 resizingMode,
			     uint32 flags)
  : BControl(frame, name,
	     label,
	     msg,
	     resizingMode,
	     flags)
  , mBitmap(bitmap)
  , mIsHolding(false)
{
  y_app->SetHelp(this, label);
}


YBitmapButton::~YBitmapButton(void)
{
  y_app->SetHelp(this, NULL);
}


void
YBitmapButton::SetLabel(const char *label)
{
  BControl::SetLabel(label);
  y_app->SetHelp(this, label);
}



void
YBitmapButton::Draw(BRect updateRect)
{
  this->SetDrawingMode(B_OP_OVER);
  this->DrawBitmap(mBitmap, updateRect, updateRect);
  if (this->mIsHolding)
    {
      this->SetDrawingMode(B_OP_INVERT);
      this->DrawBitmap(mBitmap, updateRect, updateRect);
    }
}


void
YBitmapButton::AttachedToWindow(void)
{
  this->SetViewColor(this->Parent()->ViewColor());
}


void
YBitmapButton::MouseDown(BPoint point) 
{
  this->SetMouseEventMask(B_POINTER_EVENTS);

  mIsHolding = true;

  this->Invalidate();
}


void
YBitmapButton::MouseUp(BPoint point)
{
  if (!mIsHolding)
    return;

  if (this->Bounds().Contains(point))
    this->Invoke();

  ::snooze(200 * 1000);

  mIsHolding = false;
  this->Invalidate();
}


void
YBitmapButton::MouseMoved(BPoint point, 
			  uint32 transit, 
			  const BMessage *message)
{
  if (transit == B_ENTERED_VIEW)
    {
      this->SetViewColor(tint_color(this->Parent()->ViewColor(),
      				    B_DARKEN_2_TINT));
      this->Invalidate();
    }
  else if (transit == B_EXITED_VIEW)
    {
      this->SetViewColor(this->Parent()->ViewColor());
      this->Invalidate();
    }
}

