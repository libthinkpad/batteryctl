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

#include "battery.h"
#include "storage.h"

#include <QFile>
#include <QDebug>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

Battery::Battery()
{

}

void Battery::readBattery(Battery::BatteryLocation location)
{
    capacity = readFileInt(location, "capacity");
    energy_now = readFileInt(location, "energy_now");
    charge_start_threshold = readFileInt(location, "charge_start_threshold");
    charge_stop_threshold = readFileInt(location, "charge_stop_threshold");
    cycle_count = readBatteryCycles(location);
    energy_full = readFileInt(location, "energy_full");
    energy_full_design = readFileInt(location, "energy_full_design");
    manufacturer = readFileString(location, "manufacturer");
    model_name = readFileString(location, "model_name");
    power_now = readFileInt(location, "power_now");
    present = readFileInt(location, "present");
    serial_number = readFileString(location, "serial_number");
    technology = readFileString(location, "technology");
    voltage_now = readFileInt(location, "voltage_now");
    voltage_min_design = readFileInt(location, "voltage_min_design");
    status = Battery::guessBatteryStatus(this, location);


    if (energy_full_design == 0)
        return;

    health = (float) energy_full / energy_full_design * 100.0f;
}

bool Battery::isWearControlSupported(Battery::BatteryLocation location)
{
    QFile f(getBatteryFolder(location) + QString("charge_start_threshold"));
    return f.exists();
}

bool Battery::isPrimaryAvailable()
{
    return isAvailable(Battery::BatteryLocation::Primary);
}

bool Battery::isSecondaryAvailable()
{
    return isAvailable(Battery::BatteryLocation::Secondary);
}

bool Battery::isAvailable(Battery::BatteryLocation location)
{
    QFile f(getBatteryFolder(location));
    return f.exists();
}

void Battery::setStartThreshold(Battery::BatteryLocation location, int value)
{
    if (!isWearControlSupported(location)) {
        qDebug() << "Wear control is not supported. You need Linux 4.17+";
        return;
    }
    setThreshold(location, "charge_start_threshold", value);
    Storage *storage = Storage::getStorage();
    storage->setStartThreshold(location, value);
    storage->setSettingType(location, SETTING_CUSTOM);
}

void Battery::setStopThreshold(Battery::BatteryLocation location, int value)
{
    if (!isWearControlSupported(location)) {
        qDebug() << "Wear control is not supported. You need Linux 4.17+";
        return;
    }
    setThreshold(location, "charge_stop_threshold", value);
    Storage *storage = Storage::getStorage();
    storage->setStopThreshold(location, value);
    storage->setSettingType(location, SETTING_CUSTOM);
}

void Battery::resetThresholdSettings(Battery::BatteryLocation location)
{
    setThreshold(location, "charge_start_threshold", 0);
    setThreshold(location, "charge_stop_threshold", 100);
}

Battery::BatteryLocation Battery::locationFromString(QString location)
{
    if (location == PRIMARY)
        return Battery::BatteryLocation::Primary;
    if (location == SECONDARY)
        return Battery::BatteryLocation::Secondary;
    qDebug() << "!!!ERROR INVALID RUNTINME VALUE!!!";
    exit(1);
}

Battery::BatteryLocation Battery::locationFromStringConsole(QString location)
{
    if (location == "primary")
        return Battery::BatteryLocation::Primary;
    if (location == "secondary")
        return Battery::BatteryLocation::Secondary;
    qDebug() << "!!!ERROR INVALID RUNTINME VALUE!!!";
    exit(1);
}

QString Battery::stringFromLocationConsole(Battery::BatteryLocation location)
{
    if (location == Battery::BatteryLocation::Primary)
        return "primary";
    if (location == Battery::BatteryLocation::Secondary)
        return "secondary";
    qDebug() << "!!!ERROR INVALID RUNTINME VALUE!!!";
    exit(1);
}

QString Battery::guessBatteryStatus(Battery *battery, Battery::BatteryLocation location)
{
    QString status = Battery::readFileString(location, "status");
    if (battery->capacity >= battery->charge_start_threshold
            && battery->capacity <= battery->charge_stop_threshold && status == "Unknown")
        return "Not Charging";
    else
        return status;
}

void Battery::setThreshold(Battery::BatteryLocation where, const char *what, int much)
{
    QString base = getBatteryFolder(where) + what;
    int fd = open(base.toStdString().c_str(), O_WRONLY);
    if (fd < 0) {
        qDebug() << "Error opening file for writing: " << strerror(errno);
        return;
    }
    QString data = QString::number(much);
    if (write(fd, data.toStdString().c_str(), data.length() + 1) < 0) {
        qDebug() << "Error writing"  << what << " (" << much << ") to file: " << strerror(errno);
        return;
    }
    close(fd);
}

QString Battery::readFileString(Battery::BatteryLocation location, QString file)
{
    QFile data(getBatteryFolder(location) + file);
    if (!data.exists())
        return "Not Available";
    data.open(QIODevice::ReadOnly);
    QByteArray arr = data.read(1024);
    data.close();
    return QString(arr).replace("\n", "");
}

int Battery::readFileInt(Battery::BatteryLocation location, QString file)
{
    QString ret = readFileString(location, file);
    if (ret == "Not Available")
        return 0;
    return ret.toInt();
}

int Battery::readBatteryCycles(Battery::BatteryLocation location)
{
    QString location_str;
    switch (location) {
    case Battery::BatteryLocation::Primary:
        location_str = ("/sys/devices/platform/smapi/BAT0/cycle_count");
        break;
    case Battery::BatteryLocation::Secondary:
        location_str = ("/sys/devices/platform/smapi/BAT1/cycle_count");
        break;
    }

    QFile smapi(location_str);

    if (smapi.exists()) {
        smapi.open(QIODevice::ReadOnly);
        QByteArray arr = smapi.read(1024);
        smapi.close();
        return QString(arr).replace("\n", "").toInt();
    }
    return readFileInt(location, "cycle_count");
}

QString Battery::getBatteryFolder(Battery::BatteryLocation location)
{
    switch (location) {
    case Battery::BatteryLocation::Primary:
        return "/sys/class/power_supply/BAT0/";
    case Battery::BatteryLocation::Secondary:
        return "/sys/class/power_supply/BAT1/";
    }
    return "Not Available";
}

