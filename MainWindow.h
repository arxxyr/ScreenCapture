
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainterPath>
#include <QPainter>
#include <QImage>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow

{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
protected:

private slots:

private:
    void btnMainToolSelected();
    void initMask();
    void initToolMain();
    void initToolRect();
    void initToolEraser();
    void initCanvasImg();
    void switchTool(const QString& toolName);
    void showToolMain(int x, int y);
    Ui::MainWindow* ui;
    QString style = R"(#%1{background:#f6f6f6;}
#%1 QPushButton{color:#414141;font-size:16px;border:none;}
#%1 QPushButton:hover{background:#e5e5e5;}
#%1 QPushButton:checked{background:#1677ff;color:#fff;})";
    QPointF mousePressPoint;
    bool isMouseDown = false;
    QPainterPath maskPath;
    qreal maskBorderWidth = 2.0;
    QPainter* painter1;
    QPainter* painter2;
    QImage* canvasImg1;
    QImage* canvasImg2;
    QString state = "Start";
    QVector<QPainterPath> paths;
};

#endif // MAINWINDOW_H
