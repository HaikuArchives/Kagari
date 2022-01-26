//----------------------------------------------------------------------
// Kagari
//  Application View.


#ifndef _KAGARI_VIEW_H
#define _KAGARI_VIEW_H

#include <Application.h>
#include <InterfaceKit.h>
#include <SupportKit.h>
#include "KagariInfo.h"

//----------------------------------------------------------------------
// application view.

class _EXPORT KagariView : public BView
{
public:
  KagariView(BMessenger parent, BRect bounds, const char *name, BBitmap *icon);
  KagariView(BMessage *archive);
  static KagariView *Instantiate(BMessage *archive);
  virtual status_t Archive(BMessage *archive, bool deep = true) const;
  virtual ~KagariView(void);

private:
  virtual void MouseDown(BPoint point);
  virtual void Draw(BRect updateRect);

  BMessenger mParent;
  BBitmap *mIcon;
};

#endif
