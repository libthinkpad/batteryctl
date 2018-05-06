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

#include <iostream>
#include <QApplication>
#include <QDebug>

#include "mainwindow.h"
#include "battery.h"
#include "storage.h"

#define VERSION "1.11"

QTextStream& qStdOut()
{
    static QTextStream ts( stdout );
    return ts;
}

void printVersion()
{
    qStdOut() << (
                     "batteryctl " VERSION "\n"
                     "Copyright (C) 2018 The Thinkpads.org Team\n"
                     "License: FreeBSD License (BSD 2-Clause) <https://www.freebsd.org/copyright/freebsd-license.html>.\n"
                     "This is free software: you are free to change and redistribute it.\n"
                     "There is NO WARRANTY, to the extent permitted by law.\n"
                     "\n"
                     "\n"
                     "Written by Ognjen Galic\n"
                     "See --help for more information\n"
                     ""
    );
}

void printHelp()
{
    qStdOut() << (
                     "Usage:\n"
                     "\n"
                     "   info\t\t\t\t\t\tPrint detailed information about the batteries\n"
                     "   set\t\t\t\t\t\tSet a custom charge threshold for the batteries\n"
                     "       start [primary|secondary] (value)  \tSet the start charge threshold\n"
                     "       stop [primary|secondary] (value)\t\tSet the stop charge threshold\n"
                     "       [primary|secondary] (start) (stop)\tSet both thresholds at once\n"
                     "\n"
                     "   preset [primary|secondary] (value)\t\tSet a preset charge start/stop threshold pair\n"
                     "       full\t\t\t\t\tCharge the battery to 100% always (default)\n"
                     "       ac\t\t\t\t\tCharge the battery to 100% but optimize for always AC\n"
                     "       life\t\t\t\t\tOptimize for maximum battery life (cycles)\n"
                     " \n"
                     "   gui\t\t\t\t\t\tRun the Qt GUI\n"
                     "   restore\t\t\t\t\t\tRestore the stored settings to the batteries"
                     "\n"
                     "   --help\t\t\t\t\tPrint this help\n"
                     "   --version\t\t\t\t\tPrint the version\n"
                     "\n"
                     "Examples:\n"
                     "\n"
                     " batteryctl info\t\t\t\tPrint the information\n"
                     " batteryctl set start primary 45\t\tSet the charge threshold of the primary bat to 45\n"
                     " batteryctl set stop primary 45\t\t\tSet the charge stop of the primary battery to 45\n"
                     " batteryctl preset primary life\t\t\tOptimize the primary battery for battery life (cycles)\n\n"
    );
}

void printBatteryInfo(Battery *bat) {

    qStdOut() << QString(QString("Status:\t\t\t\t%1\nCapacity:\t\t\t%2\nCurrent capacity:\t\t%3\n"
                                 "Full charge capacity:\t\t%4\nCurrent:\t\t\t%5\nVoltage:\t\t\t%6\n"
                                 "Wattage:\t\t\t%7\nCharge start threshold:\t\t%8\nCharge stop threshold:\t\t%9\n").arg(
                             bat->status,
                             QString::number(bat->capacity) + " %",
                             bat->energy_now == 0 ? "-" : QString::number(bat->energy_now / 1000000.0f) + " Wh",
                             bat->energy_full == 0 ? "-" : QString::number(bat->energy_full/ 1000000.0f) + " Wh",
                             bat->power_now == 0 ? "-" : QString::number((bat->power_now / (float) bat->voltage_now)) + " A",
                             bat->voltage_now == 0 ? "-" : QString::number(bat->voltage_now / 1000000.0f) + " V",
                             bat->power_now == 0 ? "-" : QString::number(bat->power_now / 1000000.0f) + " W",
                             QString::number(bat->charge_start_threshold) + " %",
                             QString::number(bat->charge_stop_threshold) + " %"));

}

void printBatteryOptional(Battery::BatteryLocation location)
{
    QString name = location == Battery::BatteryLocation::Primary ? "1 - Main Battery" : "2 - Secondary Battery";
    Battery battery;
    if (Battery::isAvailable(location)) {
        qStdOut() << name << " - Installed\n";
        battery.readBattery(location);
        printBatteryInfo(&battery);
    } else {
        qStdOut() << name << " - Not Installed\n";
    }
    qStdOut() << "\n";
}

int setThreshold(QString what, QString where, QString value_raw)
{
    int value;
    bool ok;
    bool battery_ok = false;
    Battery::BatteryLocation location;
    Battery battery;

    value = value_raw.toInt(&ok);

    if (!ok) {
        qStdOut() << "Invalid number: " << value_raw << "\n";
        return 1;
    }

    if (what == "primary" || what == "secondary") {
        int start = where.toInt(&ok);
        if (!ok) {
            qStdOut() << "Invalid start threshold " << what << "\n";
            return 1;
        }
        int stop = value_raw.toInt(&ok);
        if (!ok) {
            qStdOut() << "Invalid stop threshold " << value_raw << "\n";
            return 1;
        }
        Battery::BatteryLocation location = Battery::locationFromStringConsole(what);
        Battery::resetThresholdSettings(location);
        Battery::setStartThreshold(location, start);
        Battery::setStopThreshold(location, stop);
        return 0;
    }

    if (where == "primary") {
        location = Battery::BatteryLocation::Primary;
        battery_ok = true;
    }

    if (where == "secondary") {
        location = Battery::BatteryLocation::Secondary;
        battery_ok = true;
    }

    if (!battery_ok) {
        qStdOut() << "Invalid battery: " << where << "\n";
        return 1;
    }

    if (!battery.isAvailable(location)) {
        qStdOut() << "Battery not available\n";
        return 1;
    }

    if (what == "start") {
        Battery::setStartThreshold(location, value);
        return 0;
    }

    if (what == "stop") {
        Battery::setStopThreshold(location, value);
        return 0;
    }

    qStdOut() << "Invalid action: " << what << "\n";

    return 1;

}

int setPreset(QString which, QString where)
{
    bool battery_ok = false;
    Battery::BatteryLocation location;
    Storage *storage = Storage::getStorage();

    if (where == "primary") {
        location = Battery::BatteryLocation::Primary;
        battery_ok = true;
    }

    if (where == "secondary") {
        location = Battery::BatteryLocation::Secondary;
        battery_ok = true;
    }

    if (!battery_ok) {
        qStdOut() << "Unknown battery: " << where << "\n";
        return 1;
    }

    if (which == SETTING_AC) {
        Battery::resetThresholdSettings(location);
        Battery::setStopThreshold(location, 100);
        Battery::setStartThreshold(location, 96);
        storage->setSettingType(location, SETTING_AC);
        return 0;
    }

    if (which == SETTING_AC_FULL) {
        Battery::resetThresholdSettings(location);
        Battery::setStopThreshold(location, 100);
        Battery::setStartThreshold(location, 0);
        storage->setSettingType(location, SETTING_AC_FULL);
        return 0;
    }

    if (which == SETTING_LIFE) {
        Battery::resetThresholdSettings(location);
        Battery::setStopThreshold(location, 85);
        Battery::setStartThreshold(location, 65);
        storage->setSettingType(location, SETTING_LIFE);
        return 0;
    }

    qStdOut() << "Unknown preset: " << which << "\n";
    return 1;
}

int restoreSettings()
{
    Storage *storage = Storage::getStorage();
    QString type;

    if (Battery::isPrimaryAvailable()) {
        qStdOut() << "Restoring settings on primary battery\n";
        Battery::BatteryLocation location = Battery::BatteryLocation::Primary;
        type = storage->getSettingType(location);
        Battery::resetThresholdSettings(location);
        Battery::setStartThreshold(location, storage->getStartThreshold(location));
        Battery::setStopThreshold(location, storage->getStopThreshold(location));
        storage->setSettingType(location, type);
    }

    if (Battery::isSecondaryAvailable()) {
        qStdOut() << "Restoring settings on secondary battery\n";
        Battery::BatteryLocation location = Battery::BatteryLocation::Secondary;
        type = storage->getSettingType(location);
        Battery::resetThresholdSettings(location);
        Battery::setStartThreshold(location, storage->getStartThreshold(location));
        Battery::setStopThreshold(location, storage->getStopThreshold(location));
        storage->setSettingType(location, type);
    }

    return 0;

}

int runConsole(int argc, char **argv)
{
    QString command(argv[1]);

    if (command == "--help") {
        printHelp();
        return 0;
    }

    if (command == "--version") {
        printVersion();
        return 0;
    }

    if (command == "info") {
        printBatteryOptional(Battery::BatteryLocation::Primary);
        printBatteryOptional(Battery::BatteryLocation::Secondary);
        return 0;
    }

    if (command == "set") {
        if (argc < 5) {
            qStdOut() << "Not enough arguments, see --help\n";
            return 11;
        }
        return setThreshold(QString(argv[2]), QString(argv[3]), QString(argv[4]));
    }

    if (command == "preset") {
        if (argc < 4) {
            qStdOut() << "Not enough arguments, see --help\n";
            return 1;
        }
        return setPreset(QString(argv[3]), QString(argv[2]));
    }

    if (command == "restore") {
        return restoreSettings();
    }

    if (command == "gui") {
        QApplication app(argc, argv);
        MainWindow control;
        control.show();
        exit(app.exec());
    }

    qStdOut() << QString("Unknown command: %1, see --help.\n").arg(command);
    return 1;
}

int main(int argc, char **argv)
{
    if (argc > 1)
        return runConsole(argc, argv);
    else
        printVersion();
}
