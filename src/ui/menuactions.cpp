#include "src/ui/menuactions.hpp"
#include "src/ui/dialogprefs.hpp"

#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QIcon>
#include <QMessageBox>
#include <QSettings>
#include <QStandardPaths>

MenuActions::MenuActions(QWidget *parent)
    : QMenu(parent)
    , dlgPrefs    (new DialogPrefs)
    , actAutoStart(new QAction(tr("Auto&start"), this))
    , actAbout    (new QAction(QIcon::fromTheme("help-about"),         tr("&About"),       this))
    , actPrefs    (new QAction(QIcon::fromTheme("preferences-system"), tr("&Preferences"), this))
    , actQuit     (new QAction(QIcon::fromTheme("application-exit"),   tr("&Quit"),        this))
{
    actAutoStart->setCheckable(true);

    addAction(actAutoStart);
    addAction(actPrefs);
    addAction(actAbout);
    addAction(actQuit);

    connect(actAbout, &QAction::triggered, this, &MenuActions::onAboutClicked);
    connect(actQuit,  &QAction::triggered, this, &MenuActions::onAboutToQuit);
    connect(actPrefs, &QAction::triggered, this, &MenuActions::onPrefsClicked);
    connect(qApp,     &QApplication::aboutToQuit, dlgPrefs, &QObject::deleteLater);

    dlgPrefs->setWindowIcon(QIcon::fromTheme("audio-volume-medium"));
//  dlgPrefs->setAttribute(Qt::WA_DeleteOnClose);

    loadSettings();
}
void MenuActions::onAboutClicked()
{
    QFile f(":/about.html");
    if (!f.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug() << tr("Error loading about file") << Qt::endl;
        return;
    }
    QTextStream in(&f);
    QString text = in.readAll();
    f.close();

    QMessageBox mb;
    mb.setIconPixmap(QIcon::fromTheme("preferences-system").pixmap(22));
    mb.about(this, tr("About"), text);
}
void MenuActions::onPrefsClicked()
{
    if (dlgPrefs->isHidden())
        dlgPrefs->show();
}
void MenuActions::loadSettings()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                       QApplication::organizationName(),
                       QApplication::applicationDisplayName());

    settings.beginGroup("Main");
    actAutoStart->setChecked(settings.value("AutoStart", false).toBool());
    settings.endGroup();
}
void MenuActions::saveSettings()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                       QApplication::organizationName(),
                       QApplication::applicationDisplayName());

    settings.beginGroup("Main");
    settings.setValue("AutoStart", actAutoStart->isChecked());
    settings.endGroup();
}
void MenuActions::onAboutToQuit()
{
    actAutoStart->isChecked() ? avz::createAutostartFile()
                              : avz::deleteAutostartFile();
    saveSettings();
}

namespace avz {

void createAutostartFile()
{
    QDir    configDir(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation));
    QString appName  = QApplication::applicationName();
    QString filePath = configDir.filePath("autostart/" + appName + ".desktop");
    QFile   file(filePath);

    if (file.exists() || !file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    out << "[Desktop Entry]\n";
    out << "Name=" + QApplication::applicationDisplayName() + "\n";
    out << "Type=Application\n";
    out << "Exec=" + appName + "\n";
    out << "Terminal=false\n";
}
void deleteAutostartFile()
{
    QDir configDir(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation));
    QString filePath(configDir.filePath("autostart/" + QApplication::applicationName() + ".desktop"));
    QFile file(filePath);

    if (!file.exists())
        return;

    file.remove();
}
} // namespace avz
