#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "core/battery.h"
#include "chargethreshold.h"
#include "thinkpads_org_about.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    thinkpads_org_about about;

    Battery *primary = nullptr;
    Battery *secondary = nullptr;
    ChargeThreshold *thresholds;
    QTimer *refresh;

    void evaluateBatteries();
    void displayBatteryInfo(Battery &battery);
    void displayCondition();
    void displayDamaged(bool damaged);
    void removeAllBatteries();
    void displayTotalRemaining();
    static QPixmap getManufacturerLogo(QString manufacturer);
    QPixmap getBatteryHealthIcon(Battery *health);
    QString getBatteryHealth(Battery *battery);

public slots:
    void refreshData();
    void displaySelectedBattery(QString bat);
    void openSite();
    void openAbout();

};

#endif // MAINWINDOW_H
