#ifndef MAINSETTINGS_H
#define MAINSETTINGS_H

#include <QDialog>
#include <QSettings>
namespace Ui {
class MainSettings;
}

class MainSettings : public QDialog
{
    Q_OBJECT

public:
    explicit MainSettings(QWidget *parent = nullptr);
    ~MainSettings();
    bool getShowCloseMessage() const;
    void setCloseMessageNotif(bool ch);
    void saveSettings();
    void loadSettings();

private:
    Ui::MainSettings *ui;
};

#endif // MAINSETTINGS_H
