#pragma once
#include "include/core/SkSurface.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkRect.h"
#include "include/core/SkPoint.h"
#include "include/core/SkPath.h"
#include "ShapeBase.h"
#include <vector>

class ShapeRect : public ShapeBase
{
public:
    ShapeRect(const int& x, const int& y);
    ~ShapeRect();
    virtual bool OnMouseDown(const int& x, const int& y) override;
    virtual bool OnMouseUp(const int& x, const int& y) override;
    virtual bool OnMouseMove(const int& x, const int& y) override;
    bool OnMoseDrag(const int& x, const int& y) override;
    virtual void Paint(SkCanvas* canvas) override;
protected:
    void initParams();
    void setDragger();
    SkRect rect;
    bool stroke{ true };
    int strokeWidth{ 4 };
    SkColor color{ SkColorSetARGB(255, 207, 19, 34) };
private:
};