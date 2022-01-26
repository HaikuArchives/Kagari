// Object Properties.
//

#include <AppKit.h>
#include <InterfaceKit.h>
#include <SupportKit.h>
#include <StorageKit.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <strstream>
#include <string>
#include <cstring>
#include "YProperty.h"


YProperty::YProperty(const char *path, const y_property_arg *args)
  : mArg(args)
{
  char dir[B_PATH_NAME_LENGTH + 1];

  strncpy(mPath, path, B_PATH_NAME_LENGTH);
  strncpy(dir, path, B_PATH_NAME_LENGTH);
  *strrchr(dir, '/') = '\0';
  create_directory(dir, 755);
  this->InitArgs();
  this->Read();
}


YProperty::~YProperty(void)
{
  this->Write();
}


void
YProperty::Lock(void)
{
  mLock.Lock();
}


void
YProperty::Unlock(void)
{
  mLock.Unlock();
}


void
YProperty::Read(void)
{
  string buf, cmd;
  const y_property_arg *arg;

  ifstream in(mPath);

  mLock.Lock();

  while (getline(in, buf, '\n'))
    {
      istrstream lin(buf.c_str());
      if ((lin >> cmd) && (arg = this->FindArg(cmd.c_str())))
	this->DecodeArg(lin, arg);
    }

  mLock.Unlock();
}

void
YProperty::Write(void)
{
  const y_property_arg *arg;

  ofstream out(mPath, ios::out | ios::trunc);
  if (!out)
    return;

  mLock.Lock();

  for (arg = mArg; arg->name; arg++)
    {
      out << arg->name << '\t';
      this->EncodeArg(out, arg);
      out << '\n';
    }

  mLock.Unlock();
}


const y_property_arg *
YProperty::FindArg(const char *name)
{
  for (const y_property_arg* arg = mArg; arg->name; arg++)
    if (strcmp(name, arg->name) == 0)
      return arg;

  return NULL;
}


status_t
YProperty::DecodeArg(istream &in, const y_property_arg *arg)
{
  if (mArchive.HasData(arg->name, arg->type))
    mArchive.RemoveData(arg->name);

  switch (arg->type)
    {
    default:
      {
	cerr << "YProperty: unsupported type " << arg->type << "\n";
	return B_ERROR;
      }

    case B_INT32_TYPE:
      {
	int32 value;
	in >> value;
	mArchive.AddInt32(arg->name, value);
	break;
      }

    case B_POINT_TYPE:
      {
	float x, y;
	in >> x >> y;
	mArchive.AddPoint(arg->name, BPoint(x, y));
	break;
      }

    case B_RECT_TYPE:
      {
	float left, top, right, bottom;
	in >> left >> top >> left >> right >> bottom;
	mArchive.AddRect(arg->name, BRect(left, top, right, bottom));
	break;
      }

    case B_STRING_TYPE:
      {
	string value, buf;
	const char *c;

	if (mIsInitiarizing)
	  {
	    getline(in, value, '\0');
	    mArchive.AddString(arg->name, value.c_str());
	    break;
	  }

	in >> buf;
	for (c = buf.c_str(); *c; c++)
	  if (*c != '\\')
	    value += *c;
	  else
	    {
	      c++;
	      if (!*c)
		goto string_failure;
	      else if (*c == 'n')
		value += '\n';
	      else if (*c == '\\')
		value += *c;
	      else if (isdigit(*c))
		{
		  int c0, c1, c2;
		  c0 = *c++;
		  c1 = *c++;
		  c2 = *c;
		  if (!isdigit(c1) || !isdigit(c2))
		    goto string_failure;
		  value += char(((c0 - '0') << 6) |
				((c1 - '0') << 3) |
				((c2 - '0')));
		}
	      else 
		goto string_failure;
	    }

	mArchive.AddString(arg->name, value.c_str());
	break;

      string_failure:
	cerr << "YProperty: parsing string'" << arg->name << "' failed.\n";
	mArchive.AddString(arg->name, value.c_str());
	break;
      }
    }

  return B_OK;
}  


status_t
YProperty::EncodeArg(ostream &out, const y_property_arg *arg)
{
  switch (arg->type)
    {
    default:
      {
	cerr << "YProperty: unsupported type " << arg->type << "\n";
	return B_ERROR;
      }

    case B_INT32_TYPE:
      {
	int32 value;
	value = mArchive.FindInt32(arg->name);
	out << value;
	break;
      }

    case B_POINT_TYPE:
      {
	BPoint value;
	value = mArchive.FindPoint(arg->name);
	out << value.x << ' ' << value.y;
	break;
      }
	
    case B_RECT_TYPE:
      {
	BRect value;
	value = mArchive.FindRect(arg->name);
	out << value.left << ' ' 
	    << value.top << ' '
	    << value.right << ' '
	    << value.bottom;
	break;
      }

    case B_STRING_TYPE:
      {
	const char *value;

	for (value = mArchive.FindString(arg->name); *value; value++)
	  {
	    if (*value == '\n')
	      out << "\\n";
	    else if (*value == '\\')
	      out << "\\\\";
	    else if (*value < 0x20 || *value == ' ')
	      out << '\\'
		  << oct << setw(3) << setfill('0') 
		  << int(*value) 
		  << dec << setw(0) << setfill(' ');
	    else
	      out << *value;
	  }

	break;
      }
    }

  return B_OK;
}


void
YProperty::InitArgs(void)
{
  mIsInitiarizing = true;
  for (const y_property_arg *arg = mArg; arg->name; arg++)
    {
      istrstream buf(const_cast<char*>(arg->default_value));
      this->DecodeArg(buf, arg);
    }
  mIsInitiarizing = false;
}


void
YProperty::VerifyArg(const char *name, type_code type)
{
  const y_property_arg *arg;
  arg = this->FindArg(name);
  if (!arg || arg->type != type)
    {
      cerr << "YProperty: invalid arg, " << name << ' ' << type << ".\n" ;
      exit (1);
    }
	
}


void
YProperty::StartWatching(const char *name,
			 BMessenger target)
{
  if (FindArg(name) == NULL)
    {
      cerr << "YProperty : no such argument name" << name << ".\n";
      exit (1);
    }

  watcher_map::iterator i;
  pair<watcher_map::iterator, watcher_map::iterator> range;

  range = mWatcher.equal_range(name);

  for (i = range.first; i != range.second; i++)
    if ((*i).second == target)
      {
	cerr << "YProperty: target already registerd.\n";
	exit (1);
      }

  watcher_map::value_type value(name, target);
  mWatcher.insert(value);
}


void
YProperty::StartWatching(const char *name,
			 const BHandler *handler, 
			 const BLooper *looper)
{
  BMessenger target(handler, looper);
  this->StartWatching(name, target);
}


void
YProperty::StopWatching(const char *name, BMessenger target)
{
  if (FindArg(name) == NULL)
    {
      cerr << "YProperty : no such argument name" << name << ".\n";
      exit (1);
    }

  watcher_map::iterator i;
  pair<watcher_map::iterator, watcher_map::iterator> range;

  range = mWatcher.equal_range(name);

  for (i = range.first; i != range.second; i++)
    if ((*i).second == target)
      {
	mWatcher.erase(i);
	return;
      }

  cerr << "YProperty: target is not registerd.\n";
  exit (1);
}


void
YProperty::StopWatching(const char *name,
			const BHandler *handler, 
			const BLooper *looper)
{
  BMessenger target(handler, looper);
  this->StopWatching(name, target);
}


void
YProperty::TellModification(const char *name, const BHandler *modifier)
{
  watcher_map::iterator i;
  pair<watcher_map::iterator, watcher_map::iterator> range;
  BMessenger target(modifier);

  range = mWatcher.equal_range(name);

  if (range.first == range.second)
    return;

  BMessage msg(Y_PROPERTY_MODIFIED);
  msg.AddString("name", name);

  for (i = range.first; i != range.second; i++)
    if ((*i).second != modifier)
      (*i).second.SendMessage(&msg);
}


void
YProperty::SetInt32(const char *name, int32 value, const BHandler *modifier)
{
  this->Lock();

  this->VerifyArg(name, B_INT32_TYPE);
  mArchive.ReplaceInt32(name, value);
  this->TellModification(name, modifier);

  this->Unlock();
}


int32
YProperty::GetInt32(const char *name)
{
  int32 value;

  this->Lock();

  this->VerifyArg(name, B_INT32_TYPE);
  value = mArchive.FindInt32(name);

  this->Unlock();

  return value;
}


void
YProperty::SetPoint(const char *name, BPoint value, const BHandler *modifier)
{
  this->Lock();

  this->VerifyArg(name, B_POINT_TYPE);
  mArchive.ReplacePoint(name, value);
  this->TellModification(name, modifier);

  this->Unlock();
}


BPoint
YProperty::GetPoint(const char *name)
{
  BPoint value;

  this->Lock();

  this->VerifyArg(name, B_POINT_TYPE);
  value = mArchive.FindPoint(name);

  this->Unlock();

  return value;
}


void
YProperty::SetRect(const char *name, BRect value, const BHandler *modifier)
{
  this->Lock();

  this->VerifyArg(name, B_RECT_TYPE);
  mArchive.ReplaceRect(name, value);
  this->TellModification(name, modifier);

  this->Unlock();
}


BRect
YProperty::GetRect(const char *name)
{
  BRect value;

  this->Lock();

  this->VerifyArg(name, B_RECT_TYPE);
  value = mArchive.FindRect(name);

  this->Unlock();
  
  return value;
}


