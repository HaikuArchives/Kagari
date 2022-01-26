//======================================================================
// Kagari
//  Application Information.

#ifndef _KAGARI_INFO_H
#define _KAGARI_INFO_H

#include <AppKit.h>
#include <InterfaceKit.h>
#include <SupportKit.h>
#include <vector>
#include <deque>
#include <map>
#include "YMessageDeliver.h"


//======================================================================
// reference to application informatin.

struct AppRef
{
  const char *signature;
  const char *name;
  const BBitmap *mini_icon, *large_icon;
  int launched;
  int32 recently_launched_time;
};


struct AppInfo
{
  AppInfo(void);
  AppInfo(const AppInfo&);

  status_t Init(const char *signature);
  void InternalLaunched(int32 time);
  void ExternalLaunched(int32 time);

  BBitmap mLargeIcon, mMiniIcon;
  entry_ref mRef;
  bool mIsActive;
  std::deque<int32> mLaunchedTime;
};


//======================================================================
// apprication information watcher

typedef std::map<BString, AppInfo> app_map;

class KagariInfo : public BLooper
{
public:
  enum {
    K_SORT_WITH_NAME,
    K_SORT_WITH_FREQUENCY
  };

  KagariInfo(const char *about);
  virtual ~KagariInfo(void);

  void GetAppInfo(std::vector<AppRef> *info, int32 sort_option = K_SORT_WITH_NAME);
  void Refresh(void);

  void StartWatching(BMessenger target)
    {mWatcher.AddTarget(target);}
  void StopWatching(BMessenger target)
    {mWatcher.RemoveTarget(target);}

private:
  void MessageReceived(BMessage *msg);

  void CreateDirectory(void);
  void ReadLog(void);
  void WriteLog(void);
  void InternalLaunched(const char *signature);
  void ExternalLaunched(const char *signature);

  void InitRead(app_map::iterator i);

  app_map mAppInfo;
  app_map::iterator mAppInfoInitIterator;
  BString mAbout;
  YMessageDeliver mWatcher;
};


#endif

