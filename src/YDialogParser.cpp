// Dialog Parser.
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
#include <strstream>
#include "YLayoutUtils.h"
#include "YDialog.h"

using std::cerr;


//----------------------------------------------------------------------
static const char *sTerms = ",;{}";



BMessage*
YDialog::ModificationMessage(const string &name)
{
  BMessage *msg = new BMessage(Y_DIALOG_MODIFIED);
  msg->AddString("name", name.c_str());
  return msg;
}


bool
YDialog::IsTerm(const string &s)
{
  return (s.size() == 1 && strchr(sTerms, s[0]));
}



istream&
YDialog::Read(istream &in, string &s)
{
  enum {NONE, QUOT} state = NONE;
  int ch;

  s = "";

  if (in.eof())
    {
      in.setstate(std::ios::failbit);
      return in;
    }

  // skip white spaces.
  while (in)
    {
      ch = in.get();

      if (ch == EOF)
	{
	  in.setstate(std::ios::failbit);
	  return in;
	}

      if (ch == '\n')
	mLine++;

      if (!isspace(ch))
	{
	  in.putback(ch);
	  break;
	}
    }

  // read in string.
  while (in)
    {
      ch = in.get();
      if (ch == '\n')
	mLine++;

      switch (state)
	{
	case NONE:
	  {
	    if (ch == EOF)
	      {
		in.clear(std::ios::eofbit);
		return in;
	      }
	    if (isspace(ch))
	      return in;
	    else if (strchr(sTerms, ch))
	      {
		if (s == "")
		  s = ch;
		else
		  in.putback(ch);
		return in;
	      }
	    else if (ch == '\'')
	      {
		state = QUOT;
		break;
	      }

	    s += ch;

	    break;
	  }

	case QUOT:
	  {
	    if (ch == EOF)
	      this->Error("unterminated string.");
	    else if (ch == '\'')
	      {
		state = NONE;
		break;
	      }

	    s += ch;

	    break;
	  }
	}
    }

  return in;
}



uint32
YDialog::Align(option &opt)
{
  int32 align = B_FOLLOW_NONE;

  if (opt.count("align"))
    {
      if (opt["align"].str == "fill")
	return B_FOLLOW_ALL_SIDES;
      else if (opt["align"].str == "center")
	return B_FOLLOW_H_CENTER | B_FOLLOW_V_CENTER;
      else
	{
	  std::ostrstream s;
	  s << "invalid align value '" << opt["align"].str << "'";
	  this->Error(s.str());
	}
    }

  if (!opt.count("halign"))
    align |= B_FOLLOW_NONE;
  else if (opt["halign"].str == "left")
    align |= B_FOLLOW_LEFT;
  else if (opt["halign"].str == "center")
    align |= B_FOLLOW_H_CENTER;
  else if (opt["halign"].str == "right")
    align |= B_FOLLOW_RIGHT;
  else if (opt["halign"].str == "fill")
    align |= B_FOLLOW_LEFT_RIGHT;
  else
    {
      std::ostrstream s;
      s << "invalid halign value '" << opt["halign"].str << "'";
      this->Error(s.str());
    }

  if (!opt.count("valign"))
    align |= B_FOLLOW_NONE;
  else if (opt["valign"].str == "top")
    align |= B_FOLLOW_TOP;
  else if (opt["valign"].str == "center")
    align |= B_FOLLOW_V_CENTER;
  else if (opt["valign"].str == "bottom")
    align |= B_FOLLOW_BOTTOM;
  else if (opt["valign"].str == "fill")
    align |= B_FOLLOW_TOP_BOTTOM;
  else
    {
      std::ostrstream s;
      s << "invalid valign value '" << opt["valign"].str << "'";
      this->Error(s.str());
    }

  return align;
}


BPoint
YDialog::Padding(option &opt)
{
  BPoint padding(0, 0);

  if (opt.count("padding"))
    padding.x = padding.y = opt["padding"].num;
  if (opt.count("wpadding"))
    padding.x = opt["wpadding"].num;
  if (opt.count("hpadding"))
    padding.y = opt["hpadding"].num;

  return padding;
}


BRect
YDialog::Mergin(option &opt)
{
  BRect mergin(0, 0, 0, 0);

  if (opt.count("mergin"))
    mergin.left = mergin.top = mergin.right = mergin.bottom = opt["mergin"].num;
  if (opt.count("wmergin"))
    mergin.left = mergin.right = opt["wmergin"].num;
  if (opt.count("hmergin"))
    mergin.top = mergin.bottom = opt["hmergin"].num;
  if (opt.count("leftmergin"))
    mergin.left = opt["leftmergin"].num;
  if (opt.count("rightmergin"))
    mergin.right = opt["rightmergin"].num;
  if (opt.count("topmergin"))
    mergin.top = opt["topmergin"].num;
  if (opt.count("bottommergin"))
    mergin.bottom = opt["bottommergin"].num;

  return mergin;
}




void
YDialog::ParseOption(istream &in,
		     char *term,
		     option &opt,
		     const char *option_rule,
		     const char *option_init,
		     int32 flags)
{
  std::set<string> command, command_needed;
  string buf0, buf1;

  // clear option.
  opt.clear();

  // set default command.
  if (flags & Y_VIEW)
    {
      command.insert("tablabel");
    }

  if (flags & Y_ALIGN)
    {
      command.insert("align");
      command.insert("valign");
      command.insert("halign");
    }

  if (flags & Y_PADDING)
    {
      command.insert("padding");
      command.insert("wpadding");
      command.insert("hpadding");
    }

  if (flags & Y_MERGIN)
    {
      command.insert("mergin");
      command.insert("wmergin");
      command.insert("hmergin");
      command.insert("leftmergin");
      command.insert("topmergin");
      command.insert("rightmergin");
      command.insert("bottommergin");
    }

  // read in option rule.
  std::istrstream in0(option_rule);

  while (this->Read(in0, buf0))
    {
      if (buf0[0] == '*')
	{
	  command.insert(buf0.c_str() + 1);
	  command_needed.insert(buf0.c_str() + 1);
	}
      else
	command.insert(buf0);
    }

  // read in default value;
  std::istrstream in1(option_init);

  while (this->Read(this->Read(in1, buf0), buf1))
    {
      opt[buf0].str = buf1;
      opt[buf0].num = atoi(buf1.c_str());
    }

  // read in default option.
  while (in)
    {
      this->Read(in, buf0);

      if (!in)
	this->Error();

      if (strchr(sTerms, buf0[0]))
	{
	  *term = buf0[0];
	  break;
	}

      if (buf0[0] != '-')
	{
	  std::ostrstream s;
	  s << "option required '" << buf0 << "'";
	  this->Error(s.str());
	}

      this->Read(in, buf1);

      if (!in)
	this->Error();

      opt[buf0.c_str() + 1].str = buf1;
      opt[buf0.c_str() + 1].num = atoi(buf1.c_str());
    }

  // check option rule.
;
  for (option::iterator i = opt.begin(); i != opt.end(); i++)
    if (!command.count((*i).first))
      {
	cerr << "no such option '" << (*i).first << "'.\n";
	this->Error();
      }

  for (std::set<string>::iterator i = command_needed.begin();
       i != command_needed.end(); i++)
    {
      if (opt.count(*i) == 0)
	{
	  cerr << "option '" << *i << "' is needed.\n";
	  this->Error();
	}
    }
}


void
YDialog::ParseBlock(istream &in,
		    char *term,
		    vector<BView*> &childs)
{
  BView *view;
  string prev_command(mCommand), buf;

  childs.clear();

  if (*term != '{')
    this->Error();

  cerr << "begin block.\n";

  do
    {
      view = this->Parse(in, term);
      if (!view)
	break;
      childs.push_back(view);
    }
  while (*term == ',');

  mCommand = prev_command;

  if (*term != '}')
    this->Error();

  cerr << "end block.\n";

  this->Read(in, buf);
  if (!this->IsTerm(buf))
    this->Error("not terminated");

  *term = buf[0];
}


BView*
YDialog::Parse(istream &in)
{
  BView *view;
  char term;
  string name, op;

  while (this->Read(in, name))
    {
      if (!this->Read(in, op) || op != "=")
	{
	  this->Error("operator = required.");
	}

      if (mViewsBuf.count(name))
	this->Error("multiple difinition.");

      view = this->Parse(in, &term);
      if (!view || term != ';')
	this->Error("not terminated with ';'.");

      mViewsBuf[name] = view;
    }

  if (!mViewsBuf.count("top"))
    this->Error("top view not defined.");

  view = mViewsBuf["top"];
  mViewsBuf.erase(mViewsBuf.find("top"));

  for (view_map::iterator i = mViewsBuf.begin(); i != mViewsBuf.end(); i++)
    delete (*i).second;

  return view;
}
