//

#include <KernelKit.h>
#include <Window.h>
#include "YApplication.h"
#include "YDragger.h"
#include <iostream>

YDragger::YDragger(BRect rect,
		   const char *name,
		   uint32 resizingMode,
		   uint32 flags,

		   BMessenger target,
		   uint32 command,
		   coordinate coord)
  : BView(rect, name, resizingMode, flags)
  , mTarget(target)
  , mCommand(command)
  , mCoordinate(coord)
  , mIsDragging(false)
{
  y_app->SetHelp(this, name);
}

YDragger::~YDragger(void)
{
  y_app->SetHelp(this, NULL);
}


void
YDragger::Send(BPoint point, uint32 status)
{
  BPoint offset;

  switch (mCoordinate)
    {
    default:
    case Y_VIEW_COORDINATE:
      break;

    case Y_PARENT_COORDINATE:
      point = this->ConvertToParent(point);
      break;

    case Y_WINDOW_COORDINATE:
      point = this->ConvertToScreen(point);
      point = this->Window()->ConvertFromScreen(point);
      break;

    case Y_SCREEN_COORDINATE:
      point = this->ConvertToScreen(point);
      break;
    }

  if (status == Y_DRAG_BEGIN)
      offset = BPoint(0, 0);
  else
    offset = BPoint(point.x - mPrevPoint.x, point.y - mPrevPoint.y);
  mPrevPoint = point;

  BMessage msg(mCommand);
  msg.AddPoint("where", point);
  msg.AddPoint("offset", offset);
  msg.AddInt32("status", status);
  mTarget.SendMessage(&msg);
}


void
YDragger::MouseDown(BPoint point)
{
  BView::SetMouseEventMask(B_POINTER_EVENTS);
  mIsDragging = true;
  this->Send(point, Y_DRAG_BEGIN);
}


void
YDragger::MouseMoved(BPoint point, uint32 transit, const BMessage *message)
{
  if (!mIsDragging)
    return;

  uint32 buttons;
  this->GetMouse(&point, &buttons, false);
  this->Send(point, Y_DRAG_CONTINUE);
}

void
YDragger::MouseUp(BPoint point)
{
  if (!mIsDragging)
    return;

  mIsDragging = false;
  this->Send(point, Y_DRAG_END);
}

