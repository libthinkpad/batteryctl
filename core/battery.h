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

#ifndef BATTERY_H
#define BATTERY_H

#include <QObject>
#include <QString>

#define PRIMARY "1 - Main Battery"
#define SECONDARY "2 - Seconday Battery"

class Battery : public QObject
{
    Q_OBJECT

public:

    enum BatteryLocation {
        Primary, Secondary
    };

    enum Condition {
        Good, Poor, Bad
    };

    Battery();
    int capacity;
    int energy_now;
    int charge_start_threshold;
    int charge_stop_threshold;
    int cycle_count;
    int energy_full;
    int energy_full_design;
    QString manufacturer;
    QString model_name;
    int power_now;
    int present;
    QString serial_number;
    QString status;
    QString technology;
    int voltage_now;
    int voltage_min_design;
    float health;

    void readBattery(Battery::BatteryLocation location);
    static bool isWearControlSupported(BatteryLocation location);

    static bool isPrimaryAvailable();
    static bool isSecondaryAvailable();
    static bool isAvailable(Battery::BatteryLocation location);

    static void setStartThreshold(Battery::BatteryLocation location, int value);
    static void setStopThreshold(Battery::BatteryLocation location, int value);

    static void resetThresholdSettings(Battery::BatteryLocation location);

    static Battery::BatteryLocation locationFromString(QString location);
    static Battery::BatteryLocation locationFromStringConsole(QString location);

    static QString stringFromLocationConsole(Battery::BatteryLocation location);
    static QString guessBatteryStatus(Battery *battery, Battery::BatteryLocation location);


private:
    static QString readFileString(BatteryLocation location, QString file);
    int readFileInt(BatteryLocation location, QString file);
    int readBatteryCycles(BatteryLocation location);
    static QString getBatteryFolder(BatteryLocation location);
    static void setThreshold(Battery::BatteryLocation where, const char *what, int much);

};

#endif // BATTERY_H
