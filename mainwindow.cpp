#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFile>
#include "dialogprehled.h"
#include <QPrinter>
#include <QTextDocument>
#include <QTextTable>
#include <QTextTableFormat>
#include <QPrintDialog>
#include "widgettoolbar.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //jméno
    tool = new WidgetToolBar(this);
    ui->toolBar->insertWidget(ui->actionSaveName,tool);
    connect(ui->actionSaveName,SIGNAL(triggered()),
            tool,SLOT(SaveDataToFile()));
    connect(tool,SIGNAL(DovolenaChanged(int)),this,SLOT(on_dovolenaChanged(int)));

    year = new ClassYear;
    year->SetVolnaDovolena(tool->ui->spinDovolena->value());
    PlonkDay = year->GetDay(QDate::currentDate());
    PlonkMonth = year->GetMonth(QDate::currentDate());
    fillForm();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::fillForm()
{
    setDovolena(PlonkDay->dovolena);
    fillFormDay(*PlonkDay);
    LoadCombos(*PlonkDay);
    fillFormMonth(*PlonkMonth);
    recolorCalendar(*PlonkMonth,PlonkDay->datum);
}

void MainWindow::setDovolena(bool enabled)
{
    if (enabled)
        ui->checkDovolena->setText(trUtf8("Ano"));
    else
        ui->checkDovolena->setText(trUtf8("Ne"));

    ui->table->setDisabled(enabled);
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
    //LoadCombos();
}

void MainWindow::on_actionSave_triggered()
{
    year->SaveFile();
}

void MainWindow::on_actionPrehled_triggered()
{
    DialogPrehled dlg(*PlonkMonth,this);
    dlg.exec();
}

void MainWindow::on_table_itemChanged(QTableWidgetItem *item)
{
    int row = item->row();
    int col = item->column();
    if (row < 0 || col < 0)
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

    fillForm();
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
    if (PlonkDay->IsNotWeekend())
    {
        item->setCheckState(Qt::Unchecked);
    }
    else
    {
        item->setCheckState(Qt::Checked);
        item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
    }
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
    LoadCombos(*PlonkDay);
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

void MainWindow::on_calendarWidget_clicked(const QDate &date)
{
    PlonkDay  = year->GetDay(date);
    PlonkMonth =  year->GetMonth(date);

    fillForm();
}

void MainWindow::LoadCombos(const ClassDay &day)
{
    ui->editVykazano->setText(QString("%1").arg(day.GetHodinyVykazano()));
    ui->editVpraci->setText(QString("%1").arg(day.GetHodinyPrace()));
    ui->editPrescas->setText(QString("%1").arg(day.GetHodinyPrescasVykazano()));
    ui->checkDovolena->setChecked(day.dovolena);
    ui->checkDovolena->setEnabled(day.IsNotWeekend());

    QPalette pal;
    QColor col;

    if (day.IsOk())
    {
        col = Qt::black;
    }
    else
    {
        col = Qt::red;
    }

    pal.setColor(QPalette::Text,col);
    ui->editVpraci->setPalette(pal);
    ui->editVykazano->setPalette(pal);
    ui->editPrescas->setPalette(pal);

    //fillFormMonth();
}

void MainWindow::fillFormMonth(ClassMonth & month)
{
    ui->editMVykazano->setText(QString("%1").arg(month.GetVykazanoHours()));
    ui->editMRegul->setText(QString("%1").arg(month.GetEstimatedHours()));
    ui->editMVpraci->setText(QString("%1").arg(month.GetHoursInWork()));
    ui->editMPVykazano->setText(QString("%1").arg(month.GetVykazanoPrescas()));
    ui->editMDovol->setText(QString("%1").arg(year->GetVolnaDovolena()));

    QPalette pal;
    QColor col;
    if (month.IsVykazanoRegulerne())
        col = Qt::black;
    else
        col = Qt::red;
    pal.setColor(QPalette::Text,col);
    ui->editMRegul->setPalette(pal);

    if (month.IsVykazanoVPraci())
        col = Qt::black;
    else
        col = Qt::red;
    pal.setColor(QPalette::Text,col);
    ui->editMPVykazano->setPalette(pal);
    ui->editMVpraci->setPalette(pal);
    ui->editMVykazano->setPalette(pal);
}

void MainWindow::recolorCalendar(ClassMonth &month,QDate ch)
{
    //kalendář vyplnit správnejma barvama
    for (int i = 1; i <= QDate::currentDate().daysInMonth(); i++)
    {
        QDate date(ch.year(),ch.month(),i);
        bool bold = !month.GetDay(date)->IsOk();

        QTextCharFormat format = ui->calendarWidget->dateTextFormat(date);
        QBrush brush;
        if (i <= QDate::currentDate().day() && bold &&
                PlonkDay->datum.month() == QDate::currentDate().month())
            brush.setColor(Qt::cyan);
        else
            brush.setColor(Qt::white);

        if (month.GetDay(date)->dovolena)
            brush.setColor(Qt::yellow);

        format.setBackground(brush);
        ui->calendarWidget->setDateTextFormat(date,format);
    }
}

void MainWindow::on_checkDovolena_clicked(bool checked)
{
    PlonkDay->dovolena = checked;
    fillForm();
}




#define DEBUG

void MainWindow::on_actionPrint_triggered()
{
    QPrinter printer(QPrinter::PrinterResolution);
#ifndef DEBUG
    QPrintDialog dialog(&printer, this);
    //dialog.
    if (dialog.exec() != QDialog::Accepted)
        return;
#else
    printer.setOutputFileName("./print.ps");
#endif

    QTextDocument * text = new QTextDocument(this)  ;
    QTextCursor dc(text);

    dc.insertText(trUtf8("Výkaz práce\n\n"));
    dc.insertText(QString("%3 %4 \n%1 %2").
                  arg(PlonkDay->datum.shortMonthName(PlonkDay->datum.month())).
            arg(PlonkDay->datum.year()).
            arg(tool->ui->editName->text()).
                  arg(tool->ui->spinCislo->value()));

    QTextTable * table = dc.insertTable(1,3);

    //hlavička
    QTextCursor tc;
    tc = table->cellAt(0,0).firstCursorPosition();
    tc.insertText(trUtf8("Den"));
    tc = table->cellAt(0,1).firstCursorPosition();
    tc.insertText(trUtf8("Hlášení"));
    tc = table->cellAt(0,2).firstCursorPosition();
    tc.insertText(trUtf8("Hodiny"));

    foreach (ClassDay * day, PlonkMonth->GetDays()) {
        printDay(*day,table);
    }

    QTextTableFormat tab_format = table->format();
    tab_format.setCellPadding(5);
    tab_format.setCellSpacing(0);
    tab_format.setHeaderRowCount(1);
    tab_format.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
    tab_format.setBorder(0.5);
    table->setFrameFormat(tab_format);

    text->print(&printer);
}

void MainWindow::printDay(const ClassDay &day, QTextTable * table)
{
    if(!day.Prichod1.isValid())
        return;

    QTextCursor tc;

    foreach (ClassDay::prace_t * prace, day.GetPrace()) {
        int row = table->rows();
        table->appendRows(1);
        tc = table->cellAt(row,0).firstCursorPosition();
        tc.insertText(day.datum.toString(DATEFORMAT));
        tc = table->cellAt(row,1).firstCursorPosition();
        tc.insertText(prace->hlaseni);
        tc = table->cellAt(row,2).firstCursorPosition();
        tc.insertText(QString("%1").arg(prace->hodiny));
    }
}

void MainWindow::on_dovolenaChanged(int value)
{
    year->SetVolnaDovolena(value);
    fillForm();
}
