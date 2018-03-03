#include "thinkpads_org_about.h"
#include "ui_thinkpads_org_about.h"

thinkpads_org_about::thinkpads_org_about(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::thinkpads_org_about)
{
    ui->setupUi(this);
}

thinkpads_org_about::~thinkpads_org_about()
{
    delete ui;
}
