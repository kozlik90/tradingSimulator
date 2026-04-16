#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    // ui->checkBoxMA->setChecked(true);
    // ui->checkBoxRSI->setChecked(true);
    // ui->checkBoxMACD->setChecked(true);
    // ui->spinBoxMA->setValue(20);
    // ui->spinBoxRSI->setValue(14);
    loadSettings();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

bool SettingsDialog::getShowMA() const
{
    return ui->checkBoxMA->isChecked();
}

bool SettingsDialog::getShowRSI() const
{
    return ui->checkBoxRSI->isChecked();
}

bool SettingsDialog::getShowMACD() const
{
    return ui->checkBoxMACD->isChecked();
}

int SettingsDialog::getMAPeriod() const
{
    return ui->spinBoxMA->text().toInt();
}

int SettingsDialog::getRSIPeriod() const
{
    return ui->spinBoxRSI->text().toInt();
}

void SettingsDialog::saveSettings()
{
    QSettings settings("kozlik90 Inc.", "user");
    qDebug() << "Saving settings:";
    qDebug() << "MA period:" << ui->spinBoxMA->value();
    qDebug() << "RSI period:" << ui->spinBoxRSI->value();
    settings.setValue("chart/showMA", ui->checkBoxMA->isChecked());
    settings.setValue("chart/showRSI", ui->checkBoxRSI->isChecked());
    settings.setValue("chart/showMACD", ui->checkBoxMACD->isChecked());
    settings.setValue("chart/maPeriod", ui->spinBoxMA->value());
    settings.setValue("chart/rsiPeriod", ui->spinBoxRSI->value());
    settings.sync();

}

void SettingsDialog::loadSettings()
{
    QSettings settings("kozlik90 Inc.", "user");
    qDebug() << "Loading settings:";
    qDebug() << "MA period from settings:" << settings.value("chart/maPeriod", 20).toInt();
    ui->checkBoxMA->setChecked(settings.value("chart/showMA", true).toBool());
    ui->checkBoxRSI->setChecked(settings.value("chart/showRSI", true).toBool());
    ui->checkBoxMACD->setChecked(settings.value("chart/showMACD", true).toBool());
    ui->spinBoxMA->setValue(settings.value("chart/maPeriod", 20).toInt());
    ui->spinBoxRSI->setValue(settings.value("chart/rsiPeriod", 14).toInt());

}
