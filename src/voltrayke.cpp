#include "src/voltrayke.hpp"
#include "src/ui/menuvolume.hpp"
#include "src/ui/menuactions.hpp"

#include <QCursor>
#include <QTranslator>

VolTrayke::VolTrayke(int &argc, char *argv[])
    : QApplication(argc, argv)
    , trayIcon(new QSystemTrayIcon(QIcon::fromTheme("audio-volume-medium"), this))
    , mnuVolume(new MenuVolume)
    , mnuActions(new MenuActions)
{
    setQuitOnLastWindowClosed(false);

    setOrganizationName("AZDrums");
    setOrganizationDomain("azdrums.org");
    setApplicationName("voltrayke");
    setApplicationDisplayName("VolTrayke");

    trayIcon->setContextMenu(mnuActions);
    trayIcon->setVisible(true);
    trayIcon->show();

    connect(trayIcon, &QSystemTrayIcon::activated, this, &VolTrayke::onIconActivated);
//  connect(this,     &VolTrayke::aboutToQuit,     this, &VolTrayke::onAboutToQuit);
    connect(this,     &QApplication::aboutToQuit,  mnuVolume,  &QObject::deleteLater);
    connect(this,     &QApplication::aboutToQuit,  mnuActions, &QObject::deleteLater);
    connect(this,     &QApplication::aboutToQuit,  trayIcon,   &QObject::deleteLater);
}
#if 0
void VolTrayke::onAboutToQuit()
{
    delete mnuVolume;
}
#endif
void VolTrayke::onIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick)
    {
        QPoint pos = QCursor::pos();
        pos.setX(pos.x() - mnuVolume->width() / 2);
        mnuVolume->popup(pos);
    }
}
int main(int argc, char *argv[])
{
    VolTrayke app(argc, argv);

    QTranslator translator;
    if (translator.load(QLocale(), QLatin1String("voltrayke"),
                        QLatin1String("_"),
                        QLatin1String(":/translations")))
        app.installTranslator(&translator);

    return app.exec();
}
