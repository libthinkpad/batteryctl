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

#ifndef STORAGE_H
#define STORAGE_H

#include <QSettings>
#include <QMutex>

#include "battery.h"

#define SETTING_CUSTOM "custom"
#define SETTING_AC "ac"
#define SETTING_AC_FULL "full"
#define SETTING_LIFE "life"

class Storage;

class Storage
{
public:

    static Storage *instance;
    ~Storage();
    Storage();
    static Storage* getStorage();

    QMutex mutex;

    int getStartThreshold(Battery::BatteryLocation location);
    int getStopThreshold(Battery::BatteryLocation location);
    QString getSettingType(Battery::BatteryLocation location);

    void setStartThreshold(Battery::BatteryLocation location, int value);
    void setStopThreshold(Battery::BatteryLocation location, int value);
    void setSettingType(Battery::BatteryLocation location, QString type);

private:
    QSettings *settings;
    void beginGroup(Battery::BatteryLocation location);
};

#endif // STORAGE_H
