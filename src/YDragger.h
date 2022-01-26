// Drag Reporter.
//

#include <View.h>
#include <Message.h>
#include <Messenger.h>
#include <Point.h>
#include <OS.h>

#ifndef Y_DRAGGER_H
#define Y_DRAGGER_H

// message when dragged
//  where, B_POINT_TYPE, cursur position.
//  offset, B_POINT_TYPE, offset from previous position.
//  status, B_INT32_TYPE, drag status Y_DRAG_{BEGIN,CONTINUE,END}


class YDragger : public BView
{
public:
  enum coordinate
  {
    Y_VIEW_COORDINATE,
    Y_WINDOW_COORDINATE,
    Y_SCREEN_COORDINATE,
    Y_PARENT_COORDINATE,
  };

  enum
  {
    Y_DRAG_BEGIN,
    Y_DRAG_CONTINUE,
    Y_DRAG_END,
  };

  YDragger(BRect rect,
	   const char *name,
	   uint32 resizingMode,
	   uint32 flags,
	   
	   BMessenger mTarget,
	   uint32 command,
	   coordinate coord = Y_VIEW_COORDINATE);
  virtual ~YDragger(void);

protected:
  virtual void MouseDown(BPoint point);
  virtual void MouseMoved(BPoint point, uint32 transit, const BMessage *message);
  virtual void MouseUp(BPoint point);

private:
  void Send(BPoint point, uint32 status);

  BMessenger mTarget;
  uint32 mCommand;
  coordinate mCoordinate;
  BPoint mPrevPoint;
  bool mIsDragging;
};


#endif
