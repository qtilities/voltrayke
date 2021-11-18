#pragma once

class QScreen;
class QPoint;
class QWidget;

namespace azd {

QScreen* screenAt(const QPoint&);
void centerOnScreen(QWidget*);

} // namespace azd
