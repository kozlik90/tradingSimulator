#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QSettings>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();
    bool getShowMA() const;
    bool getShowRSI() const;
    bool getShowMACD() const;
    int getMAPeriod() const;
    int getRSIPeriod() const;
    void saveSettings();
    void loadSettings();

private:
    Ui::SettingsDialog *ui;
};

#endif // SETTINGSDIALOG_H
