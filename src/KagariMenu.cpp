//======================================================================
// Kagari
//  Application Menu.

#include <AppKit.h>
#include <InterfaceKit.h>
#include <StorageKit.h>
#include <Deskbar.h>
#include <algorithm>
#include <climits>
#include "KagariApp.h"
#include "KagariInfo.h"
#include "KagariMenu.h"
#include "KagariDefs.h"
#include <iostream>


//======================================================================
// menu item with bitmap.

class IconMenuItem : public BMenuItem
{
public:
  IconMenuItem(const char *label, BMessage *msg, const BBitmap *icon, int mwergin = 0, int mhmergin = 0);
  virtual ~IconMenuItem(void);

private:
  virtual void DrawContent(void);
  virtual void GetContentSize(float *height, float *width);

  const BBitmap *mIcon;;
  int mWMergin, mHMergin;
  BRect mSize;
};


IconMenuItem::IconMenuItem(const char *label, BMessage *msg, const BBitmap *bmp, int wmergin, int hmergin)
  : BMenuItem(label, msg)
  , mIcon(bmp)
  , mWMergin(wmergin)
  , mHMergin(hmergin)
{
}

IconMenuItem::~IconMenuItem(void)
{
}

void
IconMenuItem::GetContentSize(float *width, float *height)
{
  BMenuItem::GetContentSize(width, height);
  *width += mIcon->Bounds().Width() + mWMergin;
  *height = max(*height, mIcon->Bounds().Height() + mHMergin);
}


void
IconMenuItem::DrawContent(void)
{
  float w, h;
  BMenuItem::GetContentSize(&w, &h);
  h = max((float)0.0, mIcon->Bounds().Height() - h);
  w = mIcon->Bounds().Width() + mWMergin;

  drawing_mode mode = Menu()->DrawingMode();
  Menu()->SetDrawingMode(B_OP_OVER);
  Menu()->DrawBitmap(mIcon);
  Menu()->MovePenBy(w, h);
  Menu()->SetDrawingMode(mode);

  BMenuItem::DrawContent();
}
 


//======================================================================
// application pop up menu.

//----------------------------------------------------------------------
KagariAppMenu::KagariAppMenu(const char *name, KagariInfo *info)
  : BPopUpMenu(name, false, false)
  , mTopNum(0)
  , mInfo(info)
{
}


//----------------------------------------------------------------------
KagariAppMenu::~KagariAppMenu(void)
{
}


//----------------------------------------------------------------------
void
KagariAppMenu::LookUpPosition(void)
{
  BPoint point;
  uint32 buttons;
  BRect screenRect;

  this->GetMouse(&point, &buttons);
  screenRect = BScreen().Frame();

  if (point.y < screenRect.Height() / 2)
    mPosition = POSITION_UPPER;
  else
    mPosition = POSITION_LOWER;
}


//----------------------------------------------------------------------
void
KagariAppMenu::AddLastItem(BMenu* menu, BMenuItem *item)
{
  if (mPosition == POSITION_UPPER)
    menu->AddItem(item);
  else
    menu->AddItem(item, 0);
}


//----------------------------------------------------------------------
void
KagariAppMenu::AddLastItem(BMenu* menu, BMenu *item)
{
  if (mPosition == POSITION_UPPER)
    menu->AddItem(item);
  else
    menu->AddItem(item, 0);
}


//----------------------------------------------------------------------
// make menu item from apprication information.
BMenuItem *
KagariAppMenu::MakeAppItem(uint32 command, const AppRef *ref)
{
  BMessage *msg;
  BMenuItem *item;

  msg = new BMessage(command);
  msg->AddString("signature", ref->signature);
  if (k_pref->GetInt32("menu_icon_size") == B_MINI_ICON)
    item = new IconMenuItem(ref->name, msg, ref->mini_icon, 6, 2);
  else
    item = new IconMenuItem(ref->name, msg, ref->large_icon, 6, 2);
  item->SetTarget(mInfo);

  return item;
}


//----------------------------------------------------------------------
// update menu items.
void
KagariAppMenu::Update(void)
{
  vector<AppRef> info; //, showen_info, hidden_info;
  BMenu *menu;
#if 0
  BMessage *msg;
#endif
  BMenuItem *item;
  uint32 i, n;
  int32 launched = 0;
#if 0
  int32 expire, topN;
#endif
  BMenuItem *about, *pref, *quit;

  mInfo->Lock();
  k_pref->Lock();

  this->LookUpPosition();

  // delete old items.
  this->RemoveItems(0, this->CountItems(), true);

  // get application information.
  mInfo->GetAppInfo(&info);

  // update top menu.
  n = min ((uint32)info.size(), (uint32)k_pref->GetInt32("menu_display_top"));
  if (info.size())
    launched = info[0].launched;

  for (i = 0; i < n; i++)
    {
      if (launched != info[i].launched)
	this->AddLastItem(this, new BSeparatorItem());
      item = this->MakeAppItem(LAUNCH_REQUESTED, &info[i]);
      this->AddLastItem(this, item);
      launched = info[i].launched;
    }

  // update ALL menu.
  if (i < info.size())
    {
      this->AddLastItem(this, new BSeparatorItem());
      this->AddLastItem(this, menu = new BMenu("More.."));
      launched = info[i].launched;

    for (; i < info.size(); i++)
      {
	if (launched != info[i].launched)
	  this->AddLastItem(menu, new BSeparatorItem());
	item = this->MakeAppItem(LAUNCH_REQUESTED, &info[i]);
	this->AddLastItem(menu, item);
	launched = info[i].launched;
      }
    }

#if 0
  this->AddLastItem(this, new BSeparatorItem());
  this->AddLastItem(this, menu = new BMenu("Display"));
  for (i = 0; i < sizeof(sTopN) / sizeof(sTopN[0]); i++)
    {
      msg = new BMessage(SET_TOP_N);
      msg->AddInt32("num", sTopN[i].num);
      menu->AddItem(new BMenuItem(sTopN[i].label, msg));
    }
  topN = k_pref->GetInt32("top"); //mInfo->GetTopN();
  for (i = 0; i < sizeof(sTopN) / sizeof(sTopN[0]) - 1; i++)
    {
      if (sTopN[i].num >= topN)
	break;
    }
  menu->SetTargetForItems(mInfo);
  menu->SetRadioMode(true);
  menu->ItemAt(i)->SetMarked(true);

  this->AddLastItem(this, menu = new BMenu("Expire"));
  for (i = 0; i < sizeof(sExpirePeriod) / sizeof(sExpirePeriod[0]); i++)
    {
      msg = new BMessage(SET_EXPIRE_PERIOD);
      msg->AddInt32("period", sExpirePeriod[i].num);
      menu->AddItem(new BMenuItem(sExpirePeriod[i].label, msg));
    }
  expire = k_pref->GetInt32("expire"); // mInfo->GetExpirePeriod();
  for (i = 0; i < sizeof(sExpirePeriod) / sizeof(sExpirePeriod[0]) - 1; i++)
    {
      if (sExpirePeriod[i].num >= expire)
	break;
    }
  menu->SetTargetForItems(mInfo);
  menu->SetRadioMode(true);
  menu->ItemAt(i)->SetMarked(true);

  this->AddLastItem(this, new BMenuItem("Refresh", new BMessage(REFRESH_REQUESTED)));

  this->AddLastItem(this, new BSeparatorItem());
#endif

  this->AddLastItem(this, new BSeparatorItem());
  pref = new BMenuItem("Preferences", new BMessage(K_PREFERENCES_REQUESTED));
  this->AddLastItem(this, pref);

  this->AddLastItem(this, new BSeparatorItem());
  about = new BMenuItem("About Kagari", new BMessage(K_ABOUT_REQUESTED));
  this->AddLastItem(this, about);
	
  this->AddLastItem(this, new BSeparatorItem());
  quit = new BMenuItem("Quit", new BMessage(B_QUIT_REQUESTED));
  this->AddLastItem(this, quit);

  this->SetTargetForItems(mInfo);
  pref->SetTarget(k_app);
  about->SetTarget(k_app);
  quit->SetTarget(k_app);

  k_pref->Unlock();
  mInfo->Unlock();
}


//----------------------------------------------------------------------
// call when menu opended.
bool
KagariAppMenu::AddDynamicItem(add_state s)
{
  switch (s)
    {
    case B_INITIAL_ADD:
      {
	this->Update();
	return false;
      }
      
    default:
      return false;
    }
};



BPoint
KagariAppMenu::ScreenLocation(void)
{
  BPoint point = BPopUpMenu::ScreenLocation();

  if (!k_pref->GetInt32("menu_avoid_deskbar"))
    return point;

  float w, h;
  BRect frame(BDeskbar().Frame());

  this->GetPreferredSize(&w, &h);

  switch (BDeskbar().Location())
    {
    case B_DESKBAR_TOP:
      point.y = frame.bottom;
      break;

    case B_DESKBAR_BOTTOM:
      point.y = frame.top - h;
      break;

    case B_DESKBAR_LEFT_TOP:
      point.x = frame.right;
      break;

    case B_DESKBAR_LEFT_BOTTOM:
      point.x = frame.right;
      point.y -= h;
      break;

    case B_DESKBAR_RIGHT_TOP:
      point.x = frame.left - w;
      break;

    case B_DESKBAR_RIGHT_BOTTOM:
      point.x = frame.left - w;
      point.y -= h;
      break;

    default:
      break;
    }

  return point;
}
