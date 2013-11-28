#include "dojizdeni.h"
#include "ui_dojizdeni.h"
#include "classday.h"
#include "classmonth.h"

dojizdeni::dojizdeni(ClassMonth **month, ClassDay **day, QWidget *parent):
    QWidget(parent),
    ui(new Ui::dojizdeni),
    plonkDay(day),
    plonkMonth(month),
    count(4)
{
    ui->setupUi(this);
    radios.resize(count);
    radios[0] = ui->radPupik;
    radios[1] = ui->radAutem;
    radios[2] = ui->radKolo;
    radios[3] = ui->radJinak;

    for (int i = 0; i < radios.count(); i++)
        connect(radios[i],SIGNAL(clicked(bool)),this,SLOT(radioClicked(bool)));
}

dojizdeni::~dojizdeni()
{

    delete ui;
}

void dojizdeni::radioClicked(bool )
{
    for (int i = 0 ; i <radios.count(); i++)
    {
        if (radios[i] == sender())
        {
            (*plonkDay)->Dojizdeni = static_cast<ClassDay::dojizdeni_t>(i);
            recount();
            return;
        }
    }
    recount();
}

void dojizdeni::refresh()
{
    for (int i = 0 ; i <radios.count(); i++)
    {
        radios[i]->setChecked(false);
    }

    int i = (*plonkDay)->Dojizdeni;
    radios[i]->setChecked(true);
    recount();
}

void dojizdeni::recount()
{
    float one = 56.525;
    float cena = -170;
    foreach (ClassDay * d, (*plonkMonth)->GetDays())
    {
        if (d->IsOk() && d->Prichod1.isValid() && d->Dojizdeni == ClassDay::PUPIK)
        {
            cena += one;
        }
    }
    ui->labPlatit->setText(QString("%1 CZK").arg(int(cena)));
}
