#pragma once

#include <QApplication>
#include <QSystemTrayIcon>

class MenuActions;
class MenuVolume;
class VolTrayke : public QApplication
{
    Q_OBJECT

public:
    VolTrayke(int &, char **);

private:
//  void onAboutToQuit();
    void onIconActivated(QSystemTrayIcon::ActivationReason);

    QSystemTrayIcon *trayIcon;
    MenuVolume      *mnuVolume;
    MenuActions     *mnuActions;
};
