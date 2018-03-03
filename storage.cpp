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

#include "storage.h"

#include <QDebug>

Storage* Storage::instance = nullptr;

Storage::~Storage()
{
    delete settings;
}

Storage::Storage()
{
    mutex.lock();
    settings = new QSettings("/etc/batteryctl/values.conf", QSettings::IniFormat);
    if (settings->status() != QSettings::NoError) {
        qDebug() << "Error opening settings file!";
        exit(1);
    }
    settings->beginGroup("Primary");
    QString type = settings->value("type", "none").toString();
    if (type == "none") {
        settings->setValue("type", SETTING_AC_FULL);
        settings->setValue("start", 0);
        settings->setValue("stop", 100);
    }
    settings->endGroup();

    settings->beginGroup("Secondary");
    type = settings->value("type", "none").toString();
    if (type == "none") {
        settings->setValue("type", SETTING_AC_FULL);
        settings->setValue("start", 0);
        settings->setValue("stop", 100);
    }
    settings->endGroup();
    mutex.unlock();
}

Storage *Storage::getStorage()
{
    if (instance == nullptr)
        instance = new Storage();
    return instance;
}

int Storage::getStartThreshold(Battery::BatteryLocation location)
{
    mutex.lock();
    beginGroup(location);
    int ret =  settings->value("start", 0).toInt();
    settings->endGroup();
    mutex.unlock();
    return ret;
}

int Storage::getStopThreshold(Battery::BatteryLocation location)
{
    mutex.lock();
    beginGroup(location);
    int ret = settings->value("stop", 100).toInt();
    settings->endGroup();
    mutex.unlock();
    return ret;
}

QString Storage::getSettingType(Battery::BatteryLocation location)
{
    mutex.lock();
    beginGroup(location);
    QString ret = settings->value("type", SETTING_AC_FULL).toString();
    settings->endGroup();
    mutex.unlock();
    return ret;
}

void Storage::setStartThreshold(Battery::BatteryLocation location, int value)
{
    mutex.lock();
    beginGroup(location);
    settings->setValue("start", value);
    settings->endGroup();
    settings->sync();
    mutex.unlock();
}

void Storage::setStopThreshold(Battery::BatteryLocation location, int value)
{
    mutex.lock();
    beginGroup(location);
    settings->setValue("stop", value);
    settings->endGroup();
    settings->sync();
    mutex.unlock();
}

void Storage::setSettingType(Battery::BatteryLocation location, QString type)
{
    mutex.lock();
    beginGroup(location);
    settings->setValue("type", type);
    settings->endGroup();
    settings->sync();
    mutex.unlock();
}

void Storage::beginGroup(Battery::BatteryLocation location)
{
    if (location == Battery::BatteryLocation::Primary)
        settings->beginGroup("Primary");
    else
        settings->beginGroup("Secondary");
}
