#include "MainWin.h"


BLRect MainWin::getToolRect()
{
	int btnCount = 14;
	int span = 8;//�����������ͼ����ĸ߶�
	int toolBtnWidth = 58;
	int toolBtnSpanWidth = 6;
	int toolBtnSpanCount = 2;
	BLRect result;
	result.w = btnCount * toolBtnWidth + toolBtnSpanWidth * toolBtnSpanCount;
	result.h = 56;
	result.x = cutBox.x1 - result.w;
	if (h - cutBox.y1 > span * 2 + result.h) //�����߶ȣ�Ϊ��Ļ�ױ�Ҳ��һ���϶ 
	{
		result.y = cutBox.y1 + span;
	}
	else if(cutBox.y0 > span * 2 + result.h)
	{
		result.y = cutBox.y0 - span - result.h;
	}
	else
	{
		result.y = cutBox.y1 - span - result.h;
	}
	return result;
}

void MainWin::drawTool()
{
	if (state == State::Start) return;
	auto rect = getToolRect();
	paintCtx->setFillStyle(BLRgba32(240, 240, 240, 255));
	paintCtx->fillRect(rect);
}