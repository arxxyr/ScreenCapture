#include "Eraser.h"
#include "../MainWin.h"
namespace Shape {

	Eraser::Eraser()
	{
		state = State::eraser;
	}
	Eraser::~Eraser()
	{

	}
	void Eraser::Draw(const double& x1, const double& y1, const double& x2, const double& y2)
	{
		auto context = Painter::Get()->paintCtx;
		context->begin(*Painter::Get()->canvasImage);
		context->setCompOp(BL_COMP_OP_CLEAR);
		context->setStrokeStyle(BLRgba32(0, 0, 0));
		context->setStrokeWidth(strokeWidth);
		context->setStrokeCaps(BL_STROKE_CAP_ROUND);
		if (x1 == -1) {
			for (size_t i = 0; i < points.size(); i++)
			{
				if (i + 1 >= points.size())break;
				context->strokeLine(points[i].x, points[i].y, points[i + 1].x, points[i + 1].y);
			}
		}
		else
		{
			context->strokeLine(x2, y2, x1, y1);
			if (points.size() < 1) {
				points.push_back(BLPoint(x2, y2));
			}
			points.push_back(BLPoint(x1, y1));
		}		
		context->end();
		auto win = MainWin::Get();
		win->MouseDownPos.x = (LONG)x1;
		win->MouseDownPos.y = (LONG)y1;
		InvalidateRect(win->hwnd, nullptr, false);		
	}
	bool Eraser::EndDraw()
	{
		auto painter = Painter::Get();
		auto context = painter->paintCtx;
		context->begin(*painter->canvasImage);
		context->setCompOp(BL_COMP_OP_CLEAR);
		context->setStrokeStyle(BLRgba32(0, 0, 0));
		context->setStrokeWidth(strokeWidth);
		context->setStrokeCaps(BL_STROKE_CAP_ROUND);
		for (size_t i = 0; i < points.size(); i++)
		{
			if (i + 1 >= points.size())break;
			context->strokeLine(points[i].x, points[i].y, points[i + 1].x, points[i + 1].y);
		}
		context->end();
		auto win = MainWin::Get();
		painter->isDrawing = false;
		InvalidateRect(win->hwnd, nullptr, false);
		return true;
	}
}