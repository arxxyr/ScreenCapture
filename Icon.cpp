
#include "Icon.h"

QFont Icon::font;
QMap<Icon::Name, QChar> Icon::icons;

void Icon::Init()
{
    int fontId = QFontDatabase::addApplicationFont(":/iconfont.ttf");
    QString fontName = QFontDatabase::applicationFontFamilies(fontId).at(0);
    font = QFont(fontName);
    font.setStyleStrategy(QFont::PreferAntialias); //todo
    font.setPixelSize(12);


    icons.insert(Name::rect, QChar(0xe86d));
    icons.insert(Name::arrow, QChar(0xeb0c));
    icons.insert(Name::line, QChar(0xe601));
    icons.insert(Name::mosaic, QChar(0xe82e));
    icons.insert(Name::text, QChar(0xe6ec));
    icons.insert(Name::undo, QChar(0xed85));
    icons.insert(Name::redo, QChar(0xed8a));
    icons.insert(Name::save, QChar(0xe62f));
    icons.insert(Name::copy, QChar(0xe87f));
    icons.insert(Name::ok, QChar(0xe633));
    icons.insert(Name::number, QChar(0xe776));
    icons.insert(Name::eraser, QChar(0xe6b8));
}
