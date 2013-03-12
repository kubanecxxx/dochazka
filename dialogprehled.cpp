#include "dialogprehled.h"
#include "ui_dialogprehled.h"

DialogPrehled::DialogPrehled(const ClassMonth & Month, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogPrehled),
    month(Month)
{
    ui->setupUi(this);
    Refresh();
}

DialogPrehled::~DialogPrehled()
{
    delete ui;
}

void DialogPrehled::Refresh()
{

}
