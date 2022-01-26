// Object Properties.
//

#ifndef Y_PROPERTY_H
#define Y_PROPERTY_H

#include <Message.h>
#include <Locker.h>
#include <iostream>
#include <string>
#include <map>

using namespace std;

const static uint32 Y_PROPERTY_MODIFIED = 'Ypmd';
// 'name', B_STRING_TYPE, property name


struct y_property_arg
{
  const char *name;
  type_code type;
  const char *default_value;
};


class YProperty
{
public:
  YProperty(const char *path, const y_property_arg *args);
  ~YProperty(void);

  void Read(void);
  void Write(void);

  void Lock(void);
  void Unlock(void);

  void StartWatching(const char *name,
		     const BHandler *handler,
		     const BLooper *looper=NULL);
  void StartWatching(const char *name,
		     BMessenger target);

  void StopWatching(const char *name,
		    const BHandler *handler,
		    const BLooper *looper=NULL);
  void StopWatching(const char *name,
		    BMessenger target);

  void SetInt32(const char *name, int32 value, const BHandler *modifier = NULL);
  int32 GetInt32(const char *name);

  void SetPoint(const char *name, BPoint point, const BHandler *modifier = NULL);
  BPoint GetPoint(const char *name);

  void SetRect(const char *name, BRect rect, const BHandler *modifier = NULL);
  BRect GetRect(const char *name);

  void SetString(const char *name, const char *string, const BHandler *modifier = NULL);
  const char *GetString(const char *name);

private:
  typedef multimap<string, BMessenger> watcher_map;

  const y_property_arg *FindArg(const char *name);
  void VerifyArg(const char *name, type_code type);
  void TellModification(const char *name, const BHandler *modifier);
  status_t DecodeArg(istream &in, const y_property_arg *arg);
  status_t EncodeArg(ostream &out, const y_property_arg *arg);
  void InitArgs(void);

  BMessage mArchive;
  const y_property_arg *mArg;
  char mPath[B_PATH_NAME_LENGTH + 1];
  BLocker mLock;
  watcher_map mWatcher;
  bool mIsInitiarizing;
};


#endif
