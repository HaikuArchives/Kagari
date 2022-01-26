//----------------------------------------------------------------------
// Kagari
//  Intelligent Laucher.
//

#include <AppKit.h>
#include <InterfaceKit.h>
#include <StorageKit.h>
#include <Deskbar.h>
#include "YApplication.h"
#include "YProperty.h"
#include "YDialog.h"
#include "KagariApp.h"
#include "KagariDefs.h"
#include "KagariInfo.h"
#include "KagariMenu.h"
#include "KagariView.h"
#include "KagariPane.h"
#include <iostream>
#include <strstream>


//----------------------------------------------------------------------
YProperty  *k_pref;
KagariApp  *k_app;
KagariInfo *k_info;

static y_property_arg
sPropertyArgs[] =
{
  {"display_flag", B_INT32_TYPE, "1"},
  {"expire_date", B_INT32_TYPE, "30"},

  {"menu_display_top", B_INT32_TYPE, "20"},
  {"menu_icon_size", B_INT32_TYPE, "16"},
  {"menu_avoid_deskbar", B_INT32_TYPE, "1"},

  {"pane_position", B_POINT_TYPE, "100 50"},
  {"pane_width", B_INT32_TYPE, "3"},
  {"pane_height", B_INT32_TYPE, "3"},
  {"pane_icon_size", B_INT32_TYPE, "32"},
  {"pane_lock", B_INT32_TYPE, "0"},
  {"pane_on_top", B_INT32_TYPE, "1"},

  {"pref_position", B_POINT_TYPE, "100 200"},
  
  {NULL},
};


    
//----------------------------------------------------------------------

KagariApp::KagariApp(void)
  : YApplication(cAppSignature)
  , mMenuLastClosed(0)
  , mPref(NULL)
  , mAbout(NULL)
{
  k_pref = mProperty = new YProperty("/boot/home/config/settings/Kamnagi/Kagari/preferences", sPropertyArgs);
  k_app = this;

  cout << "begin info initiarize.\n";

  k_info = mInfo = new KagariInfo("");

  k_pref->StartWatching("display_flag", this);

  mInfo->Run();

  cout << "begin popup menu initiarize.\n";

  mMenu = new KagariAppMenu("", mInfo);

  cout << "begin floating launcher initiarize.\n";

  mPane = new KagariPane();
  //  mPane->Show();

  cout << "begin preference initiarize.\n";

  //  mPref = new KagariPreferences();
  //  mPref->Show();

  cout << "initiarize finished.\n";

  this->DisplayFlagModified();
}


KagariApp::~KagariApp(void)
{
}

void
KagariApp::Quit(void)
{
  mPane->Lock();
  mPane->Quit();

  mInfo->Lock();
  mInfo->Quit();

  mProperty->StopWatching("display_flag", this);
  delete mProperty;

  delete mMenu;

  BDeskbar deskbar;

  while (deskbar.HasItem(cViewName))
    deskbar.RemoveItem(cViewName);

  YApplication::Quit();
}


void
KagariApp::DisplayFlagModified(void)
{
  int32 flag;

  flag = k_pref->GetInt32("display_flag");

  mPane->Lock();

  if (flag == K_DISPLAY_LAUNCHER_AND_DESKBAR ||
      flag == K_DISPLAY_LAUNCHER)
    {
      if (mPane->IsHidden())
	mPane->Show();
    }
  else
    {
      if (!mPane->IsHidden())
	mPane->Hide();
    }

  mPane->Unlock();

  BDeskbar deskbar;

  if (flag == K_DISPLAY_LAUNCHER_AND_DESKBAR ||
      flag == K_DISPLAY_DESKBAR)
    {
      if (!deskbar.HasItem(cViewName))
	{
	  KagariView *view =
	    new KagariView(BMessenger(this),
			   BRect(0, 0, 15, 15),
			   cViewName,
			   new BBitmap(this->GetIcon(B_MINI_ICON)));
	  deskbar.AddItem(view); 
	  delete view;
	}
    }
  else
    {
      while (deskbar.HasItem(cViewName))
	deskbar.RemoveItem(cViewName);
    }
}
	


void
KagariApp::MessageReceived(BMessage *msg)
{
  switch (msg->what)
    {
    default:
      BApplication::MessageReceived(msg);
      break;
      
    case K_PREFERENCES_REQUESTED:
      {
	if (mPref)
	  {
	    mPref->Lock();
	    mPref->Activate();
	    mPref->Unlock();
	  }
	else
	  {
	    mPref = new KagariPreferences();
	    mPref->Show();
	  }
	break;
      }

    case K_PREFERENCES_QUITED:
      {
	if (!mPref)
	  break;

	mPref = NULL;
	break;
      }

    case K_ABOUT_REQUESTED:
      {
	if (mAbout)
	  {
	    mAbout->Lock();
	    mAbout->Activate();
	    mAbout->Unlock();
	  }
	else
	  {
	    mAbout = new KagariAbout();
	    mAbout->Show();
	  }
	break;
      }

    case K_ABOUT_QUITED:
      {
	if (!mAbout)
	  break;

	mAbout = NULL;
	break;
      }

    case B_MOUSE_DOWN:
      {
	bigtime_t when;

	msg->FindInt64("when", &when);
	if (when < mMenuLastClosed)
	  break;

	BPoint point = msg->FindPoint("where");

	BRect range = BRect(point.x - 5, point.y - 5,
			    point.x + 5, point.y + 5);

	mMenu->Go(point, true, false, range);

	mMenuLastClosed = system_time();

	break;
      }

    case Y_PROPERTY_MODIFIED:
      {
	if (strcmp(msg->FindString("name"), "display_flag") == 0)
	  this->DisplayFlagModified();
	break;
      }
    }
}


//----------------------------------------------------------------------
int
main(void)
{
  KagariApp app;
  app.Run();

  return 0;
}


