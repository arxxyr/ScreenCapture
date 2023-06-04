#include "MainWindow.h"
#include <QScreen>
#include <QGuiApplication>
#include <Windows.h>
#include <qtransform.h>
#include <qapplication.h>
#include <qdesktopwidget.h>
#include <qwindow.h>
#include "qdebug.h"


void MainWindow::shotScreen()
{
    auto screens = QGuiApplication::screens();
    QList<QPixmap> pixmaps;
    qreal tempSize = 0;
    int x1 = 999999, y1 = 999999, x2 = -999999, y2 = -999999;
    for (size_t i = 0; i < screens.count(); i++)
    {

        auto screen = QGuiApplication::primaryScreen();
        auto windowId = QApplication::desktop()->winId();
        auto height = GetSystemMetrics(SM_CYVIRTUALSCREEN);
        auto width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        auto x = GetSystemMetrics(SM_XVIRTUALSCREEN);
        auto y = GetSystemMetrics(SM_YVIRTUALSCREEN);

        QRect rect1{ x, y, width, height };
        auto rectPosition = rect1.topLeft();
        auto img1 = screen->grabWindow(windowId, rectPosition.x(), rectPosition.y(), rect1.width(), rect1.height());
        img1.save("img1.png");

        auto p = screens[i]->grabWindow(0);
        pixmaps.append(std::move(p));
        auto r = screens[i]->devicePixelRatio();
        if (maxRate < r) {
            maxRate = r; //�ҵ��������ű���
        }
        auto rect = screens[i]->geometry();
        rect.setSize(rect.size() * screens[i]->devicePixelRatio()); //ע�����������Ҫrate
        if (i != 0) {  //�е�ʱ���м���и��죬�������Ҫ�ϲ������
            if (rect.x() > screenRects[i - 1].right()) {
                rect.moveLeft(screenRects[i - 1].right());
            }
            if (rect.y() > screenRects[i - 1].bottom()) {
                rect.moveTop(screenRects[i - 1].bottom());
            }
            //if (rect.x()<=21) {
            //    //�������ڵײ��븱�������ʱ���������������xΪ21,��ʱ��֪��Ϊʲô
            //    rect.moveLeft(0);
            //}
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
    desktopImage->save("desktopImage.png");
}


int MainWindow::isPrimaryScreenLeft()
{
    for (size_t i = 1; i < screenRects.count(); i++)
    {
        if (screenRects[i].left() < screenRects[0].left()) {
            return false;
        }
    }
    return true;
}
int MainWindow::isPrimaryScreenRight()
{
    for (size_t i = 1; i < screenRects.count(); i++)
    {
        if (screenRects[i].right() > screenRects[0].right()) {
            return false;
        }
    }
    return true;
}
int MainWindow::isPrimaryScreenBottom()
{
    for (size_t i = 1; i < screenRects.count(); i++)
    {
        if (screenRects[i].bottom() > screenRects[0].bottom()) {
            return false;
        }
    }
    return true;
}
int MainWindow::isPrimaryScreenTop()
{
    for (size_t i = 1; i < screenRects.count(); i++)
    {
        if (screenRects[i].top() < screenRects[0].top()) {
            return false;
        }
    }
    return true;
}

void MainWindow::adjustWindowToScreen()
{
    auto screens = QGuiApplication::screens();
    //QDesktopWidget* desktopWidget = qApp->desktop();
    //QRect secondScreenGeometry = desktopWidget->screenGeometry(0);
    //this->setGeometry(secondScreenGeometry);
    //showFullScreen();
    screenRect.setSize(screenRect.size() / maxRate);
    setGeometry(screenRect);
    if (screenRects[1].left() < screenRects[0].right()) {
        auto span = screenRects[0].right() - screenRects[1].left();
        screenRect.moveLeft(296);
        setGeometry(screenRect);
    }
    return;
    auto rate = screens[0]->devicePixelRatio();
    auto topLeft = screenRect.topLeft();
    if (topLeft.x() < 0 && topLeft.y() < 0) {
        //����ʾ�����·��в�
        
    }
    else if (topLeft.x() == 0 && topLeft.y() < 0) {
        if (0 - screenRects[1].y() == screenRects[1].height()) {
            //����ʾ�����·����븱��ʾ�������
            //����ʾ�����·����
            //����ʾ�����·��Ҳ�
            auto x = screenRects[1].left();
            auto r = screens[1]->devicePixelRatio();
            move(topLeft.x() + x / (2 * r), topLeft.y());
            resize(screenRect.size()/rate);
        }
        else if(screenRects[0].bottom() <= screenRects[1].bottom())
        {            
            //����ʾ��������в�
            //����ʾ������ಢ�븱��ʾ���׶���
            move(topLeft.x(), topLeft.y()/rate);
            resize(screenRect.size()/rate);
        }
        else
        {
            //����ʾ��������·�
            auto r = screens[1]->devicePixelRatio();
            auto span1 = screenRects[0].bottom();
            auto span2 = screenRects[1].bottom();
            auto span = span1 - span2;
            auto x = screenRects[1].top() / (2 * r);
            resize(screenRect.size() / rate);
            move(topLeft.x(), topLeft.y());
        }
    }
    else if (topLeft.x() == 0 && topLeft.y() == 0) {
        //����ʾ��������Ϸ�
        //����ʾ������ಢ�븱��ʾ��������
        move(topLeft.x(), topLeft.y());
        resize(screenRect.size()/rate);
    }
    else if (topLeft.x() > 0 && topLeft.y() < 0) {
        move(topLeft.x() * rate, topLeft.y());
        resize(screenRect.size());

}
    else if(topLeft.x() >= 0 || topLeft.y() >= 0)
    {
        move(topLeft.x(), topLeft.y());
        resize(screenRect.size() / rate);
    }
    else if(topLeft.x() < 0)
    {
        move(topLeft.x() / rate, topLeft.y());
        resize(screenRect.size() / rate);
    }
    else if (topLeft.y() < 0) {
        move(topLeft.x(), topLeft.y() / rate);
        resize(screenRect.size() / rate);
    }
}


//void MainWindow::adjustWindowToScreen()
//{    
//    if (isHighRateScreenPrimary) {
//        //�����ű�������Ļ����������������͹���
//        move(screenRect.topLeft() / maxRate);
//        resize(screenRect.size() / maxRate);
//    }
//    else
//    {
//        //�����ű�������Ļ��������
//        auto topLeft = screenRect.topLeft();
//        move(topLeft.x(), topLeft.y());
//        resize(screenRect.size()*maxRate);
//        if (isPrimaryScreenTop())
//        {
//            //�������ϱ�
//            auto span = screenRects[0].height() / (maxRate * 2);
//            move(topLeft.x(), topLeft.y() + span);
//            resize(screenRect.size());
//        }
//        else if (isPrimaryScreenBottom())
//        {
//            //�������±�
//            move(topLeft.x(), topLeft.y());
//            resize(screenRect.size());
//        }
//        else if (isPrimaryScreenLeft()) {
//            //���������
//            auto span = screenRects[0].width() / (maxRate * 2);
//            move(topLeft.x()+span, topLeft.y());
//            resize(screenRect.size());
//        }
//        else if(isPrimaryScreenRight())
//        {
//            //�������ұ�
//            move(topLeft.x(), topLeft.y());
//            resize(screenRect.size());
//        }        
//    }
//}