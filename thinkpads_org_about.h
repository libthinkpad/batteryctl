#ifndef THINKPADS_ORG_ABOUT_H
#define THINKPADS_ORG_ABOUT_H

#include <QDialog>

namespace Ui {
class thinkpads_org_about;
}

class thinkpads_org_about : public QDialog
{
    Q_OBJECT

public:
    explicit thinkpads_org_about(QWidget *parent = 0);
    ~thinkpads_org_about();

private:
    Ui::thinkpads_org_about *ui;
};

#endif // THINKPADS_ORG_ABOUT_H
