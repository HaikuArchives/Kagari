// My Application base.
//

#ifndef Y_APP_H
#define Y_APP_H

#include <Application.h>
#include <Bitmap.h>
#include <String.h>
#include <Mime.h>
#include "BubbleHelper.h"

class YApplication : public BApplication
{
public:
  YApplication(const char *signature);

  const BBitmap *GetIcon(icon_size size);
  BBitmap *DuplicateLargeIcon(void);
  BBitmap *DuplicateMiniIcon(void);
  const char *GetName(void);
  const char *GetVersion(void);
  const char *GetDescription(void);
  void SetHelp(BView *view, const char *label);

private:
  BBitmap mLargeIcon, mMiniIcon;
  BString mName, mVersion, mDescription;
  BubbleHelper mHelper;
};

extern YApplication *y_app;

#endif
