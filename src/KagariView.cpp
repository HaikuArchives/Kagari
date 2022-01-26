//----------------------------------------------------------------------
// Kagari
//  Application View.

#include <InterfaceKit.h>
#include "KagariMenu.h"
#include "KagariInfo.h"
#include "KagariView.h"
#include "KagariDefs.h"


//======================================================================
// application view.

//----------------------------------------------------------------------
KagariView::KagariView(BMessenger parent, BRect bounds, const char *name, BBitmap *icon)
  : BView(bounds, name, B_FOLLOW_ALL_SIDES, B_WILL_DRAW)
  , mParent(parent)
  , mIcon(icon)
{
  this->SetViewColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR), B_DARKEN_1_TINT));
  this->SetDrawingMode(B_OP_OVER);
}

KagariView::KagariView(BMessage *archive)
  : BView(archive)
{
  BMessage bitmapArchive;

  archive->FindMessage("bitmap", &bitmapArchive);
  mIcon = cast_as(instantiate_object(&bitmapArchive), BBitmap);
  archive->FindMessenger("parent", &mParent);
}

KagariView*
KagariView::Instantiate(BMessage *archive)
{
  if (validate_instantiation(archive, "KagariView"))
    return new KagariView(archive);

  return NULL;
}

status_t
KagariView::Archive(BMessage *archive, bool deep) const
{
  if (BView::Archive(archive, deep) != B_OK)
    return B_ERROR;

  archive->AddString("add_on", cAppSignature);
  archive->AddMessenger("parent", mParent);

  if (deep)
    {
      BMessage bitmapArchive;
      if (mIcon->Archive(&bitmapArchive, deep) == B_OK)
	archive->AddMessage("bitmap", &bitmapArchive);
      else
	return B_ERROR;
    }

  return B_OK;
}


//----------------------------------------------------------------------
KagariView::~KagariView(void)
{
  delete mIcon;
}


//----------------------------------------------------------------------
void
KagariView::MouseDown(BPoint point)
{
  BMessage *msg = this->Looper()->CurrentMessage();
  msg->ReplacePoint("where", this->ConvertToScreen(msg->FindPoint("where")));
  mParent.SendMessage(msg);

  return;
}


//----------------------------------------------------------------------
void
KagariView::Draw(BRect updateRect)
{
  this->DrawBitmap(mIcon);
}
