#include "Arrow.h"
#include <math.h>
#include "../MainWin.h"

namespace Shape {

    Arrow::Arrow()
    {
        //path.moveTo(-1.0f, -1.0f);
        //for (size_t i = 0; i < 6; i++)
        //{
        //    path.lineTo(-1.0f, -1.0f);
        //}
    }
    Arrow::~Arrow()
    {

    }
	void Arrow::Draw(const double& x1, const double& y1, const double& x2, const double& y2)
	{
        auto context = Painter::Get()->paintCtx;
        context->begin(*Painter::Get()->prepareImage);
        context->clearAll();
        context->setStrokeCaps(BL_STROKE_CAP_ROUND);
        path.clear();
        path.moveTo(x2, y2);
        //path.lineTo(x2, y2);
        double height = 52.0;
        double width = 52.0;
        auto x = x1 - x2;
        auto y = y2 - y1;
        auto z = sqrt(x * x + y * y);
        auto sin = y / z;
        auto cos = x / z;
        // ���ױߵ��е�
        double centerX = x1 - height * cos;
        double centerY = y1 + height * sin;
        double tempA = width / 4 * sin;
        double tempB = width / 4 * cos;
        // �� ���µĶ�����ױ��е�֮���м�λ�õĵ�
        double X1 = centerX - tempA;
        double Y1 = centerY - tempB;
        path.lineTo(X1, Y1);
        // �� ���µĶ���
        double X2 = X1 - tempA;
        double Y2 = Y1 - tempB;
        path.lineTo(X2, Y2);
        // �� �ϲ����㣬Ҳ���Ǽ�ͷ�յ�
        path.lineTo(x1, y1);
        // �� ���¶���
        tempA = width / 2 * sin;
        tempB = width / 2 * cos;
        double X3 = centerX + tempA;
        double Y3 = centerY + tempB;
        path.lineTo(X3, Y3);
        // �� ���µĶ�����ױ��е�֮���м�λ�õĵ�
        double X4 = centerX + tempA / 2;
        double Y4 = centerY + tempB / 2;
        path.lineTo(X4, Y4);
        path.lineTo(x2, y2);
        if (isFill)
        {
            context->setFillStyle(color);
            context->fillPath(path);
        }
        else
        {
            context->setStrokeStyle(color);
            context->setStrokeWidth(strokeWidth-1);
            context->strokePath(path);
        }
        context->end();
        InvalidateRect(MainWin::Get()->hwnd, nullptr, false);
	}
}