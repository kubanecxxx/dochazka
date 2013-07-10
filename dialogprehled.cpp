#include "dialogprehled.h"
#include "ui_dialogprehled.h"
#include <QTreeWidget>
#include <QPushButton>

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
    //vytvořit stroma
    QTreeWidgetItem *date, *work;

    QList<ClassDay *> days = month.GetDays();

    foreach (ClassDay * day, days) {
        if (day->Prichod1.isValid() || day->dovolena)
        {
            date = new QTreeWidgetItem;
            date->setFlags(Qt::ItemIsEnabled);
            ui->treeWidget->addTopLevelItem(date);
            date->setData(0,Qt::DisplayRole,day->datum);
            if (day->dovolena)
            {
                date->setText(1,trUtf8("Dovolená"));
            }
            else
            {
                date->setData(5,Qt::DisplayRole,day->GetHodinyPrace());
                //date->setData(2,Qt::DisplayRole,day->Odchod1);
            }

            QList<ClassDay::prace_t *> praces = day->GetPrace();
            foreach (ClassDay::prace_t * prace, praces) {
                work = new QTreeWidgetItem;
                work->setFlags(Qt::ItemIsEnabled);
                date->addChild(work);
                work->setData(0,Qt::DisplayRole,prace->hodiny);

                if (prace->mx)
                    work->setText(1,"MX");
                else
                    work->setText(1,"MP");


                work->setData(2,Qt::DisplayRole,prace->hlaseni);
                work->setText(3,prace->Poznamka);
                if (prace->prescas)
                    work->setCheckState(4,Qt::Checked);
                else
                    work->setCheckState(4,Qt::Unchecked);
            }
        }
    }
}

void DialogPrehled::on_butRol_clicked()
{
    static bool jo = true;

    if (jo)
    {
        ui->treeWidget->expandAll();
        ui->butRol->setText(trUtf8("Sbalit"));
    }
    else
    {
        ui->treeWidget->collapseAll();
        ui->butRol->setText(trUtf8("Rozbalit"));
    }

    jo = !jo;
}
