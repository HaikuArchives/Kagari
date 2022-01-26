//======================================================================
// KagariPane.cpp

#include <AppKit.h>
#include <InterfaceKit.h>
#include "YApplication.h"
#include "YBitmapButton.h"
#include "KagariDefs.h"
#include "KagariApp.h"
#include "KagariInfo.h"
#include "KagariPane.h"
#include <algorithm>


enum
{
  K_RESIZE_PANE = 256,
  K_MOVE_PANE,
  K_PREFERENCES,
};

//======================================================================
// class KagariDragger
class KagariDragger : public YDragger
{
public:
  enum {
    K_LEFT_TOP = 0,
    K_RIGHT_BOTTOM = 1,
  };

  KagariDragger(BRect rect,
		const char *name,
		uint32 resizingMode,
		uint32 flags,
		BMessenger mTarget,
		uint32 command,
		coordinate coord = Y_VIEW_COORDINATE,
		int32 dir = K_LEFT_TOP);

  virtual ~KagariDragger(void);

  void SetDirection(int32 dir);

  virtual void MouseDown(BPoint);
  virtual void Draw(BRect updateRect);

private:
  int32 mDirection;
};


KagariDragger::KagariDragger(BRect rect,
			     const char *name,
			     uint32 resizingMode,
			     uint32 flags,
			     BMessenger target,
			     uint32 command,
			     coordinate coord,
			     int32 dir)
  : YDragger(rect, name,
	     resizingMode,
	     flags | B_FULL_UPDATE_ON_RESIZE,
	     target,
	     command,
	     coord)
  , mDirection(dir)
{
  this->SetViewColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_1_TINT));
}


KagariDragger::~KagariDragger(void)
{
}


void
KagariDragger::SetDirection(int32 dir)
{
  mDirection = dir;
}

void
KagariDragger::MouseDown(BPoint point)
{
  if (mDirection == K_LEFT_TOP)
    {
      if (point.x + point.y <= this->Bounds().Width() + 4)
	YDragger::MouseDown(point);
    }
  else
    {
      if (point.x + point.y >= this->Bounds().Width() - 4)
	YDragger::MouseDown(point);
    }
}


void
KagariDragger::Draw(BRect updateRect)
{
  BRect frame = this->Bounds();
  BPolygon poly;

  if (mDirection == K_LEFT_TOP)
    {
      BPoint tri[] =
      {
	BPoint(frame.left + 4, frame.top + 2),
	BPoint(frame.right - 3, frame.top + 2),
	BPoint(frame.right - 2, frame.top + 3),
	BPoint(frame.right - 2, frame.top + 5),
	BPoint(frame.left + 5, frame.bottom - 2),
	BPoint(frame.left + 3, frame.bottom - 2),
	BPoint(frame.left + 2, frame.bottom - 3),
	BPoint(frame.left + 2, frame.top + 4),
      };
      poly.AddPoints(tri, 8);
    }
  else
    {
      BPoint tri[] = 
      {
	BPoint(frame.right - 4, frame.bottom - 2),
	BPoint(frame.left + 3, frame.bottom - 2),
	BPoint(frame.left + 2, frame.bottom - 3),
	BPoint(frame.left  + 2, frame.bottom - 5),
	BPoint(frame.right - 5, frame.top + 2),
	BPoint(frame.right - 3, frame.top + 2),
	BPoint(frame.right - 2, frame.top + 3),
	BPoint(frame.right - 2, frame.bottom - 4),
      };
      poly.AddPoints(tri, 8);
  }


  this->SetHighColor(tint_color(ui_color(B_WINDOW_TAB_COLOR), B_LIGHTEN_2_TINT ));
  this->SetOrigin(-1, -1);
  this->FillPolygon(&poly);

  this->SetHighColor(tint_color(ui_color(B_WINDOW_TAB_COLOR), B_DARKEN_2_TINT ));
  this->SetOrigin(+1, +1);
  this->FillPolygon(&poly);

  this->SetHighColor(ui_color(B_WINDOW_TAB_COLOR));
  this->SetOrigin(0, 0);
  this->FillPolygon(&poly);
}



//======================================================================
//
KagariPane::KagariPane(void)
  : BWindow(BRect(100, 100, 41, 41),
	    "Floating Launcher",
	    B_BORDERED_WINDOW_LOOK,
	    B_FLOATING_ALL_WINDOW_FEEL,
	    B_NOT_RESIZABLE | B_NOT_CLOSABLE | B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE | B_AVOID_FOCUS | B_ASYNCHRONOUS_CONTROLS,
	    B_ALL_WORKSPACES)
{
  mWidth = k_pref->GetInt32("pane_width");
  mHeight = k_pref->GetInt32("pane_height");
  mPosition = k_pref->GetPoint("pane_position");

  mFrame = new BBox(this->Bounds(), "Frame",
		    B_FOLLOW_ALL_SIDES,
		    B_WILL_DRAW | B_FRAME_EVENTS,
		    B_PLAIN_BORDER);

  mFrame->SetViewColor
    (tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_1_TINT));

  mContainer = new BView(BRect(3, 3, 0, 0), "Container",
			 B_FOLLOW_NONE, B_WILL_DRAW | B_FRAME_EVENTS | B_DRAW_ON_CHILDREN);
  mContainer->SetViewColor
    (tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_1_TINT));

  mMover = new KagariDragger(BRect(0, 0, 0, 0),
			     "Floating Launcher",
			     B_FOLLOW_NONE, B_WILL_DRAW,
			     BMessenger(this), K_MOVE_PANE,
			     YDragger::Y_SCREEN_COORDINATE,
			     KagariDragger::K_LEFT_TOP);

  mPreferer = new KagariDragger(BRect(0, 0, 0, 0),
				"Preferences",
				B_FOLLOW_NONE, B_WILL_DRAW,
				BMessenger(this), K_PREFERENCES,
				YDragger::Y_SCREEN_COORDINATE,
				KagariDragger::K_LEFT_TOP);

  mDragger  =new KagariDragger(BRect(0, 0, 0, 0),
			       "Drag Me To Resize",
			       B_FOLLOW_NONE, B_WILL_DRAW,
			       BMessenger(this), K_RESIZE_PANE,
			       YDragger::Y_PARENT_COORDINATE,
			       KagariDragger::K_RIGHT_BOTTOM);

  this->AddChild(mFrame);
  mFrame->AddChild(mContainer);
  mContainer->AddChild(mMover);
  mContainer->AddChild(mDragger);
  mContainer->AddChild(mPreferer);

  this->Refresh();

  if (!k_pref->GetInt32("pane_on_top"))
    this->SetFeel(B_NORMAL_WINDOW_FEEL);

  k_pref->StartWatching("pane_on_top", this);
  k_pref->StartWatching("pane_icon_size", this);
  k_info->StartWatching(BMessenger(this));
}


KagariPane::~KagariPane(void)
{
  k_info->StopWatching(BMessenger(this));
  k_pref->StopWatching("pane_icon_size", this);
  k_pref->StopWatching("pane_on_top", this);

  k_pref->SetPoint("pane_position", mPosition);

  k_pref->SetInt32("pane_width", mWidth);
  k_pref->SetInt32("pane_height", mHeight);
}


void
KagariPane::Show(void)
{
  this->Redraw();
  this->MoveTo(this->FitToScreen(mPosition));
  BWindow::Show();
}


void
KagariPane::Hide(void)
{
  BWindow::Hide();
}


void
KagariPane::Refresh(void)
{
  int32 i;

  k_pref->Lock();

  for (i = 0; i < (int32)mAppButtons.size(); i++)
    {
      mAppButtons[i]->RemoveSelf();
      delete mAppButtons[i];
    }

  mAppInfo.resize(0);
  mAppButtons.resize(0);
  k_info->GetAppInfo(&mAppInfo, KagariInfo::K_SORT_WITH_FREQUENCY);

  k_pref->Unlock();
}  


void
KagariPane::Redraw(void)
{
  int32 i, disp_item_num;
  icon_size size;

  k_pref->Lock();

  disp_item_num = min(mWidth * mHeight - 2, (int32)mAppInfo.size());
  size = (icon_size)k_pref->GetInt32("pane_icon_size");

  this->ResizeTo(mWidth * size + 6 - 1, mHeight * size + 6 - 1);
  mContainer->ResizeTo(mWidth * size - 1, mHeight * size - 1);

  for (i = mAppButtons.size(); i < disp_item_num; i++)
    {
      BMessage *msg;
      const BBitmap *bitmap;
      YBitmapButton *button;

      msg = new BMessage(LAUNCH_REQUESTED);
      msg->AddString("signature", mAppInfo[i].signature);

      bitmap =  size == B_MINI_ICON
	? mAppInfo[i].mini_icon 
	: mAppInfo[i].large_icon;

      button =
	new YBitmapButton(bitmap->Bounds(),
			  mAppInfo[i].name,
			  mAppInfo[i].name,
			  msg,
			  bitmap,
			  B_FOLLOW_NONE,
			  B_WILL_DRAW);

      button->SetTarget(k_info);

      mContainer->AddChild(button);
      mAppButtons.push_back(button);
    }
  
  for (i = 0; i < disp_item_num; i++)
    mAppButtons[i]->MoveTo((i + 1) % mWidth * size,
			   (i + 1) / mWidth * size);

  for (; i < (int32)mAppButtons.size(); i++)
    mAppButtons[i]->MoveTo(-100, -100);

  mMover->MoveTo(0, 0);
  mMover->ResizeTo(size - 1, size - 1);

  mPreferer->MoveTo((mWidth - 1) * size,
		    (mHeight - 1) * size);
  mPreferer->ResizeTo(size * 2 / 3 - 1, size * 2 / 3 - 1);

  mDragger->MoveTo((mWidth) * size - size * 2 / 3,
		   (mHeight) * size - size * 2 / 3);
  mDragger->ResizeTo(size * 2 / 3 - 1, size * 2 / 3 - 1);

  k_pref->Unlock();
}


void
KagariPane::Shrink(BPoint point)
{
  int32 size;
  int32 w, h;
  
  k_pref->Lock();

  size = k_pref->GetInt32("pane_icon_size");

  if (point.x <= size * mWidth)
    w = max((int32)1, ((int32)point.x - 1) / size + 1);
  else
    w = max((int32)1, ((int32)point.x - size / 2 - 1) / size + 1);

  if (point.y <= size * mHeight)
    h = max((int32)1, ((int32)point.y -  1) / size + 1);
  else
    h = max((int32)1, ((int32)point.y - size / 2 - 1) / size + 1);

  if (w * h == 1)
    if (point.x > point.y)
      w = 2;
    else
      h = 2;

  if (w != mWidth || h != mHeight)
    {
      mWidth = w;
      mHeight = h;
      this->Redraw();
    }
  
  k_pref->Unlock();
}



BPoint
KagariPane::FitToScreen(BPoint point)
{
  BRect screen_frame, my_frame;

  screen_frame = BScreen().Frame();
  my_frame = this->Frame();

  point.x = min(point.x, screen_frame.right - my_frame.Width() - 1);
  point.y = min(point.y, screen_frame.bottom - my_frame.Height() - 1);
  point.x = max(point.x, 1.0F);
  point.y = max(point.y, 1.0F);

  return point;
}


void
KagariPane::WorkspaceActivated(int32 workspace, bool active)
{
  this->MoveTo(this->FitToScreen(mPosition));
}


void
KagariPane::ScreenChanged(BRect frame, color_space mode)
{
  this->MoveTo(this->FitToScreen(mPosition));
  BWindow::ScreenChanged(frame, mode);
}


void
KagariPane::MessageReceived(BMessage *msg)
{
  switch (msg->what)
    {
    default:
      BWindow::MessageReceived(msg);
      break;

    case K_INFO_INIT:
      {
	while (msg = this->MessageQueue()->FindMessage((uint32)K_INFO_INIT))
	  {
	    this->MessageQueue()->RemoveMessage(msg);
	    delete msg;
	  }
      }
      // through down.

    case B_SOME_APP_LAUNCHED:
      {
	k_pref->Lock();
	this->Refresh();
	this->Redraw();
	k_pref->Unlock();
	break;
      }

    case K_PREFERENCES:
      {
	if (msg->FindInt32("status") != YDragger::Y_DRAG_END)
	  break;
	
	BPopUpMenu *menu = 
	  new BPopUpMenu("", false, false);
	menu->AddItem
	  (new BMenuItem("Preferences", new BMessage(K_PREFERENCES_REQUESTED)));
	menu->AddSeparatorItem();
	menu->AddItem
	  (new BMenuItem("About Kagari", new BMessage(K_ABOUT_REQUESTED)));
	menu->AddSeparatorItem();
	menu->AddItem
	  (new BMenuItem("Quit", new BMessage(B_QUIT_REQUESTED)));
	menu->SetTargetForItems(be_app);

	BPoint point = msg->FindPoint("where");
	BRect range(point.x - 5, point.y - 5,
		    point.x + 5, point.y + 5);

	menu->Go(point, true, false, range);

	delete menu;
	
	break;
      }

    case K_RESIZE_PANE:
      {
	if (k_pref->GetInt32("pane_lock"))
	  break;

	BPoint point;

	point = msg->FindPoint("where");

	while (msg = this->MessageQueue()->FindMessage((uint32)K_RESIZE_PANE))
	  {
	    point = msg->FindPoint("where");
	    this->MessageQueue()->RemoveMessage(msg);
	    delete msg;
	  }

	this->Shrink(point);
	this->MoveTo(this->FitToScreen(mPosition));

	break;
      }

    case K_MOVE_PANE:
      {
	if (k_pref->GetInt32("pane_lock"))
	  break;

	BPoint offset;

	offset = msg->FindPoint("offset");

	while (msg = this->MessageQueue()->FindMessage((uint32)K_MOVE_PANE))
	  {
	    offset += msg->FindPoint("offset");
	    this->MessageQueue()->RemoveMessage(msg);
	    delete msg;
	  }

	this->MoveTo(this->FitToScreen(mPosition + offset));
	mPosition = this->Frame().LeftTop();

	break;
      }

    case Y_PROPERTY_MODIFIED:
      {
	string name(msg->FindString("name"));
	if (name == "pane_icon_size")
	  {
	    BPoint point(mDragger->Frame().right,
			 mDragger->Frame().bottom);
	    k_pref->Lock();
	    this->Refresh();
	    this->Shrink(point);
	    this->MoveTo(this->FitToScreen(mPosition));
	    this->Redraw();
    
	    k_pref->Unlock();
	  }
	else if (name == "pane_on_top")
	  {
	    if (k_pref->GetInt32("pane_on_top"))
	      this->SetFeel(B_FLOATING_ALL_WINDOW_FEEL);
	    else
	      this->SetFeel(B_NORMAL_WINDOW_FEEL);
	  }
	break;
      }
    }
}


