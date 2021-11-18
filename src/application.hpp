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
    void onIconActivated(QSystemTrayIcon::ActivationReason);

    MenuActions     *mnuActions;
    MenuVolume      *mnuVolume;
    QSystemTrayIcon *trayIcon;
};
