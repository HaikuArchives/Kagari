// Kagari Launcher Button
//

#ifndef Y_BITMAP_BUTTON_H
#define Y_BITMAP_BUTTON_H

#include <Control.h>

class YBitmapButton : public BControl
{
public:
  YBitmapButton(BRect frame,
		const char *name,
		const char *label,
		BMessage *msg,
		const BBitmap *bitmap,
		uint32 resizingMode,
		uint32 flags);

  virtual ~YBitmapButton(void);

protected:
  virtual void Draw(BRect updateRect);
  virtual void SetLabel(const char *label);
  virtual void MouseDown(BPoint point) ;
  virtual void MouseMoved(BPoint point, uint32 transit, const BMessage *message);
  virtual void MouseUp(BPoint point);
  virtual void AttachedToWindow(void);

private:
  const BBitmap *mBitmap;
  bool mIsHolding;
};

#endif


