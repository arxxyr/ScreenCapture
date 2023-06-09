
#include "MainWindow.h"
#include "./ui_MainWindow.h"
#include <Windows.h>
#include <QScreen>
#include <QPainter>
#include <QFile>
#include <QQmlEngine>
#include "ScreenImageProvider.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    initDesktopImage();

    mainView = new QQuickWidget(this);
    mainView->setParent(this, Qt::Widget);
    mainView->engine()->addImageProvider(QLatin1String("ScreenImage"), new ScreenImageProvider(desktopImage));
    ui->centralwidget->layout()->addWidget(mainView);
    mainView->setResizeMode(QQuickWidget::SizeRootObjectToView);
    const QUrl url(u"qrc:/MainView.qml"_qs);
    mainView->setSource(url);


    int x = (int)winX;
    int y = (int)winY;
    this->setGeometry(x,y,desktopImage->width(),desktopImage->height());
    //https://github.com/Myles-Trevino/Qt-Quick-Borderless-Window/blob/master/Source/QML%20Window.cpp
}

MainWindow::~MainWindow()
{
    delete desktopImage;
    if(mainView) delete mainView;
    delete ui;
}
void MainWindow::Close()
{
    QApplication::quit();
}

bool MainWindow::nativeEvent(const QByteArray& eventType, void *message, qintptr *result)
{
    MSG *qt_message{reinterpret_cast<MSG *>(message)};
    if(qt_message->message == WM_GETMINMAXINFO)
    {
        MINMAXINFO* mminfo = (PMINMAXINFO)qt_message->lParam;
        mminfo->ptMaxPosition.x = GetSystemMetrics(SM_XVIRTUALSCREEN);
        mminfo->ptMaxPosition.y = GetSystemMetrics(SM_YVIRTUALSCREEN);
        mminfo->ptMaxSize.x = GetSystemMetrics(SM_CXVIRTUALSCREEN)+1000;
        mminfo->ptMaxSize.y = GetSystemMetrics(SM_CYVIRTUALSCREEN)+800;
        return false;
    }
    return false;
}
void MainWindow::initDesktopImage()
{
    auto screens = QGuiApplication::screens();
    QList<QRectF> rects;
    QList<QPixmap> images;
    for(auto s:screens){
        auto ratio = s->devicePixelRatio();
        auto g = s->geometry();
        auto x = g.x();
        auto y = g.y();
        auto w = g.width()*ratio;
        auto h = g.height()*ratio;
        if(x < this->winX) this->winX = x;
        if(y < this->winY) this->winY = y;
        QRectF r(x,y,w,h);
        rects.append(std::move(r));
        QPixmap pixmap = s->grabWindow();
        images.append(std::move(pixmap));
    }
    for(auto& r:rects){
        r.moveTopLeft({r.x()-this->winX,r.y()-this->winY});
        if(winBottom < r.bottom()) this->winBottom = r.bottom();
        if(winRight < r.right()) this->winRight = r.right();
    }
    desktopImage = new QPixmap(this->winRight,this->winBottom);
    QPainter painter(desktopImage);
    for (int i = 0; i < rects.count(); ++i) {
        painter.drawImage(rects.at(i),images.at(i).toImage());
    }
//    QFile file("desktopImage.png");
//    file.open(QIODevice::WriteOnly);
//    desktopImage->save(&file, "PNG");
}

