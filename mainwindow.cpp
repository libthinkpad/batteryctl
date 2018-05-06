#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "thinkpads_org_about.h"

#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::MainWindow), refresh(QTimer())
{
    ui->setupUi(this);
    setWindowIcon(QIcon::fromTheme("battery"));

    ui->primary_battery->setVisible(false);
    ui->secondary_battery->setVisible(false);

    evaluateBatteries();
    refreshData();

    connect(refresh, SIGNAL(timeout()), this, SLOT(refreshData()));
    connect(ui->battery_combo, SIGNAL(currentIndexChanged(QString)), this, SLOT(displaySelectedBattery(QString)));
    connect(ui->close_button, SIGNAL(clicked(bool)), this, SLOT(close()));

    this->refresh->setInterval(3000);
    this->refresh->start();

    thresholds = new ChargeThreshold();
    connect(ui->maintain, SIGNAL(clicked(bool)), thresholds, SLOT(open()));

    connect(ui->actionAbout_Battery_Control, SIGNAL(triggered(bool)), this, SLOT(openAbout()));
    connect(ui->actionVisit_Thinkpads_org, SIGNAL(triggered(bool)), this, SLOT(openSite()));

}

void MainWindow::evaluateBatteries()
{
    ui->battery_combo->clear();
    QString backup = ui->battery_combo->currentText();

    if (Battery::isWearControlSupported(Battery::BatteryLocation::Primary) ||
            Battery::isWearControlSupported(Battery::BatteryLocation::Secondary))
        ui->maintain->setEnabled(true);

    if (Battery::isPrimaryAvailable()) {
        if (primary == nullptr)
            primary = new Battery();
        primary->readBattery(Battery::BatteryLocation::Primary);
        ui->battery_combo->addItem(PRIMARY);
    } else {
        if (primary != nullptr)
            delete primary;
        primary = nullptr;
    }

    if (Battery::isSecondaryAvailable()) {
        if (secondary == nullptr)
            secondary = new Battery();
        secondary->readBattery(Battery::BatteryLocation::Secondary);
        ui->battery_combo->addItem(SECONDARY);
    } else {
        if (secondary != nullptr)
            delete secondary;
        secondary = nullptr;
    }

    ui->battery_combo->setCurrentText(backup);
}

void MainWindow::displayBatteryInfo(Battery &battery)
{
    /*
     * Status, Remaining Percentage, Remaining capacity, Full capacity, Current, Voltage, Wattage, Cycles
     */

    this->ui->battery_info->setText(QString("%1\n%2\n%3\n%4\n%5\n%6\n%7\n%8").arg(
                                        battery.status,
                                        QString::number(battery.capacity) + " %",
                                        battery.energy_now == 0 ? "-" : QString::number(battery.energy_now / 1000000.0f) + " Wh",
                                        battery.energy_full == 0 ? "-" : QString::number(battery.energy_full/ 1000000.0f) + " Wh",
                                        battery.power_now == 0 ? "-" : QString::number((battery.power_now / (float) battery.voltage_now)) + " A",
                                        battery.voltage_now == 0 ? "-" : QString::number(battery.voltage_now / 1000000.0f) + " V",
                                        battery.power_now == 0 ? "-" : QString::number(battery.power_now / 1000000.0f) + " W",
                                        battery.cycle_count == 0 ? "-" : QString::number(battery.cycle_count)));

    this->ui->battery_manu->setText(QString("%1\n%2\n%3\n%4\n%5\n%6").arg(
                                        battery.manufacturer,
                                        battery.serial_number,
                                        battery.model_name,
                                        battery.technology,
                                        QString::number(battery.energy_full_design / 1000000.0f) + "Wh",
                                        QString::number(battery.voltage_min_design / 1000000.0f) + "V"));

    this->ui->manu_logo->setPixmap(getManufacturerLogo(battery.manufacturer));

}

void MainWindow::displayCondition()
{
    ui->primary_battery->setVisible(false);
    ui->secondary_battery->setVisible(false);

    if (primary != nullptr) {
        ui->primary_battery->setVisible(true);
        ui->primary_icon->setPixmap(getBatteryHealthIcon(primary));
        ui->primary_text->setText(getBatteryHealth(primary));
    }

    if (secondary != nullptr) {
        ui->secondary_battery->setVisible(true);
        ui->secondary_icon->setPixmap(getBatteryHealthIcon(secondary));
        ui->secondary_text->setText(getBatteryHealth(secondary));
    }
}

QPixmap MainWindow::getBatteryHealthIcon(Battery *health)
{
    if (health->health < 10)
        return QPixmap(":/bad.png");
    if (health->health < 30)
        return QPixmap(":/fair.png");
    return QPixmap(":/good.png");
}

QString MainWindow::getBatteryHealth(Battery *health)
{
    if (health->health < 10) {
        displayDamaged(true);
        return "Poor";
    }
    if (health->health < 30) {
        displayDamaged(true);
        return "Fair";
    }
    displayDamaged(false);
    return "Good";
}

void MainWindow::displayDamaged(bool damaged)
{
    if (damaged)
        ui->status->document()->setPlainText(("One of the batteries is in poor condition. Consider replacing the battery."));
    else if (!(Battery::isWearControlSupported(Battery::BatteryLocation::Primary) ||
             Battery::isWearControlSupported(Battery::BatteryLocation::Secondary)))
        ui->status->document()->setPlainText(("The batteries are in good condition. "
                                              "Setting of the thresholds is only supported on Sandy Bridge"
                                              " Lenovo ThinkPad laptops or newer, and on Linux 4.17."));
    else
        ui->status->document()->setPlainText(("The batteries are in good condition."));
}

void MainWindow::removeAllBatteries()
{
    ui->battery->setPercentage(0);
    ui->battery_combo->clear();
    ui->status->document()->setPlainText("No batteries are installed.");
    ui->battery_info->setText("-\n-\n-\n-\n-\n-\n-\n-");
    ui->battery_manu->setText("-\n-\n-\n-\n-\n-");
    ui->primary_battery->setVisible(false);
    ui->secondary_battery->setVisible(false);
    ui->maintain->setEnabled(false);
    ui->manu_logo->setPixmap(QPixmap());
}

void MainWindow::displayTotalRemaining()
{
    int max = 0;
    int current = 0;

    if (primary != nullptr) {
        max += 100;
        current += primary->capacity;
    }

    if (secondary != nullptr) {
        max += 100;
        current += secondary->capacity;
    }

    if (max == 0) {
        ui->battery->setPercentage(0);
        return;
    }

    ui->battery->setPercentage((int) (current / (float) max * 100.0f));
}

QPixmap MainWindow::getManufacturerLogo(QString manufacturer)
{
    QString lower = manufacturer.toLower();
    if (lower.contains("lg"))
        return QPixmap(":/lgc.png");
    if (lower.contains("sanyo"))
        return QPixmap(":/sanyo.png");
    if (lower.contains("sony"))
        return QPixmap(":/sony.png");
    if (lower.contains("pan"))
        return QPixmap(":/panasonic.png");
    return QPixmap();
}


void MainWindow::displaySelectedBattery(QString bat)
{
    if (primary == nullptr && secondary == nullptr) {
        removeAllBatteries();
        return;
    }

    if (bat == PRIMARY)
        if (primary != nullptr)
            displayBatteryInfo(*primary);

    if (bat == SECONDARY)
        if (secondary != nullptr)
            displayBatteryInfo(*secondary);

}

void MainWindow::openSite()
{
    QDesktopServices::openUrl(QUrl("http://thinkpads.org"));
}

void MainWindow::openAbout()
{
    about.show();
}

void MainWindow::refreshData()
{
    evaluateBatteries();
    QString data = ui->battery_combo->currentText();
    if (data == "") {
        removeAllBatteries();
        return;
    }
    displaySelectedBattery(data);
    displayTotalRemaining();
    displayCondition();
}


MainWindow::~MainWindow()
{
    delete ui;
    refresh->stop();
    delete refresh;
}

