#include "MainWin.h"

void MainWin::setToolBoxMain()
{
	toolBoxMain.x0 = cutBox.x1 - toolBoxWidth;
	auto heightSpan = toolBoxSpan * 3 + toolBoxHeight * 2;
	if (int(painter->h - cutBox.y1) > heightSpan)
	{
		//��Ļ�ײ������㹻�Ŀռ�
		//�����߶ȣ�Ϊ��Ļ�ױ�Ҳ��һ���϶ 
		toolBoxMain.y0 = cutBox.y1 + toolBoxSpan;
	}
	else if(int(cutBox.y0) > heightSpan)
	{
		//��Ļ���������㹻�Ŀռ�
		if (selectedToolIndex == -1) 
		{
			//��δȷ��state�������������Ž�ͼ��
			toolBoxMain.y0 = cutBox.y0 - toolBoxSpan - toolBoxHeight;			
		}
		else
		{
			//�Ѿ�ȷ����state��ҪΪ�ӹ�������������
			toolBoxMain.y0 = cutBox.y0 - (double)toolBoxSpan * 2 - (double)toolBoxHeight * 2;
		}		
	}
	else
	{
		//�����ײ���û���㹻�Ŀռ�
		if (selectedToolIndex == -1) 
		{
			//��δȷ��state�������������Ž�ͼ���ײ��Ϸ�
			toolBoxMain.y0 = cutBox.y1 - toolBoxSpan - toolBoxHeight;
		}
		else
		{
			//��δȷ��state�������������Ž�ͼ���ײ��Ϸ�����Ϊ�ӹ����������ռ�
			toolBoxMain.y0 = cutBox.y1 - (double)toolBoxSpan * 2 - (double)toolBoxHeight * 2;
		}
	}
	if (toolBoxMain.x0 < 0) {
		//�����������������Ļ��࣬��ô�ù��������ͼ����������
		//todo ������������Ҳ���Ļ�����Ե�����������Ҳ���Ļ�����Ե
		toolBoxMain.x0 = cutBox.x0;
	}



	toolBoxMain.y1 = toolBoxMain.y0 + toolBoxHeight;
	toolBoxMain.x1 = toolBoxMain.x0 + toolBoxWidth;

}

void MainWin::drawToolMain()
{
	if (state == State::start) return;
	setToolBoxMain();
	painter->paintCtx->setFillStyle(BLRgba32(255, 255, 255, 255));
	painter->paintCtx->fillBox(toolBoxMain);
	BLPoint point;
	point.x = toolBoxMain.x0 + iconLeftMargin;
	point.y = toolBoxMain.y0 + iconTopMargin;
	for (size_t i = 0; i < 9; i++)
	{
		drawBtnCheckable(point, (Icon::Name)i, selectedToolIndex == i, mouseEnterMainToolIndex == i);
		point.x += toolBtnWidth;
	}
	drawSplitter(point);
	point.x += toolBtnSpanWidth;
	if (IsLeftBtnDown) {
		drawBtnUndoRedo(point, Icon::Name::undo, (int)Icon::Name::undo == mouseEnterMainToolIndex, false);
		point.x += toolBtnWidth;
		drawBtnUndoRedo(point, Icon::Name::redo, (int)Icon::Name::redo == mouseEnterMainToolIndex, false);
		point.x += toolBtnWidth;
	}
	else
	{
		auto enables = History::UndoRedoEnable();
		drawBtnUndoRedo(point, Icon::Name::undo, (int)Icon::Name::undo == mouseEnterMainToolIndex, enables.first);
		point.x += toolBtnWidth;
		drawBtnUndoRedo(point, Icon::Name::redo, (int)Icon::Name::redo == mouseEnterMainToolIndex, enables.second);
		point.x += toolBtnWidth;
	}
	drawSplitter(point);
	point.x += toolBtnSpanWidth;
	drawBtn(point, Icon::Name::save, (int)Icon::Name::save == mouseEnterMainToolIndex);
	point.x += toolBtnWidth;
	drawBtn(point, Icon::Name::copy, (int)Icon::Name::copy == mouseEnterMainToolIndex);
	point.x += toolBtnWidth;
	drawBtn(point, Icon::Name::close, (int)Icon::Name::close == mouseEnterMainToolIndex);
	point.x += toolBtnWidth;
	if (selectedToolIndex != -1) {
		drawSubTool();
	}
}

