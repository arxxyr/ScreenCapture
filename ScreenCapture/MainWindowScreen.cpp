﻿#include "MainWindow.h"
#include <QScreen>
#include <QGuiApplication>
#include <Windows.h>
#include <qtransform.h>
#include <qapplication.h>
#include <qdesktopwidget.h>
#include <qwindow.h>
#include "qdebug.h"
#include <qtimer.h>


void MainWindow::shotScreen()
{
    auto screens = QGuiApplication::screens();
    QList<QPixmap> pixmaps;
    qreal tempSize = 0;
    int x1 = 999999, y1 = 999999, x2 = -999999, y2 = -999999;
    for (size_t i = 0; i < screens.count(); i++)
    {
        auto p = screens[i]->grabWindow(0);
        pixmaps.append(std::move(p));
        auto r = screens[i]->devicePixelRatio();
        if (maxRate < r) {
            maxRate = r; 
        }
        auto rect = screens[i]->geometry();
        rect.setSize(rect.size() * screens[i]->devicePixelRatio()); 
        if (i != 0) {  
            if (rect.x() > screenRects[i - 1].right()) {
                rect.moveLeft(screenRects[i - 1].right());
            }
            if (rect.y() > screenRects[i - 1].bottom()) {
                rect.moveTop(screenRects[i - 1].bottom());
            }
        }
        if (rect.x() < x1) {
            x1 = rect.x();
        }
        if (rect.y() < y1) {
            y1 = rect.y();
        }
        if (rect.right() > x2) {
            x2 = rect.right();
        }
        if (rect.bottom() > y2) {
            y2 = rect.bottom();
        }
        screenRects.append(std::move(rect));
    }
    screenRect.adjust(x1, y1, x2, y2);
    desktopImage = new QImage(screenRect.size(), QImage::Format_ARGB32);
    desktopImage->setDevicePixelRatio(maxRate);
    QTransform transform;
    transform.translate(std::abs(screenRect.x()), std::abs(screenRect.y()));
    QPainter p(desktopImage);
    for (size_t i = 0; i < pixmaps.count(); i++)
    {
        pixmaps[i].setDevicePixelRatio(maxRate);
        auto pos = transform.map(screenRects[i].topLeft());
        p.drawPixmap(pos / maxRate, pixmaps[i]);
    }
    //desktopImage->save("desktopImage.png");
}


void MainWindow::adjustWindowToScreen()
{
    auto screens = QGuiApplication::screens();
    
    if (screens.count()>1 && screens[0]->devicePixelRatio() > screens[1]->devicePixelRatio()) {
        //主屏缩放比例大于副屏缩放比例
        //必须让它在抖动中show两次，不然有问题
        auto desktopGeometry = QApplication::desktop()->geometry();
        desktopGeometry.moveLeft(desktopGeometry.left() - 100);
        desktopGeometry.moveTop(desktopGeometry.top() - 100);
        setGeometry(desktopGeometry);
        QWidget::show();
        desktopGeometry.moveLeft(desktopGeometry.left() + 100);
        desktopGeometry.moveTop(desktopGeometry.top() + 100);
        this->setGeometry(desktopGeometry);
        QWidget::show();
    }
    else
    {
        setGeometry(screenRect);
        QWidget::show();
        
    } 
}