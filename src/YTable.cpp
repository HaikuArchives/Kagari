// Table view.


#include "YTable.h"


YTable::YTable(const char *name, int32 column, int32 raw)
  : BView(BRect(0, 0, 0, 0), name, 
	  B_FOLLOW_NONE, B_WILL_DRAW /*| B_FRAME_EVENTS*/)
  , mColumn(column)
  , mRaw(raw)
  , mChilds(column, vector<child_status>(raw))
  , mColumnX(mColumn)
  , mRawY(mRaw)
  , mColumnWidth(mColumn)
  , mRawHeight(mRaw)
  , mCalcVariableNeeded(true)
{
  mPadding.x = mPadding.y = 0;
  mMergin.left = mMergin.top = mMergin.right = mMergin.bottom = 0;
}

YTable::~YTable(void)
{
}


void
YTable::SetPadding(BPoint padding)
{
  mPadding = padding;
}


void
YTable::SetMergin(BRect mergin)
{
  mMergin = mergin;
}



void
YTable::SetChild(int32 column, int32 raw, BView *view)
{
  if (mChilds[column][raw].view)
    {
      this->RemoveChild(mChilds[column][raw].view);
      delete mChilds[column][raw].view;
    }

  if (view)
    {
      this->AddChild(view);
    }

  mChilds[column][raw].view = view;
}


void
YTable::CalcVariable(void)
{
  if (!mCalcVariableNeeded)
    return;

  mCalcVariableNeeded = false;

  vector<bool> variable_column(mColumn, false), variable_raw(mRaw, false);
  int32 i, j;

  for (i = 0; i < mColumn; i++)
    for (j = 0; j < mRaw; j++)
      {
	if ((mChilds[i][j].view->ResizingMode() & _rule_(0, 7, 0, 7))
	    == B_FOLLOW_LEFT_RIGHT)
	  variable_column[i] = true;
	if ((mChilds[i][j].view->ResizingMode() & _rule_(7, 0, 7, 0))
	    == B_FOLLOW_TOP_BOTTOM)
	  variable_raw[j] = true;
      }

  mVariableColumnIndex.clear();
  mVariableRawIndex.clear();

  for (i = 0; i < mColumn; i++)
    if (variable_column[i])
      mVariableColumnIndex.push_back(i);
  
  if (mVariableColumnIndex.size() == 0)
    for (i = 0; i < mColumn; i++)
      mVariableColumnIndex.push_back(i);

  for (i = 0; i < mRaw; i++)
    if (variable_raw[i])
      mVariableRawIndex.push_back(i);
  
  if (mVariableRawIndex.size() == 0)
    for (i = 0; i < mRaw; i++)
      mVariableRawIndex.push_back(i);
}


void
YTable::GetPreferredSize(float *width, float *height)
{
  int32 i, j;
  float sum;

  for (i = 0; i < mColumn; i++)
    mColumnWidth[i] = 0;

  for (i = 0; i < mRaw; i++)
    mRawHeight[i] = 0;
  
  for (i = 0; i < mColumn; i++)
    for (j = 0; j < mRaw; j++)
      {
	mChilds[i][j].view->GetPreferredSize(width, height);
	mChilds[i][j].width = *width;
	mChilds[i][j].height = *height;
	mColumnWidth[i] = max(*width + 1, mColumnWidth[i]);
	mRawHeight[j] = max(*height + 1, mRawHeight[j]);
      }

  for (i = 0, sum = 0; i < mColumn; i++)
    sum += mColumnWidth[i];
  *width = sum + mPadding.x * (i - 1) + mMergin.left + mMergin.right - 1;

  for (i = 0, sum = 0; i < mRaw; i++)
    sum += mRawHeight[i];
  *height = sum + mPadding.y * (i - 1) + mMergin.top + mMergin.bottom - 1;

  mPreferredWidth = *width;
  mPreferredHeight = *height;
}


void
YTable::FrameResized(float given_width, float given_height)
{
  float sum, w;
  int32 i, j, size;

  this->CalcVariable();
  
  w = (int32)(given_width - mPreferredWidth);
  size = mVariableColumnIndex.size();
  for (i = 0; i < size; i++)
    mColumnWidth[mVariableColumnIndex[i]] += 
      w * (i + 1) / size - w * i / size;

  w = (int32)(given_height - mPreferredHeight);
  size = mVariableRawIndex.size();
  for (i = 0; i < size; i++)
    mRawHeight[mVariableRawIndex[i]] += 
      w * (i + 1) / size - w * i / size;

  for (i = 0, sum = mMergin.left; i < mColumn; i++)
    {
      mColumnX[i] = sum;
      sum += mPadding.x + mColumnWidth[i];
    }

  for (i = 0, sum = mMergin.top; i < mRaw; i++)
    {
      mRawY[i] = sum;
      sum += mPadding.y + mRawHeight[i];
    }

  for (i = 0; i < mColumn; i++)
    for (j = 0; j < mRaw; j++)
      {
	YLayoutUtils::ZoomToFit(mChilds[i][j].view,
				BRect(mColumnX[i],
				      mRawY[j],
				      mColumnX[i] + mColumnWidth[i] - 1,
				      mRawY[j] + mRawHeight[j] - 1),
				mChilds[i][j].width,
				mChilds[i][j].height);
      }
}



