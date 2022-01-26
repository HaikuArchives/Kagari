// Dialog skelton
//

#ifndef Y_DIALOG_H
#define Y_DIALOG_H

#include <Window.h>
#include <Control.h>
#include <TabView.h>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <string>
#include "YTemplates.h"

class YAdaptor;

class YDialog : public BWindow
{
public:
  YDialog(istream &script,
	  BPoint point,
	  const char *name,
	  window_look look = B_FLOATING_WINDOW_LOOK,
	  window_feel feel = B_NORMAL_WINDOW_FEEL,
	  uint32 flags = 0,
	  uint32 workspace = B_CURRENT_WORKSPACE);

  YDialog(const char *script,
	  BPoint point,
	  const char *name,
	  window_look look = B_FLOATING_WINDOW_LOOK,
	  window_feel feel = B_NORMAL_WINDOW_FEEL,
	  uint32 flags = 0,
	  uint32 workspace = B_CURRENT_WORKSPACE);

  virtual ~YDialog(void);

  void SetTarget(BHandler *target);

  void SetValue(const char *name, int32 value);
  int32 Value(const char *name);
  void SetLabel(const char *name, const char *label);
  void SetLabel(const char *name, int32 value, const char *label);
  const char *Label(const char *name);
  const char *Label(const char *name, int32 value);

  void AdaptView(const char *name, BView *view);
  const BView *View(const char *name);

  void ResizeToPreferred(void);
  void GetPreferredSize(float *width, float *height);

protected:
  virtual void DispatchMessage(BMessage *msg, BHandler *handler);
  virtual void FrameResized(float width, float height);
  virtual void ValueModified(const char *name, int32 value);

private:
  enum 
  {
    Y_DIALOG_READY_TO_RUN = 'Ydrr',
    Y_DIALOG_MODIFIED = 'Ydmd',
  };

  enum
  {
    Y_VIEW    = 0x01,
    Y_ALIGN   = 0x03,
    Y_PADDING = 0x04,
    Y_MERGIN  = 0x08,
  };

  struct option_value {string str; int32 num;};
  typedef map<string, option_value> option;

  typedef pointer_map<string, BView*>    view_map;
  typedef pointer_map<string, BControl*> control_map;
  typedef map<string, map<int32, BRadioButton*> > radio_map;
  typedef pointer_map<string, BTabView*> tab_map;
  typedef pointer_map<string, YAdaptor*> adaptor_map;

  void Init(istream &script);

  BMessage* ModificationMessage(const string &name);

  void Error(const char *msg = NULL);
  void RangeError(string msg, string command, int32 value);

  istream &Read(istream &in, string &s);


  void ParseOption(istream &in,
		   char *term,
		   option &option,
		   const char *define,
		   const char *init = "",
		   int32 flags = 0);


  void ParseBlock(istream &in, char *term, vector<BView*> &childs);

  uint32 Align(option &opt);
  BPoint Padding(option &opt);
  BRect Mergin(option &opt);

  BView* Parse(istream &in, char *term);
  BView* Parse(istream &in);

  bool IsTerm(const string &s);

  control_map mControls;
  radio_map mRadioButtons;
  tab_map   mTabs;
  view_map  mViews, mViewsBuf;
  adaptor_map mAdaptors;

  BView *mTopView;

  string mCommand;

  int mLine;
};


#endif
