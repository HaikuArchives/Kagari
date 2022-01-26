// Message Deliver
//

#include "YMessageDeliver.h"

void
YMessageDeliver::AddTarget(BMessenger target)
{
  if (mTarget.find(target) != mTarget.end())
    return;

  mTarget.insert(target);
}


void
YMessageDeliver::RemoveTarget(BMessenger target)
{
  if (mTarget.find(target) == mTarget.end())
    return;

  mTarget.erase(target);
}

void
YMessageDeliver::SendMessage(BMessage *msg)
{
  set <BMessenger>::iterator i;

  for (i = mTarget.begin(); i != mTarget.end(); i++)
    (*i).SendMessage(msg);
}
