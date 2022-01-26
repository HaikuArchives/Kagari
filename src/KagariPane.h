//======================================================================
// KagariPane
//

#ifndef KAGARI_PANE_H
#define KAGARI_PANE_H

#include <Box.h>
#include "YBitmapButton.h"
#include "YDragger.h"
#include "KagariInfo.h"


class KagariPane : public BWindow
{
public:
  KagariPane(void);
  ~KagariPane(void);

  virtual void Show(void);
  virtual void Hide(void);

private:
  void MessageReceived(BMessage *message);
  void WorkspaceActivated(int32 workspace, bool active);
  void ScreenChanged(BRect frame, color_space mode);
  BPoint FitToScreen(BPoint point);

  void Refresh(void);
  void Redraw(void);
  void Shrink(BPoint point);

  std::vector <AppRef> mAppInfo;
  std::vector <BView*> mAppButtons;
  BBox *mFrame;
  YDragger *mMover, *mDragger, *mPreferer;
  int32 mPadding;
  BView *mContainer;

  BPoint mPosition;
  int32 mWidth, mHeight;
};

#endif

