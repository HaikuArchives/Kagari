// Hey Emacs, this file is -*- c++ -*-

#ifndef Y_TABVIEW_H
#define Y_TABVIEW_H

#include <TabView.h>
#include <vector>

class YTabView;

class YTab : public BTab
{
public:
  YTab(YTabView *owner, BView *tabView = NULL);

  virtual void SetLabel(const char *label);

private:
  YTabView *mOwner;
};


class YTabView : public BTabView
{
public:
  YTabView(BRect frame, const char *name,
	   button_width width = B_WIDTH_AS_USUAL,
	   uint32 resizingMode = B_FOLLOW_ALL,
	   uint32 flags = B_FULL_UPDATE_ON_RESIZE | B_WILL_DRAW | B_NAVIGABLE_JUMP | B_FRAME_EVENTS | B_NAVIGABLE);

  void SetMergin(BRect mergin);

  virtual void AddTab(BView *target, BTab *tab = NULL);

  virtual void GetPreferredSize(float *width, float *height);

protected:
  virtual void Select(int32 tab);
  virtual void FrameResized(float width, float height);

private:
  friend class YTab;

  void ResizeChild(int32 n, float width, float height);

  struct child_status
    {
      BView *view;
      float width;
      float height;
    };

  BRect mMergin;
  std::vector<child_status> mChilds;
};

#endif
