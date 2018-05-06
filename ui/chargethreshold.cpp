/*
 * Copyright (c) 2017 Ognjen Galić
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdlib.h>

#include <QDebug>
#include <QMessageBox>

#include "chargethreshold.h"
#include "ui_chargethreshold.h"
#include "core/storage.h"

ChargeThreshold::ChargeThreshold(QWidget *parent) : QDialog(parent)
{
    ui = new Ui::ChargeThreshold;
    ui->setupUi(this);

    setWindowIcon(QIcon::fromTheme("battery"));

    connect(ui->custom, SIGNAL(toggled(bool)), this, SLOT(customClicked(bool)));
    connect(ui->ok_button, SIGNAL(clicked(bool)), this, SLOT(saveSettings()));
    connect(ui->cancel_button, SIGNAL(clicked(bool)), this, SLOT(close()));
    connect(ui->battery_chooser, SIGNAL(activated(int)), this, SLOT(restoreSettings()));

    if (Battery::isPrimaryAvailable())
        ui->battery_chooser->addItem(PRIMARY);
    if (Battery::isSecondaryAvailable())
       ui->battery_chooser->addItem(SECONDARY);

    connect(ui->start, SIGNAL(valueChanged(int)), this, SLOT(forceConstraintStart()));
    connect(ui->stop, SIGNAL(valueChanged(int)), this, SLOT(forceConstraintStop()));

    restoreSettings();

}

ChargeThreshold::~ChargeThreshold()
{
    delete ui;
    settings->sync();
    delete settings;
}

void ChargeThreshold::customClicked(bool state)
{
    ui->start->setEnabled(state);
    ui->stop->setEnabled(state);
}

void ChargeThreshold::saveSettings()
{

    QString battery = QString(" %1 ").arg(Battery::stringFromLocationConsole(
                                              Battery::locationFromString(
                                                  ui->battery_chooser->currentText())));
    QString cmd = "batteryctl";

    if (ui->always->isChecked())
        cmd += " preset " + battery + SETTING_AC_FULL;
    if (ui->always_ac->isChecked())
        cmd += " preset " + battery + SETTING_AC;
    if (ui->life->isChecked())
        cmd += " preset " + battery + SETTING_LIFE;
    if (ui->custom->isChecked()) {
        cmd += " set " + battery + QString::number(ui->start->value()) + " " + QString::number(ui->stop->value());
    }

    int ret = system(QString("pkexec " + cmd).toStdString().c_str());

    if (ret == -1)
        QMessageBox::critical(this, "ACPI error in batteryctl",
                              "The child process pkexec failed to execute.\n\n"
                              "Click Ok to close this message.", QMessageBox::Ok);

    if (ret == 127)
        QMessageBox::critical(this, "ACPI error in batteryctl",
                              "The child process shell could not be created.\n\n"
                              "Click Ok to close this message.", QMessageBox::Ok);

    if (ret != 0)
        QMessageBox::critical(this, "ACPI error in batteryctl",
                              "The child process was created but it failed to configure the batteries.\n\n("
                              + cmd +
                              ")\n\nClick Ok to close this message.", QMessageBox::Ok);

    close();
}

void ChargeThreshold::restoreSettings()
{
    Battery::BatteryLocation location = Battery::locationFromString(ui->battery_chooser->currentText());
    Storage *storage = Storage::getStorage();

    QString type = storage->getSettingType(location);

    if (type == SETTING_AC)
        ui->always_ac->click();
    if (type == SETTING_AC_FULL)
        ui->always->click();
    if (type == SETTING_LIFE)
        ui->life->click();
    if (type == SETTING_CUSTOM) {
        ui->custom->click();
        ui->start->setValue(storage->getStartThreshold(location));
        ui->stop->setValue(storage->getStopThreshold(location));
    }

}

void ChargeThreshold::forceConstraintStart()
{
    if (ui->start->value() > ui->stop->value())
        ui->stop->setValue(ui->start->value() + 1);
}

void ChargeThreshold::forceConstraintStop()
{
    if (ui->stop->value() < ui->start->value())
        ui->start->setValue(ui->stop->value() - 1);
}

