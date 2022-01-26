// Dialog Skelton.
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
#include "YTable.h"
#include "YFrame.h"
#include "YDialog.h"
#include "YTabView.h"
#include "YSlider.h"
#include "YAdaptor.h"

BView*
YDialog::Parse(istream &in,
	       char *term)
{
  string buf;
  option opt;
  BView *view(NULL);

  this->Read(in, buf);
  if (this->IsTerm(buf))
    {
      *term = buf[0];
      return NULL;
    }

  mCommand = buf;

  cerr << "parsing " << buf << ".\n";

  //----------------------------------------------------------------------
  // adaptor
  if (buf == "adaptor")
    {
      YAdaptor *adaptor;
      option opt;

      this->ParseOption(in, term, opt,
			"*name", 
			"",
			Y_VIEW | Y_ALIGN);

      view = adaptor =
	new YAdaptor();

      adaptor->SetResizingMode(this->Align(opt));
      adaptor->SetName(opt["tablabel"].str.c_str());

      mAdaptors[opt["name"].str] = adaptor;
    }
      

  //------------------------------------------------------------
  // radio button

  else if (buf == "radio")
    {
      BRadioButton *button;
      option opt;

      this->ParseOption(in, term, opt,
			"*label *name *value",
			"",
			Y_VIEW | Y_ALIGN);

      view = button = 
	new BRadioButton(BRect(0, 0, 0, 0),
			 opt["tablabel"].str.c_str(),
			 opt["label"].str.c_str(),
			 this->ModificationMessage(opt["name"].str),
			 B_FOLLOW_NONE,
			 B_WILL_DRAW);

      button->SetResizingMode(this->Align(opt));

      mControls[opt["name"].str] = button;
	mRadioButtons[opt["name"].str][opt["value"].num] = button;
    }


  //----------------------------------------------------------------------
  else if (buf == "checkbox")
    {
      BCheckBox *cbox;

      this->ParseOption(in, term, opt,
			"*label *name *value",
			"value 0",
			Y_VIEW | Y_ALIGN);

      view = cbox = 
	new BCheckBox(BRect(0, 0, 0, 0),
		      opt["tablabel"].str.c_str(),
		      opt["label"].str.c_str(),
		      this->ModificationMessage(opt["name"].str),
		      B_FOLLOW_NONE,
		      B_WILL_DRAW | B_NAVIGABLE);

      cbox->SetResizingMode(this->Align(opt));
      cbox->SetValue(opt["value"].num);

      mControls[opt["name"].str] = cbox;
    }

  //------------------------------------------------------------
  // string

  else if (buf == "string")
    {
      BStringView *str;

      this->ParseOption(in, term, opt,
			"*label name align",
			"",
			Y_VIEW | Y_ALIGN);

      view = str =
	new BStringView(BRect(0, 0, 0, 9),
			opt["tablabel"].str.c_str(),
			opt["label"].str.c_str(),
			B_FOLLOW_NONE,
			B_WILL_DRAW);

      str->SetResizingMode(this->Align(opt));
    }
  
  
  //------------------------------------------------------------
  // slider

  else if (buf == "slider")
    {
      BSlider *slider;

      this->ParseOption(in, term, opt,
			"*label *name *value *min *max width height async",
			"width 100 height 20 value 0 async 0",
			Y_VIEW | Y_ALIGN);

      view = slider =
	new YSlider(BRect(0, 0, opt["width"].num, opt["height"].num), 
		    opt["tablabel"].str.c_str(),
		    opt["label"].str.c_str(),
		    this->ModificationMessage(opt["name"].str),
		    opt["min"].num, opt["max"].num,
		    B_BLOCK_THUMB,
		    B_FOLLOW_NONE,
		    B_WILL_DRAW | B_NAVIGABLE_JUMP | B_NAVIGABLE);

      slider->SetResizingMode(this->Align(opt));

      if (opt.count("value"))
	slider->SetValue(opt["value"].num);

      if (opt["async"].num)
	slider->SetModificationMessage(this->ModificationMessage(opt["name"].str));
      mControls[opt["name"].str] = slider;
    }

  //----------------------------------------------------------------------
  // button

  else if (buf == "button")
    {
      BButton *button;

      this->ParseOption(in, term, opt, 
			"*label *name",
			"",
			Y_VIEW | Y_ALIGN);

      view = button = 
	new BButton(BRect(0, 0, 0, 0), 
		    opt["tablabel"].str.c_str(),
		    opt["label"].str.c_str(),
		    this->ModificationMessage(opt["name"].str),
		    B_FOLLOW_NONE,
		    B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE);

      button->SetResizingMode(this->Align(opt));

      mControls[opt["name"].str] = button;
    }

  //------------------------------------------------------------
  // box
  else if (buf == "box")
    {
      YFrame *box;
      vector<BView*> childs;

      this->ParseOption(in, term, opt,
			"label style",
			"style plain",
			Y_VIEW | Y_ALIGN | Y_MERGIN);

      this->ParseBlock(in, term, childs);

      if (childs.size() != 1)
	this->Error("is able to have only 1 child");

      border_style border = B_NO_BORDER;
      
      if (opt["style"].str == "plain")
	border = B_PLAIN_BORDER;
      else if (opt["style"].str == "fancy")
	border = B_FANCY_BORDER;
      else if (opt["style"].str == "no_border")
	border = B_NO_BORDER;
      else
	this->Error("no such style");

      view = box =
	new YFrame(opt["tablabel"].str.c_str(), childs[0], border);

      if (opt.count("label"))
	box->SetLabel(opt["label"].str.c_str());

      box->SetResizingMode(this->Align(opt));
      box->SetMergin(this->Mergin(opt));
    }

  //------------------------------------------------------------
  // table
  
  else if (buf == "table")
    {
      YTable *table;
      vector<BView*> childs;
      uint32 i, num;

      this->ParseOption(in, term, opt,
			"*width *height",
			"width 1 height 1",
			Y_VIEW | Y_ALIGN | Y_PADDING | Y_MERGIN);

      this->ParseBlock(in, term, childs);

      num = opt["width"].num * opt["height"].num;
      if (childs.size() != num)
	this->Error("invalid argument num");

      view = table = 
	new YTable(opt["tablabel"].str.c_str(),
		   opt["width"].num, opt["height"].num);

      table->SetResizingMode(this->Align(opt));
      table->SetPadding(this->Padding(opt));
      table->SetMergin(this->Mergin(opt));

      for (i = 0; i < num; i++)
	table->SetChild
	  (i % opt["width"].num, i / opt["width"].num, childs[i]);
    }

  //------------------------------------------------------------
  // list
  else if (buf == "list")
    {
      YTable *table;
      vector<BView*> childs;
      uint32 i, width, height, num;

      this->ParseOption(in, term, opt,
			"*orientation",
			"orientation vertical",
			Y_VIEW | Y_ALIGN | Y_PADDING | Y_MERGIN);


      this->ParseBlock(in, term, childs);

      num = childs.size();

      width = height = 1;
      if (opt["orientation"].str == "horizontal")
	width = num;
      else if (opt["orientation"].str == "vertical")
	height = num;
      else
	this->Error("no such orientation");

      view = table = 
	new YTable(opt["tablabel"].str.c_str(),
		   width, height);

      table->SetResizingMode(this->Align(opt));
      table->SetPadding(this->Padding(opt));
      table->SetMergin(this->Mergin(opt));

      for (i = 0; i < num; i++)
	table->SetChild(i % width, i / width, childs[i]);
    }

  //------------------------------------------------------------
  // tab

  else if (buf == "tab")
    {
      YTabView *tab;
      vector<BView*> childs;
      uint32 i;

      this->ParseOption(in, term, opt,
			"name",
			"",
			Y_VIEW | Y_ALIGN | Y_MERGIN);

      this->ParseBlock(in, term, childs);

      view = tab = new YTabView(BRect(0, 0, 0, 0),
				opt["tablabel"].str.c_str(),
				B_WIDTH_FROM_LABEL,
				B_FOLLOW_NONE,
				B_WILL_DRAW | B_NAVIGABLE_JUMP | /* B_FRAME_EVENTS | */ B_NAVIGABLE);

      tab->SetResizingMode(this->Align(opt));
      tab->SetMergin(this->Mergin(opt));

      for (i = 0; i < childs.size(); i++)
	tab->AddTab(childs[i]);

      if (opt.count("name"))
	mTabs[opt["name"].str] = tab;
    }

  //----------------------------------------------------------------------
  // view definition
  else if (mViewsBuf.count(buf))
    {
      uint32 align;

      this->ParseOption(in, term, opt, "", "", Y_VIEW | Y_ALIGN);

      view = mViewsBuf[buf];
      mViews[buf] = view;
      mViewsBuf.erase(mViewsBuf.find(buf));

      if (opt.count("tablabel"))
	view->SetName(opt["tablabel"].str.c_str());

      align = this->Align(opt);
      if (align != B_FOLLOW_NONE)
	view->SetResizingMode(align);
    }
  
  else
    {
      this->Error("no such command");
    }

  view->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
  return view;
}



