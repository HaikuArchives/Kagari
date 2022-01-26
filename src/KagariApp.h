
#ifndef KAGARI_APP_H
#define KAGARI_APP_H

#include "YApplication.h"
#include "KagariDefs.h"
#include "KagariInfo.h"
#include "KagariMenu.h"
#include "KagariPane.h"
#include "KagariPreferences.h"
#include "KagariAbout.h"
#include "YProperty.h"

class KagariApp : public YApplication
{
public:
  KagariApp(void);
  virtual ~KagariApp(void);
  virtual void MessageReceived(BMessage *msg);

private:
  virtual void Quit(void);

  void DisplayFlagModified(void);

  KagariInfo *mInfo;
  KagariAppMenu *mMenu;
  KagariPane *mPane;
  KagariPreferences *mPref;
  KagariAbout *mAbout;

  YProperty *mProperty;

  bigtime_t mMenuLastClosed;
};


extern YProperty  *k_pref;
extern KagariApp  *k_app;
extern KagariInfo *k_info;

#endif
