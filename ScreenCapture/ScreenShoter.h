
#ifndef SCREENSHOTER_H
#define SCREENSHOTER_H
#include <QImage>
#include <QObject>
#include <QList>
class ScreenShoter : public QObject
{
    Q_OBJECT
public:
    ~ScreenShoter();
    static void Dispose();
    static void Init(QObject* parent);
    static ScreenShoter* Get();
    QList<QImage> desktopImages;
    QList<QRect> windowRects;
    QList<QRect> screenRects;

private:
    explicit ScreenShoter(QObject* parent = nullptr);
    void shotScreen();
    void enumDesktopWindows();
};

#endif // SCREENSHOTER_H