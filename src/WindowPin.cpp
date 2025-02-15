﻿#include "WindowPin.h"
#include <windowsx.h>
#include "include/core/SkPath.h"
#include "include/core/SkRect.h"
#include "include/core/SkSurface.h"
#include "include/core/SkCanvas.h"
#include "include/utils/SkShadowUtils.h"
#include "include/core/SkPoint3.h"
#include "include/core/SkRRect.h"
#include "include/core/SkStream.h"
#include "include/core/SkImageFilter.h"
#include "include/encode/SkPngEncoder.h"
#include "App.h"
#include "Cursor.h"
#include "CutMask.h"
#include "ToolMain.h"
#include "ToolSub.h"
#include "Recorder.h"
#include "ColorBlender.h"


WindowPin::WindowPin()
{
    imgRect = CutMask::GetCutRect();
    imgW = imgRect.width();
    imgH = imgRect.height();
    auto windowMain = App::GetWin();
    auto iRect = SkIRect::MakeLTRB(imgRect.fLeft, imgRect.fTop, imgRect.fRight, imgRect.fBottom);
    img = windowMain->surfaceBase->makeImageSnapshot(iRect);
    initSize();
    initWindow();
}

WindowPin::~WindowPin()
{
    ColorBlender::Reset();
}

void WindowPin::Save(const std::string& filePath)
{
    Recorder::Get()->FinishPaint();
    auto img = surfaceBase->makeImageSnapshot(SkIRect::MakeXYWH(shadowSize, shadowSize, surfaceFront->width(), surfaceFront->height()));    
    SkPixmap pixmap;
    img->peekPixels(&pixmap);
    SkPngEncoder::Options option;
    SkFILEWStream stream(filePath.data());
    SkPngEncoder::Encode(&stream, pixmap, option);
    stream.flush();
    App::Quit(6);
}

void WindowPin::SaveToClipboard()
{
    Recorder::Get()->FinishPaint();
    HDC ScreenDC = GetDC(NULL);
    HDC hMemDC = CreateCompatibleDC(ScreenDC);
    auto w{ surfaceFront->width() }, h{ surfaceFront->height() };
    HBITMAP hBitmap = CreateCompatibleBitmap(ScreenDC, w, h);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
    StretchBlt(hMemDC, 0, 0, w, h, ScreenDC, x+shadowSize, y+shadowSize, w, h, SRCCOPY);
    hBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);
    DeleteDC(hMemDC);
    DeleteObject(hOldBitmap);
    if (!OpenClipboard(hwnd)) {
        MessageBox(NULL, L"Failed to open clipboard when save to clipboard.", L"Error", MB_OK | MB_ICONERROR);
        return;
    }
    EmptyClipboard();
    SetClipboardData(CF_BITMAP, hBitmap);
    CloseClipboard();
    ReleaseDC(NULL, ScreenDC);
    App::Quit(7);
}

void WindowPin::initCanvas()
{    
    SkImageInfo imgInfo = SkImageInfo::MakeN32Premul(w, h);
    long long rowBytes = w * 4;
    long long dataSize = rowBytes * h;
    pixSrcData.resize(dataSize);
    pixSrc = new SkPixmap(imgInfo, &pixSrcData.front(), rowBytes);
    auto canvas = SkCanvas::MakeRasterDirect(imgInfo, &pixSrcData.front(), rowBytes);
    canvas->clear(SK_ColorTRANSPARENT); 
    auto srcRect = SkRect::MakeWH(imgW, imgH);
    App::Log(std::format("---{},{},{},{}\n", imgRect.fLeft, imgRect.fTop, imgRect.width(), imgRect.height()));
    canvas->drawImageRect(img, srcRect,imgRect, SkSamplingOptions(SkFilterMode::kNearest,
        SkMipmapMode::kNearest),nullptr,SkCanvas::SrcRectConstraint::kFast_SrcRectConstraint);

    SkPaint paint;
    SkPath path;
    path.addRRect(SkRRect::MakeRectXY(SkRect::MakeXYWH(shadowSize - 2, shadowSize - 2, imgRect.width() + 4, imgRect.height() + 4), 6, 6));
    SkPoint3 zPlaneParams = SkPoint3::Make(0, 0, 20);// 定义阴影与 z 平面的关系    
    SkPoint3 lightPos = SkPoint3::Make(0, 0, 0);// 定义光源的位置和半径
    SkShadowUtils::DrawShadow(canvas.get(), path, zPlaneParams, lightPos, 20.f, SkColorSetARGB(60, 0, 0, 0), SK_ColorTRANSPARENT, 0);


    surfaceBase = SkSurfaces::Raster(SkImageInfo::MakeN32Premul(w, h));
    pixBase = new SkPixmap();
    surfaceBase->peekPixels(pixBase);
    SkImageInfo info = SkImageInfo::MakeN32Premul(imgRect.width(), imgRect.height());
    surfaceBack = SkSurfaces::Raster(info);
    surfaceFront = SkSurfaces::Raster(info);
}

void WindowPin::initSize()
{
    auto mainWin = App::GetWin();
    x = mainWin->x + imgRect.fLeft - shadowSize;
    y = mainWin->y + imgRect.fTop - shadowSize;
    w = imgRect.width() + shadowSize * 2;
    h = imgRect.height() + shadowSize * 2;
    auto tm = ToolMain::Get();
    tm->Reset();
    
    auto tempWidth = tm->ToolRect.width() + shadowSize * 2;
    if (w < tempWidth) {
        this->w = tempWidth;
    }
    h += tm->ToolRect.height() * 2 + tm->MarginTop * 2;
    imgRect.setXYWH(shadowSize, shadowSize, imgRect.width(), imgRect.height());
}

void WindowPin::showMenu()
{
    HMENU hMenu = CreatePopupMenu();
    AppendMenu(hMenu, state == State::start ? MF_UNCHECKED : MF_CHECKED, 1001, L"Tool");
    AppendMenu(hMenu, MF_STRING, 1002, L"Exit(Esc)");
    POINT point;
    GetCursorPos(&point);
    TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, 0, hwnd, NULL);
}

void WindowPin::paintCanvas()
{
    surfaceBase->writePixels(*pixSrc, 0, 0);
    //auto canvas = surfaceBase->getCanvas();
    //auto img = surfaceBack->makeImageSnapshot();
    //canvas->drawImage(img, shadowSize,shadowSize);
    //img = surfaceFront->makeImageSnapshot();
    //canvas->drawImage(img, shadowSize, shadowSize);
    //ToolMain::Get()->OnPaint(canvas);
    //ToolSub::Get()->OnPaint(canvas);
}

LRESULT WindowPin::wndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
    case WM_LBUTTONDOWN:
    {
        IsMouseDown = true;
        auto x = GET_X_LPARAM(lparam);
        auto y = GET_Y_LPARAM(lparam);
        return onMouseDown(x, y);
    }
    case WM_LBUTTONUP:
    {
        IsMouseDown = false;
        auto x = GET_X_LPARAM(lparam);
        auto y = GET_Y_LPARAM(lparam);
        return onMouseUp(x, y);
    }
    case WM_MOUSEMOVE:
    {
        auto x = GET_X_LPARAM(lparam);
        auto y = GET_Y_LPARAM(lparam);
        if (IsMouseDown) {
            return onMouseDrag(x, y);
        }
        else {
            return onMouseMove(x, y);
        }
    }
    case WM_RBUTTONDOWN:
    {
        auto x = GET_X_LPARAM(lparam);
        auto y = GET_Y_LPARAM(lparam);
        onMouseDownRight(x, y);
        return true;
    }
    case WM_KEYDOWN:
    {
        onKeyDown(wparam);
        return false;
    }
    case WM_CHAR: {
        return onChar(wparam);
    }
    case WM_MOUSEWHEEL: {
        int delta = GET_WHEEL_DELTA_WPARAM(wparam);
        onMouseWheel(delta);
        return false;
    }
    case WM_SIZE: {
        if (hoverIndex == -1) {
            return false;
        }
        delete pixSrc;
        delete pixBase;
        initCanvas();
        Refresh();
        break;
    }
    case WM_COMMAND: {
        switch (LOWORD(wparam))
        {
        case 1001:
        {
            if (state == State::start) {
                state = State::tool;
                auto tm = ToolMain::Get();
                tm->SetPosition(8.0f, h - tm->ToolRect.height() * 2 - tm->MarginTop * 2);
                tm->IndexSelected = -1;
                tm->IndexHovered = -1;
            }
            else
            {
                state = State::start;
            }
            Refresh();
            break;
        }
        case 1002:
            App::Quit(1);
            break;
        }
        break;
    }
    case WM_TIMER: {
        return onTimeout(wparam);
    }
    default:
        break;
    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

bool WindowPin::onMouseDown(const int& x, const int& y)
{
    if (state == State::start) {
        GetCursorPos(&startPos);
        SetCapture(hwnd);
    }
    else {
        if (ToolMain::Get()->OnMouseDown(x, y)) {
            return true;
        }
        if (ToolSub::Get()->OnMouseDown(x, y)) {
            return true;
        }
        Recorder::Get()->OnMouseDown(x - shadowSize, y - shadowSize);
    }
	return false;
}

bool WindowPin::onMouseDownRight(const int& x, const int& y)
{
    if (state == State::start) {
        showMenu();
    }
    else {
        auto flag = Recorder::Get()->OnMouseDownRight(x - shadowSize, y - shadowSize);
        if (!flag) {
            showMenu();
        }
    }    
	return false;
}

bool WindowPin::onMouseUp(const int& x, const int& y)
{
    if (state == State::start) {
        ReleaseCapture();
    }
    else {
        Recorder::Get()->OnMouseUp(x-shadowSize, y - shadowSize);
    }
	return false;
}

bool WindowPin::onMouseMove(const int& x, const int& y)
{
    if (state == State::start) {
        if (x >= 0 && x <= shadowSize && y>=0 && y <= shadowSize) {
            hoverIndex = 0;
            Cursor::LeftTopRightBottom();
        }
        else if (x>shadowSize && x<w-shadowSize && y>=0 &&y<=shadowSize) {
            hoverIndex = 1;
            Cursor::TopBottom();
        }
        else {
            Cursor::All();
        }
        return false;
    }
    auto tm = ToolMain::Get()->OnMouseMove(x, y);
    auto ts = ToolSub::Get()->OnMouseMove(x, y);
    if (tm || ts) {
        return false;
    }
    Recorder::Get()->OnMouseMove(x - shadowSize, y - shadowSize);
    return false;
}

bool WindowPin::onMouseDrag(const int& x1, const int& y1)
{
    if (state == State::start) {
        if (hoverIndex == 0) {
            POINT point;
            GetCursorPos(&point);
            int dx = point.x - startPos.x;
            int dy = point.y - startPos.y;
            x += dx;
            y += dy;
            w -= dx;
            h -= dy;
            App::Log(std::format("win{},{},{},{}\n", x, y, w, h));
            App::Log(std::format("-{},{},{},{}\n", imgRect.fLeft, imgRect.fTop, imgRect.width(), imgRect.height()));
            imgW = imgRect.width();
            imgH = imgRect.height();
            imgRect.setLTRB(shadowSize, shadowSize, imgRect.fRight-dx, imgRect.fBottom-dy);
            App::Log(std::format("--{},{},{},{}\n", imgRect.fLeft, imgRect.fTop, imgRect.width(), imgRect.height()));
            SetWindowPos(hwnd, nullptr, x, y, w, h, SWP_NOZORDER | SWP_NOOWNERZORDER);
            startPos = point;
        }
        else if (hoverIndex == 1) {

        }
        else{
            POINT point;
            GetCursorPos(&point);
            int dx = point.x - startPos.x;
            int dy = point.y - startPos.y;
            x += dx;
            y += dy;
            SetWindowPos(hwnd, nullptr, x, y, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOSIZE);
            startPos = point;
        }
        
    }
    else {
        Recorder::Get()->OnMouseDrag(x1 - shadowSize, y1 - shadowSize);
    }
	return false;
}

bool WindowPin::onChar(const unsigned int& val)
{
    Recorder::Get()->OnChar(val);
	return false;
}

bool WindowPin::onKeyDown(const unsigned int& val)
{
    bool flag = Recorder::Get()->OnKeyDown(val);
    if (flag) {
        return true;
    }
    if (val == VK_ESCAPE)
    {
        App::Quit(3);
    }
    else if (GetKeyState(VK_CONTROL) < 0) {
        if (val == 83) {//Ctrl+S
            App::SaveFile();
        }
        else if (val == 67) {//Ctrl+C
            SaveToClipboard();
        }
        else if (val == 89) { //Ctrl+Y
            Recorder::Get()->Redo();
        }
        else if (val == 90) { //Ctrl+Z
            Recorder::Get()->Undo();
        }
    }
	return true;
}

bool WindowPin::onMouseWheel(const int& delta)
{
    Recorder::Get()->OnMouseWheel(delta);
	return false;
}
bool WindowPin::onTimeout(const unsigned int& id)
{
    Recorder::Get()->OnTimeout(id);
    return false;
}