#include "MainSettings.h"
#include "ui_MainSettings.h"

MainSettings::MainSettings(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MainSettings)
{
    ui->setupUi(this);
    loadSettings();
}

MainSettings::~MainSettings()
{
    delete ui;
}

bool MainSettings::getShowCloseMessage() const
{
    return ui->cb_NotifClose->isChecked();
}

void MainSettings::setCloseMessageNotif(bool ch)
{
    ui->cb_NotifClose->setChecked(ch);
}

void MainSettings::saveSettings()
{
    QSettings settings("kozlik90 Inc.", "user");
    settings.setValue("mainSettings/notifClose", ui->cb_NotifClose->isChecked());
    settings.sync();
}

void MainSettings::loadSettings()
{
    QSettings settings("kozlik90 Inc.", "user");
    ui->cb_NotifClose->setChecked(settings.value("mainSettings/notifClose", true).toBool());
}
