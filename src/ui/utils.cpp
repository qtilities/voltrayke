#include "utils.hpp"

#include <QApplication>
#include <QScreen>
#include <QWidget>

QScreen* azd::screenAt(const QPoint& pos)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    return qApp->screenAt(pos);
#else
    const QList<QScreen*> screens = QGuiApplication::screens();
    for (QScreen* screen : screens) {
        if (screen->geometry().contains(pos))
            return screen;
    }
    return nullptr;
#endif
}

void azd::centerOnScreen(QWidget* widget)
{
    if (const QScreen* screen = screenAt(QCursor::pos())) {
        QRect rct = screen->geometry();
        widget->move((rct.width() - widget->width()) / 2,
                     (rct.height() - widget->height()) / 2);
    }
}
