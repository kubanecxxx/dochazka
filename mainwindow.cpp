#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFile>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    PlonkDay = new ClassDay;

    QFile fil("sobor");
    fil.open(QFile::ReadOnly);
    QString str = fil.readAll();
    PlonkDay->ReadTextLine(str);
    fillFormDay(*PlonkDay);

}

MainWindow::~MainWindow()
{
    delete ui;
}




void MainWindow::fillFormDay(const ClassDay &day)
{
    ui->editPrichod1->setText(day.Prichod1.toString(TIMEFORMAT));
    ui->editPrichod2->setText(day.Prichod2.toString(TIMEFORMAT));
    ui->editOdchod1->setText(day.Odchod1.toString(TIMEFORMAT));
    ui->editOdchod2->setText(day.Odchod2.toString(TIMEFORMAT));

    //vyplnit hodiny vykázany a vpráci

    //vyplnit tabulku
    ui->table->setRowCount(0);
    QList<ClassDay::prace_t *> list = day.GetPrace();
    int i = 0;
    foreach(ClassDay::prace_t * prac, list)
    {
        QTableWidgetItem * item;
        AddRow();

        disconnect(ui->table,SIGNAL(itemChanged(QTableWidgetItem*)),
                   this,SLOT(on_table_itemChanged(QTableWidgetItem*)));
        //hodiny
        item = ui->table->item(i,0);
        item->setData(Qt::DisplayRole,prac->hodiny);

        //mx
        item = ui->table->item(i,1);
        item->setText(prac->hlaseni);
        ui->table->setItem(i,1,item);

        //poznamka
        item = ui->table->item(i,2);
        if (!item)
            item = new QTableWidgetItem;
        item->setText(prac->Poznamka);

        //prescas
        item = ui->table->item(i,3);
        if (!item)
            item = new QTableWidgetItem;
        if (prac->prescas)
            item->setCheckState(Qt::Checked);
        else
            item->setCheckState(Qt::Unchecked);

        connect(ui->table,SIGNAL(itemChanged(QTableWidgetItem*)),
                   this,SLOT(on_table_itemChanged(QTableWidgetItem*)));
        i++;
    }
    AddRow();
}

void MainWindow::on_actionSave_triggered()
{
    // nahrát plonkday do seznamu dnů třída classMonth
    QString ret= PlonkDay->GetTextLine();
    qDebug() << ret;
    QFile fil("sobor");
    fil.open(QFile::WriteOnly);
    fil.write(ret.toLatin1());
    fil.close();
}

void MainWindow::on_actionPrehled_triggered()
{


    fillFormDay(*PlonkDay);
}

void MainWindow::on_table_itemChanged(QTableWidgetItem *item)
{
    int row = item->row();
    if (row < 0)
        return;


    if (row + 1 == ui->table->rowCount() && ui->table->item(ui->table->rowCount()-1,1)->text().count())
    {
        AddRow();
    }

    //if řádek okej tak ho teprve přidat
    //@todo dopsat tuhle podminku

    ClassDay::prace_t * prace = PlonkDay->GetPraceP(row);
    if (!prace)
    {
        //pokud tam neni tak ho vymyslet znova
        prace = PlonkDay->AddPrace();
    }

    if (ui->table->item(row,0))
        prace->hodiny = ui->table->item(row,0)->data(Qt::DisplayRole).toFloat();
    if (ui->table->item(row,1))
        prace->hlaseni = ui->table->item(row,1)->text();
    if (ui->table->item(row,2))
        prace->Poznamka = ui->table->item(row,2)->text();
    if (ui->table->item(row,3))
    {
        bool jo;
        if (ui->table->item(row,3)->checkState() == Qt::Checked)
            jo = true;
        else
            jo = false;
        prace->prescas = jo;
    }

}

void MainWindow::AddRow()
{
    disconnect(ui->table,SIGNAL(itemChanged(QTableWidgetItem*)),
               this,SLOT(on_table_itemChanged(QTableWidgetItem*)));
    ui->table->insertRow(ui->table->rowCount());

    QTableWidgetItem * item;
    int i = ui->table->rowCount() -1;

    //hodiny
    item = new QTableWidgetItem;
    ui->table->setItem(i,0,item);

    //mpú
    item = new QTableWidgetItem;
    ui->table->setItem(i,1,item);

    //poznamka
    item = new QTableWidgetItem;
    ui->table->setItem(i,2,item);

    //prescas
    item = new QTableWidgetItem;
    item->setCheckState(Qt::Unchecked);
    ui->table->setItem(i,3,item);

    connect(ui->table,SIGNAL(itemChanged(QTableWidgetItem*)),
               this,SLOT(on_table_itemChanged(QTableWidgetItem*)));
}

void MainWindow::SetTime(const QString &arg, QTime &time,QObject* widget)
{
    QLineEdit * edit = qobject_cast<QLineEdit*>(widget);

    QPalette pal;
    QColor col;

    if (QTime::fromString(arg,TIMEFORMAT).isValid())
    {
        time = QTime::fromString(arg,TIMEFORMAT);
        col = Qt::black;
    }
    else
    {
        col = Qt::red;
    }

    pal.setColor(QPalette::Text,col);
    edit->setPalette(pal);
}

void MainWindow::on_editPrichod2_textEdited(const QString &arg1)
{
    SetTime(arg1,PlonkDay->Prichod2,sender());
}

void MainWindow::on_editOdchod1_textEdited(const QString &arg1)
{
    SetTime(arg1,PlonkDay->Odchod1,sender());
}

void MainWindow::on_editPrichod1_textEdited(const QString &arg1)
{
    SetTime(arg1,PlonkDay->Prichod1,sender());
}

void MainWindow::on_editOdchod2_textEdited(const QString &arg1)
{
    SetTime(arg1,PlonkDay->Odchod2,sender());
}
