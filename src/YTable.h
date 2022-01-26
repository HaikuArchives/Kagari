// table.

#ifndef Y_TABLE_H
#define Y_TABLE_H

#include <View.h>
#include <vector>
#include "YLayoutUtils.h"

class YTable : public BView
{
public:
  YTable(const char *name, int32 column, int32 raw);
  virtual ~YTable(void);

  void SetPadding(BPoint padding);
  void SetMergin(BRect mergin);

  struct child_status
  {
    child_status(void) {view = NULL;};
    BView *view;
    float width, height;
  };

  void SetChild(int32 column, int32 raw, BView *view);

  virtual void GetPreferredSize(float *width, float *height);

protected:
  virtual void FrameResized(float width, float height);
 
private:
  void CalcVariable(void);

  int32 mColumn, mRaw;
  vector <vector <child_status> > mChilds;
  vector<float> mColumnX, mRawY, mColumnWidth, mRawHeight;
  BPoint mPadding;
  BRect  mMergin;
  bool mCalcVariableNeeded;
  vector<int32> mVariableColumnIndex, mVariableRawIndex;
  float mPreferredWidth, mPreferredHeight;
};

#endif
