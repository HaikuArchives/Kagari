
#include <View.h>

class KagariPaneView : public BView
{
public:
  KagariPaneView(void);
  ~KagariPaneView(void);

  void SetIconSize(icon_size);
  void SetOrientation(orientation);
  void SetIconNum(int32);

  icon_size GetIconSize(void);
  int32 GetIconNum(void);
  orientation GetOrientation(void);

private:
  void MessageReceived(BMessage *msg);
  void MouseDown(BPoint point);

  int32 mIconNum;
  icon_size mIconSize;
  orientation mOrientation;

  vector<AppRef> mAppRef;
  app_buttons mAppButtons;
}
