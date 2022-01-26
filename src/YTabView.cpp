//

#include <algorithm>
#include "YLayoutUtils.h"
#include "YTabView.h"

using std::max;

//----------------------------------------------------------------------
//
YTab::YTab(YTabView *owner, BView *tabView)
  : BTab(tabView)
  , mOwner(owner)
{
}

void
YTab::SetLabel(const char *label)
{
  BTab::SetLabel(label);
  if (mOwner->Window())
    mOwner->Draw(mOwner->Bounds());
}


//----------------------------------------------------------------------
//
YTabView::YTabView(BRect frame, const char *name,
		   button_width width,
		   uint32 resizingMode,
		   uint32 flags)

  : BTabView(frame, name,
	     width,
	     resizingMode,
	     flags)
  , mMergin(0, 0, 0, 0)
{
}


void
YTabView::SetMergin(BRect mergin)
{
  mMergin = mergin;
}


void
YTabView::AddTab(BView *target, BTab *tab)
{
  if (tab)
    delete tab;

  BTabView::AddTab(target, new YTab(this));

  child_status s;
  s.view = target;
  mChilds.push_back(s);
}


void
YTabView::ResizeChild(int32 n, float width, float height)
{
  height -= this->TabHeight();

  YLayoutUtils::ZoomToFit(mChilds[n].view,
			  BRect(mMergin.left - 1, // <- patch
				mMergin.top - 1,
				width - mMergin.right - 1,
				height - mMergin.bottom - 1),
			  mChilds[n].width,
			  mChilds[n].height);

}


void
YTabView::GetPreferredSize(float *width, float *height)
{
  int i;
  float max_width = 0, max_height = 0;

  for (i = 0; i < (int32)mChilds.size(); i++)
    {
      mChilds[i].view->GetPreferredSize(width, height);
      mChilds[i].width = *width;
      mChilds[i].height = *height;
      max_width = max(max_width, *width);
      max_height = max(max_height, *height);
    }

  *width = max_width + mMergin.left + mMergin.right;
  *height = max_height + mMergin.top + mMergin.bottom + this->TabHeight();

  *width = max(*width,
	       this->TabFrame(0).left
	       + this->TabFrame(mChilds.size() - 1).right);
}


void
YTabView::FrameResized(float width, float height)
{
  this->ResizeChild(this->Selection(), width, height);
}



void
YTabView::Select(int32 tab)
{
  BTabView::Select(tab);
  float w, h;
  mChilds[tab].view->GetPreferredSize(&w, &h);
  this->ResizeChild(tab, this->Bounds().Width(), this->Bounds().Height());
}


