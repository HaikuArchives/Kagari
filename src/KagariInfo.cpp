//----------------------------------------------------------------------
// Kagari
//  Application Information.

#include <AppKit.h>
#include <InterfaceKit.h>
#include <SupportKit.h>
#include <StorageKit.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include <deque>
#include <map>
#include <set>
#include <algorithm>
#include <string>
#include <cctype>
#include <cstring>

#include "KagariApp.h"
#include "KagariInfo.h"
#include "KagariDefs.h"

//======================================================================
// an instace.
 
KagariInfo *gInfo;


//======================================================================
// path constant.

static const char *cDirectory = "/boot/home/config/settings/Kamnagi/Kagari";
static const char *cLogFile = "/boot/home/config/settings/Kamnagi/Kagari/launch.log";
//static const char *cPrefFile = "/boot/home/config/settings/Kamnagi/Kagari/preferences";


//======================================================================
// an applicatino info.

AppInfo::AppInfo(void)
  : mLargeIcon(BRect(0, 0, 31, 31), B_CMAP8)
  , mMiniIcon(BRect(0, 0, 15, 15), B_CMAP8)
  , mIsActive(false)
{
}


AppInfo::AppInfo(const AppInfo &app)
  : mLargeIcon(app.mLargeIcon.Bounds(), app.mLargeIcon.ColorSpace())
  , mMiniIcon(app.mMiniIcon.Bounds(), app.mMiniIcon.ColorSpace())
  , mIsActive(false)
{
  mLargeIcon.SetBits(app.mLargeIcon.Bits(),
		     app.mLargeIcon.BitsLength(),
		     0,
		     app.mLargeIcon.ColorSpace());

  mMiniIcon.SetBits(app.mMiniIcon.Bits(),
		     app.mMiniIcon.BitsLength(),
		     0,
		     app.mMiniIcon.ColorSpace());
}


status_t
AppInfo::Init(const char *signature)
{
  // disable application info.
  mIsActive = false;
  
  // find application
  if (be_roster->FindApp(signature, &mRef) != B_OK)
    return B_ERROR;

  // read application mini icon.
  if (BNodeInfo::GetTrackerIcon(&mRef, &mLargeIcon, B_LARGE_ICON) != B_OK)
    return B_ERROR;

  if (BNodeInfo::GetTrackerIcon(&mRef, &mMiniIcon, B_MINI_ICON) != B_OK)
    return B_ERROR;

  // activate application info.
  mIsActive = true;

  return B_OK;
}


void
AppInfo::ExternalLaunched(int32 time)
{
  if (mLaunchedTime.size() == 0)
    mLaunchedTime.push_back(time);
  else if (mLaunchedTime.size() == 1)
    mLaunchedTime[0] = time;
}

void
AppInfo::InternalLaunched(int32 time)
{
  mLaunchedTime.push_back(time);
}


//======================================================================
// application information watcher.

//----------------------------------------------------------------------
KagariInfo::KagariInfo(const char *about)
  : BLooper("")
  , mAbout(about)
{
  this->CreateDirectory();
  this->ReadLog();
  //  this->Refresh();
  this->InitRead(mAppInfo.begin());
}


//----------------------------------------------------------------------
KagariInfo::~KagariInfo(void)
{
  be_roster->StartWatching(BMessenger(this));

  this->WriteLog();
}


//----------------------------------------------------------------------
void
KagariInfo::CreateDirectory(void)
{
  // create settings directory.
  create_directory(cDirectory, 0755);
}


//----------------------------------------------------------------------
// read log file.
void
KagariInfo::ReadLog(void)
{
  string signature;
  uint32 launched;

  ifstream in(cLogFile);

  while (in)
    {
      in >> launched >> signature;
      if (!in || signature.find("application/") != 0)
	break;
      mAppInfo[signature.c_str()].InternalLaunched(launched);
    }
}


//----------------------------------------------------------------------
// write log file.
void
KagariInfo::WriteLog(void)
{
  app_map::iterator i;
  uint32 j;
  int32 clock, expire_second;
  
  ofstream out(cLogFile, ios::out | ios::trunc);
  if (!out)
    return;
  
  clock = real_time_clock();
  expire_second = k_pref->GetInt32("expire_date") * 60 * 60 * 24;

  for (i = mAppInfo.begin(); i != mAppInfo.end(); i++)
      for (j = 0; j < (*i).second.mLaunchedTime.size(); j++)
	if (clock - (*i).second.mLaunchedTime[j] < expire_second)
	  out << (*i).second.mLaunchedTime[j] << ' '
	      << (*i).first.String() << '\n';

  out.close();
}


//----------------------------------------------------------------------
// read an item, and polling
void
KagariInfo::InitRead(app_map::iterator i)
{
  this->Lock();

  if (i == mAppInfo.end())
    {
      be_roster->StartWatching(BMessenger(this), B_REQUEST_LAUNCHED);
    }
  else
    {
      (*i).second.Init((*i).first.String());
      
      BMessage msg(K_INFO_INIT);
      mWatcher.SendMessage(&msg);
      
      mAppInfoInitIterator = ++i;

      this->PostMessage(K_INFO_INIT);
    }

  this->Unlock();
}
  

  
//----------------------------------------------------------------------
// refresh applicatino information.
void
KagariInfo::Refresh(void)
{
  app_map::iterator i;

  this->Lock();

  for (i = mAppInfo.begin(); i != mAppInfo.end(); i++)
    (*i).second.Init((*i).first.String());

  this->Unlock();
}


//----------------------------------------------------------------------
// application launched.
void
KagariInfo::InternalLaunched(const char *signature)
{
#if 0
  // check if app is active or not
  if (be_roster->IsRunning(signature))
    return;
#endif

  // add launch information.
  this->Lock();
  
  mAppInfo[signature].InternalLaunched(real_time_clock());

  BMessage msg(B_SOME_APP_LAUNCHED);
  msg.AddString("be:signature", signature);
  mWatcher.SendMessage(&msg);

  this->Unlock();
}


//----------------------------------------------------------------------
// application launched.
void
KagariInfo::ExternalLaunched(const char *signature)
{
  // avoid illegul signature.
  if (!*signature || strchr(signature, ' '))
    return;

  // add launch information.
  this->Lock();
  
  if (!mAppInfo.count(signature))
    mAppInfo[signature].Init(signature);
  mAppInfo[signature].ExternalLaunched(real_time_clock());

  BMessage msg(B_SOME_APP_LAUNCHED);
  msg.AddString("be:signature", signature);
  mWatcher.SendMessage(&msg);

  this->Unlock();
}


//----------------------------------------------------------------------
// get application information.
bool
ltinfo_name(const AppRef &r1, const AppRef &r2)
{
  if (r1.launched > r2.launched)
    return true;
  else if (r1.launched < r2.launched)
    return false;

  const char *s1, *s2;
  for (s1 = r1.name, s2 = r2.name; *s1 && *s2; s1++, s2++)
    if (toupper(*s1) < toupper(*s2))
      return true;
    else if (toupper(*s1) > toupper(*s2))
      return false;

  return false;
}


bool
ltinfo_freq(const AppRef &r1, const AppRef &r2)
{
  if (r1.launched > r2.launched)
    return true;
  else if (r1.launched < r2.launched)
    return false;

  if (r1.recently_launched_time > r2.recently_launched_time)
    return true;

  return false;
}



void
KagariInfo::GetAppInfo(vector<AppRef> *info, int32 sort_option)
{
  app_map::iterator i;
  AppRef ref;

  this->Lock();

  // make referencece of active application.
  for (i = mAppInfo.begin(); i != mAppInfo.end(); i++)
    {
      if (!(*i).second.mIsActive)
	continue;

      ref.signature = (*i).first.String();
      ref.name = (*i).second.mRef.name;
      ref.mini_icon = &((*i).second.mMiniIcon);
      ref.large_icon = &((*i).second.mLargeIcon);
      ref.launched = (*i).second.mLaunchedTime.size();
      ref.recently_launched_time = *(*i).second.mLaunchedTime.rbegin();
      info->push_back(ref);
    }

  // sort referencecs.
  if (sort_option == K_SORT_WITH_NAME)
    sort(info->begin(), info->end(), ltinfo_name);
  else
    sort(info->begin(), info->end(), ltinfo_freq);

  this->Unlock();
}


//----------------------------------------------------------------------
void
KagariInfo::MessageReceived(BMessage *msg)
{
  switch (msg->what)
    {
    default:
      BLooper::MessageReceived(msg);
      break;

    case K_INFO_INIT:
      this->InitRead(mAppInfoInitIterator);
      break;

      // launch report from be_rosdeter.
    case B_SOME_APP_LAUNCHED:
      {
	const char *signature;

	signature = msg->FindString("be:signature");
	this->ExternalLaunched(signature);

	break;
      }

      // launch request from menu.
    case LAUNCH_REQUESTED:
      {
	const char *signature;
	this->Lock();
	signature = msg->FindString("signature");
	this->InternalLaunched(signature);
	be_roster->Launch(signature);
	this->Unlock();
	break;
      }

#if 0
      // refresh request from menu.
    case REFRESH_REQUESTED:
      {
	this->Refresh();
	break;
      }

      // remove replicant.
    case REMOVE_MYSELF:
      {
	be_app->PostMessage(B_QUIT_REQUESTED);

	break;
      }

    case B_ABOUT_REQUESTED:
      {
	k_pref->Lock();
	if (k_pref->GetInt32("pane_icon_size") == 16)
	  k_pref->SetInt32("pane_icon_size", 16);
	else
	  k_pref->SetInt32("pane_icon_size", 32);
	k_pref->Unlock();

	//	(new BAlert(cViewName, mAbout.String(), "OK"))->Go();
	break;
      }
#endif
    }
}
