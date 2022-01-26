// Dialog Control
//

#include <AppKit.h>
#include <InterfaceKit.h>
#include <strstream>
#include <algorithm>
#include <vector>
#include <set>
#include <cstdlib>
#include <cctype>
#include <iostream>
#include "YDialog.h"
#include "YAdaptor.h"


YDialog::YDialog(istream &script,
		 BPoint point,
		 const char *name,
		 window_look look,
		 window_feel feel,
		 uint32 flags,
		 uint32 workspace)
  : BWindow(BRect(point.x, point.y, 0, 0), name, look, feel,
	    flags | B_ASYNCHRONOUS_CONTROLS,
	    workspace)
{
  this->Init(script);
}


YDialog::YDialog(const char *script,
		 BPoint point,
		 const char *name,
		 window_look look,
		 window_feel feel,
		 uint32 flags,
		 uint32 workspace)
  : BWindow(BRect(point.x, point.y, 0, 0), name, look, feel,
	    flags | B_ASYNCHRONOUS_CONTROLS,
	    workspace)
{
  std::istrstream in(script);
  this->Init(in);
}


void
YDialog::Init(istream &script)
{
  mLine = 0;
  mTopView = this->Parse(script);
  this->AddChild(mTopView);
  this->ResizeToPreferred();
  this->PostMessage(Y_DIALOG_READY_TO_RUN);
}


YDialog::~YDialog(void)
{
}

void
YDialog::DispatchMessage(BMessage *msg, BHandler *handler)
{
  switch (msg->what)
    {
    default:
      {
	BWindow::DispatchMessage(msg, handler);
	break;
      }

    case Y_DIALOG_READY_TO_RUN:
      {
	this->SetTarget(this);
	break;
      }

    case Y_DIALOG_MODIFIED:
      {
	const char *name = msg->FindString("name");
	int32 value = this->Value(name);
	this->ValueModified(name, value);
	break;
      }
    }
}


void
YDialog::ValueModified(const char *name, int32 value)
{
}


//----------------------------------------------------------------------
void
YDialog::GetPreferredSize(float *width, float *height)
{
  mTopView->GetPreferredSize(width, height);
}


void
YDialog::ResizeToPreferred(void)
{
  float width, height;
  mTopView->GetPreferredSize(&width, &height);
  this->ResizeTo(width, height);
}


void
YDialog::FrameResized(float width, float height)
{
  float w, h;
  mTopView->GetPreferredSize(&w, &h);
  mTopView->ResizeTo(width, height);
  mTopView->FrameResized(width, height);
}

void
YDialog::SetTarget(BHandler *target)
{
  control_map::iterator i;

  for (i = mControls.begin(); i != mControls.end(); i++)
    (*i).second->SetTarget(target);
}


void
YDialog::SetValue(const char *name, int32 value)
{
  if (mRadioButtons.count(name) &&
      mRadioButtons[name].count(value))
    {
      mRadioButtons[name][value]->SetValue(1);
      return;
    }

  if (mControls.count(name))
    {
      mControls[name]->SetValue(value);
      return;
    }

  this->Error("no such name of controls.\n");
}


void
YDialog::SetLabel(const char *name, const char *label)
{
  if (!mControls.count(name))
    this->Error("no such name of controls");

  mControls[name]->SetLabel(label);
}


void
YDialog::SetLabel(const char *name, int32 value, const char *label)
{
  if (mRadioButtons.count(name))
    if (mRadioButtons[name].count(value))
      {
	mRadioButtons[name][value]->SetLabel(label);
	return;
      }
    else
      this->RangeError("control range over", name, value);

  if (mTabs.count(name))
    if (value < mTabs[name]->CountTabs())
      {
	mTabs[name]->TabAt(value)->SetLabel(label);
	return;
      }
    else
      this->RangeError("control range over", name, value);

  this->Error("no such name of controls");
}


const char *
YDialog::Label(const char *name)
{
  if (!mControls.count(name))
    this->Error("no such name of controls");

  return mControls[name]->Label();
}


const char *
YDialog::Label(const char *name, int32 value)
{
  if (!(mRadioButtons.count(name) && mRadioButtons[name].count(value)))
    this->Error("no such name of controls");

  return mRadioButtons[name][value]->Label();
}


int32
YDialog::Value(const char *name)
{
  if (mRadioButtons.count(name))
    {
      radio_map::mapped_type::iterator i;

      for (i = mRadioButtons[name].begin();
	   i != mRadioButtons[name].end();
	   i++)
	if ((*i).second->Value())
	  return (*i).first;

      this->Error("radio button not active.\n");
    }

  if (mControls.count(name))
    return mControls[name]->Value();

  this->Error("no such name of controls.\n");
  return 0;
}


void
YDialog::AdaptView(const char *name, BView *view)
{
  if (view)
    view->SetFlags(view->Flags() & ~ B_FRAME_EVENTS);
  mAdaptors[name]->AdaptView(view);
}


//----------------------------------------------------------------------
void
YDialog::Error(const char *msg)
{
  std::cerr << "YDialog: " << mCommand << ": "
       << msg << " in line " << mLine << ".\n";
  exit (1);
}


void
YDialog::RangeError(string msg, string command, int32 value)
{
  std::ostrstream buf;

  buf << msg << ": " << value  <<" of " << command << ".";
  this->Error(buf.str());
}

