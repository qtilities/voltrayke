#pragma once

#include <QMenu>

namespace avz {
    void createAutostartFile();
    void deleteAutostartFile();
} // namespace avz

class DialogPrefs;
class MenuActions : public QMenu
{
    Q_OBJECT

public:
    MenuActions(QWidget *parent = nullptr);

private:
    void onAboutClicked();
    void onAboutToQuit();
    void onPrefsClicked();

    void loadSettings();
    void saveSettings();

    DialogPrefs     *dlgPrefs;
    QAction         *actAutoStart,
                    *actAbout,
                    *actPrefs,
                    *actQuit;
};
