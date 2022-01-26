// My Application base
//

#include <AppKit.h>
#include <StorageKit.h>
#include <cstdio>
#include "YApplication.h"

YApplication *y_app;

YApplication::YApplication(const char *signature)
  : BApplication(signature)
  , mLargeIcon(BRect(0, 0, 31, 31), B_CMAP8)
  , mMiniIcon(BRect(0, 0, 15, 15), B_CMAP8)
{
  BFile file;
  BAppFileInfo app_file_info;
  app_info app_info;
  version_info ver_info;
  char buf[256];

  y_app = (YApplication*)be_app;

  be_app->GetAppInfo(&app_info);
  file.SetTo(&app_info.ref, B_READ_WRITE);
  app_file_info.SetTo(&file);

  app_file_info.GetIcon(&mLargeIcon, B_LARGE_ICON);
  app_file_info.GetIcon(&mMiniIcon,  B_MINI_ICON);
  app_file_info.GetVersionInfo(&ver_info, B_APP_VERSION_KIND);

  mName = app_info.ref.name;
  sprintf(buf, "%d.%d.%d.%c%d",
	 (int)ver_info.major, (int)ver_info.middle, (int)ver_info.minor,
	 "dabggf"[ver_info.variety],  (int)ver_info.internal);
  mVersion = buf;
  sprintf(buf, "%s %s.\n%s", 
	  mName.String(), mVersion.String(), ver_info.long_info);
  mDescription = buf;
}


const BBitmap* 
YApplication::GetIcon(icon_size size)
{
  if (size == B_MINI_ICON)
    return &mMiniIcon;
  else if (size == B_LARGE_ICON)
    return &mLargeIcon;
  else
    return NULL;
}


BBitmap* 
YApplication::DuplicateLargeIcon(void)
{
  BMessage message;

  mLargeIcon.Archive(&message);
  return new BBitmap(&message);
}


BBitmap* 
YApplication::DuplicateMiniIcon(void)
{
  BMessage message;

  mMiniIcon.Archive(&message);
  return new BBitmap(&message);
}


const char*
YApplication::GetName(void)
{
  return mName.String();
}


const char*
YApplication::GetVersion(void)
{
  return mVersion.String();
}


const char*
YApplication::GetDescription(void)
{
  return mDescription.String();
}


void
YApplication::SetHelp(BView *view, const char *label)
{
  mHelper.SetHelp(view, label);
}
