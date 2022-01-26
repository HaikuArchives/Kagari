// Message Deliver
//

#ifndef Y_MESSAGE_DELIVER_H
#define Y_MESSAGE_DELIVER_H


#include <AppKit.h>
#include <set>


class YMessageDeliver 
{
public:
  void AddTarget(BMessenger target);
  void RemoveTarget(BMessenger target);
  void SendMessage(BMessage *msg);

private:
  set <BMessenger> mTarget;
};

#endif
