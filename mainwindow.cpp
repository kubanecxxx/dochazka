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
#include <libxl.h>
#include <QFileDialog>

#define T_HODINY 0
#define T_MX 1
#define T_HLASENI 2
#define T_POZN 3
#define T_PRESCAS 4
#define T_NUCENO 5

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
    setSvatek(PlonkDay->svatek);
    setDovolena(PlonkDay->dovolena);
    fillFormDay(*PlonkDay);
    LoadCombos(*PlonkDay);
    fillFormMonth(*PlonkMonth);
    recolorCalendar(*PlonkMonth,PlonkDay->datum);

    if (PlonkDay->svatek || PlonkDay->dovolena)
        ui->table->setDisabled(true);
}

void MainWindow::setDovolena(bool enabled)
{
    if (enabled)
        ui->checkDovolena->setText(trUtf8("Ano"));
    else
        ui->checkDovolena->setText(trUtf8("Ne"));

    ui->checkDovolena->setChecked(enabled);
    ui->table->setDisabled(enabled);
}

void MainWindow::setSvatek(bool enabled)
{
    if (enabled)
        ui->checkSvatek->setText(trUtf8("Ano"));
    else
        ui->checkSvatek->setText(trUtf8("Ne"));

    ui->table->setDisabled(enabled);
    ui->checkSvatek->setChecked(enabled);
}

void MainWindow::fillFormDay(const ClassDay &day)
{
    ui->editPrichod1->setText(day.Prichod1.toString(TIMEFORMAT));
    ui->editPrichod2->setText(day.Prichod2.toString(TIMEFORMAT));
    ui->editOdchod1->setText(day.Odchod1.toString(TIMEFORMAT));
    ui->editOdchod2->setText(day.Odchod2.toString(TIMEFORMAT));
    ui->editRucne->setText(day.Rucne.toString(TIMEFORMAT));
    ui->editKorekce->setText(QString::number((double)day.Korekce));

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
        item = ui->table->item(i,T_HODINY);
        item->setData(Qt::DisplayRole,prac->hodiny);

        //mx
        item = ui->table->item(i,T_HLASENI);
        item->setData(Qt::DisplayRole,prac->hlaseni);
        ui->table->setItem(i,T_HLASENI,item);

        //poznamka
        item = ui->table->item(i,T_POZN);
        if (!item)
            item = new QTableWidgetItem;
        item->setText(prac->Poznamka);

        //prescas
        item = ui->table->item(i,T_PRESCAS);
        if (!item)
            item = new QTableWidgetItem;
        if (prac->prescas)
            item->setCheckState(Qt::Checked);
        else
            item->setCheckState(Qt::Unchecked);

        //nuceno
        item = ui->table->item(i,T_NUCENO);
        if (!item)
            item = new QTableWidgetItem;
        if (prac->nuceno)
        {
            item->setCheckState(Qt::Checked);
            item->setText(trUtf8("Nuceno"));
        }
        else
        {
            item->setCheckState(Qt::Unchecked);
            item->setText(trUtf8("Dohodnuto"));
        }

        //mx/mp
        item = ui->table->item(i,T_MX);
        if(!item)
            item = new QTableWidgetItem;
        if(prac->mx)
        {
            item->setCheckState(Qt::Checked);
            item->setText("MX");
        }
        else
        {
            item->setCheckState(Qt::Unchecked);
            item->setText("MP");
        }

        connect(ui->table,SIGNAL(itemChanged(QTableWidgetItem*)),
                   this,SLOT(on_table_itemChanged(QTableWidgetItem*)));
        i++;
    }
    AddRow();
    //LoadCombos();
}

void MainWindow::on_actionSave_triggered()
{
    year->SaveXml();
    //year->SaveFile();
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

    if (ui->table->item(row,T_HODINY) )
        prace->hodiny = ui->table->item(row,T_HODINY)->data(Qt::DisplayRole).toFloat();
    if (ui->table->item(row,T_HLASENI))
        prace->hlaseni = ui->table->item(row,T_HLASENI)->data(Qt::DisplayRole).toInt();
    if (ui->table->item(row,T_POZN))
        prace->Poznamka = ui->table->item(row,T_POZN)->text().trimmed();
    if (ui->table->item(row,T_PRESCAS))
    {
        bool jo;
        if (ui->table->item(row,T_PRESCAS)->checkState() == Qt::Checked)
            jo = true;
        else
            jo = false;
        prace->prescas = jo;
    }
    if (ui->table->item(row,T_NUCENO))
    {
        bool jo;
        if (ui->table->item(row,T_NUCENO)->checkState() == Qt::Checked)
        {
            jo = true;
        }
        else
        {
            jo = false;
        }
        prace->nuceno = jo;
    }
    if (ui->table->item(row,T_MX))
    {
        bool jo;
        if (ui->table->item(row,T_MX)->checkState() == Qt::Checked)
        {
            jo = true;
        }
        else
        {
            jo = false;
        }
        prace->mx = jo;
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
    ui->table->setItem(i,T_HODINY,item);

    //mpú
    item = new QTableWidgetItem;
    ui->table->setItem(i,T_HLASENI,item);

    //poznamka
    item = new QTableWidgetItem;
    ui->table->setItem(i,T_POZN,item);

    //mx/mp
    item = new QTableWidgetItem;
    ui->table->setItem(i,T_MX,item);
    item->setCheckState(Qt::Unchecked);
    item->setText("MX");

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
    ui->table->setItem(i,T_PRESCAS,item);

    //nuceno
    item = new QTableWidgetItem;

    item->setCheckState(Qt::Checked);
    item->setText(trUtf8("Nuceno"));
    ui->table->setItem(i,T_NUCENO,item);



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
    fillFormMonth(*PlonkMonth);
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

void MainWindow::on_actionPrint_triggered()
{
    QPrinter printer(QPrinter::PrinterResolution);
#ifndef QT_DEBUG
    QPrintDialog dialog(&printer, this);
    //dialog.
    if (dialog.exec() != QDialog::Accepted)
        return;
#else
    printer.setOutputFileName("./print.pdf");
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
        QString mx;
        if (prace->mx)
            mx = "MX ";
        else
            mx = "MP ";
        tc.insertText(QString("%2%1").arg(prace->hlaseni).arg(mx));
        tc = table->cellAt(row,2).firstCursorPosition();
        tc.insertText(QString("%1").arg(prace->hodiny));
    }
}

void MainWindow::on_dovolenaChanged(int value)
{
    year->SetVolnaDovolena(value);
    fillForm();
}

void MainWindow::on_actionTiskJirasko_triggered()
{
    QPrinter printer(QPrinter::PrinterResolution);
#ifndef QT_DEBUG
    QPrintDialog dialog(&printer, this);
    //dialog.
    if (dialog.exec() != QDialog::Accepted)
        return;
#else
    printer.setOutputFileName("./prescasy.ps");
#endif

    QTextDocument * text = new QTextDocument(this)  ;
    QTextCursor dc(text);


    QTextCharFormat fmt = dc.charFormat();
    fmt.setFontUnderline(true);
    dc.setCharFormat(fmt);
    dc.insertText(trUtf8("Povolení výkonu práce přesčas"));

    fmt.setFontUnderline(false);
    dc.setCharFormat(fmt);
    dc.insertText(trUtf8("\nMěsíc")+QString(" / rok      %1 / %2\n\n").
                  arg(PlonkDay->datum.month()).
            arg(PlonkDay->datum.year()));

    QTextTable * table = dc.insertTable(4,4);

    //hlavička
    QTextCursor tc;
    tc = table->cellAt(0,0).firstCursorPosition();
    tc.insertText(trUtf8("Organizační útvar:"));
    tc = table->cellAt(0,1).firstCursorPosition();
    tc.insertText(trUtf8("FM"));

    tc = table->cellAt(1,0).firstCursorPosition();
    tc.insertText(trUtf8("Příjmení a jméno zaměstnance"));
    tc = table->cellAt(1,1).firstCursorPosition();
    tc.insertText(tool->ui->editName->text());

    tc = table->cellAt(2,0).firstCursorPosition();
    tc.insertText(trUtf8("Příjmení a jméno vedoucího \n(řídící úroveň č.4)"));
    tc = table->cellAt(2,1).firstCursorPosition();
    tc.insertText(trUtf8("Ing. Jirásko Zdeněk"));

    tc = table->cellAt(0,2).firstCursorPosition();
    tc.insertText(trUtf8("Středisko"));
    tc = table->cellAt(0,3).firstCursorPosition();
    tc.insertText(QString("M190"));

    tc = table->cellAt(1,2).firstCursorPosition();
    tc.insertText(trUtf8("Osobní číslo"));
    tc = table->cellAt(1,3).firstCursorPosition();
    tc.insertText(QString("%1").arg(tool->ui->spinCislo->value()));

    QTextTableFormat tab_format = table->format();
    tab_format.setCellPadding(5);
    tab_format.setCellSpacing(0);
    tab_format.setHeaderRowCount(1);
    QBrush blackBrush(Qt::SolidPattern);
    tab_format.setBorderBrush(blackBrush);
    tab_format.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
    tab_format.setBorder(0);
    table->setFrameFormat(tab_format);

    dc.setPosition(table->lastPosition()+1);

    //zbytek
    table = dc.insertTable(1,5);
    tab_format = table->format();
    tab_format.setCellPadding(5);
    tab_format.setCellSpacing(0);
    tab_format.setHeaderRowCount(1);
    tab_format.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
    tab_format.setBorder(0.5);
    table->setFrameFormat(tab_format);

    {
    QTextCursor tc;
    tc = table->cellAt(0,0).firstCursorPosition();
    QTextBlockFormat fmt = tc.blockFormat();
    //fmt.setAlignment(Qt::AlignVCenter);
    tc.setBlockFormat(fmt);
    tc.insertText(trUtf8("Druh \npřesčasové \npráce"));

    tc = table->cellAt(0,1).firstCursorPosition();
    tc.insertText(trUtf8("Datum \nvýkonu \npřesčasové \npráce"));
    tc = table->cellAt(0,2).firstCursorPosition();
    tc.insertText(trUtf8("Počet \nhodin"));
    tc = table->cellAt(0,3).firstCursorPosition();
    tc.insertText(trUtf8("Úkoly, na kterých bude zaměstnanec pracovat"));
    tc = table->cellAt(0,4).firstCursorPosition();
    tc.insertText(trUtf8("Podpis \nvedoucího"));

    }

    foreach (ClassDay * day, PlonkMonth->GetDays()) {
        printDayPrescas(*day,table);
    }

    dc.setPosition(table->lastPosition()+ 1);
    dc.insertText(trUtf8("\nVšechny výše uvedené přesčasové hodiny požaduji proplatit"));

    text->print(&printer);
}

void MainWindow::printDayPrescas(const ClassDay &day, QTextTable *table)
{
    if(!day.Prichod1.isValid())
        return;

    QTextCursor tc;

    foreach (ClassDay::prace_t * prace, day.GetPrace()) {
        if (!prace->prescas)
            continue;
        int row = table->rows();
        table->appendRows(1);
        tc = table->cellAt(row,0).firstCursorPosition();
        QString text;
        if (prace->nuceno)
            text = "N";
        else
            text = "D";
        tc.insertText(text);

        tc = table->cellAt(row,1).firstCursorPosition();
        tc.insertText(day.datum.toString("dd.MM."));

        tc = table->cellAt(row,2).firstCursorPosition();
        tc.insertText(QString("%1").arg(prace->hodiny));

        tc = table->cellAt(row,3).firstCursorPosition();
        tc.insertText(prace->Poznamka);
    }
}

void MainWindow::on_editKorekce_textEdited(const QString &arg1)
{
    bool ok;
    float co;

    if (arg1.endsWith(".") || arg1.endsWith(","))
        return;

    co = arg1.toFloat(&ok);


    if (arg1.isEmpty())
    {
        co = 0;
        ok = true;
    }

    QLineEdit * edit = qobject_cast<QLineEdit*>(sender());

    QPalette pal;
    QColor col;

    if (ok)
    {
        col = Qt::black;
        PlonkDay->Korekce = co;
        fillForm();
    }
    else
    {
        col = Qt::red;
    }

    pal.setColor(QPalette::Text,col);
    edit->setPalette(pal);
    //fillForm();
}

void MainWindow::on_editRucne_textEdited(const QString &arg1)
{
    SetTime(arg1,PlonkDay->Rucne,sender());
}

using namespace libxl;

#ifdef __unix
#define OFFSET 1
#else
#define OFFSET 1
#endif

void MainWindow::on_actionFrankova_triggered()
{
    Book * book = xlCreateBook();
    Sheet * sheet = book->addSheet("Formulář hlášení");

    //hlavička
    sheet->writeStr(OFFSET,0,"Číslo hlášení");
    sheet->writeStr(OFFSET,1,"Osobní číslo");
    sheet->writeStr(OFFSET,2,"Odpracovaný čas");

    int i = OFFSET + 1;

    //musim projit všechny práce co existujou a sečist jejich hodiny pokud je nějaká práce stejná
    QList<ClassDay::prace_t> vse;
    foreach (ClassDay * day, PlonkMonth->GetDays())
    {
        foreach(ClassDay::prace_t * prace, day->GetPrace())
        {
            if (ClassDay::praceValid(prace))
            {
                vse.push_back(*prace);
            }
        }
    }

    //protřidit porovnat každou práci s každou
    for (int i = 0 ; i < vse.count(); i++)
    {
        for (int j = 0 ; j< vse.count(); j++)
        {
            if (vse[i] == vse[j] && i != j)
            {
                ClassDay::prace_t pr = vse.takeAt(j);
                vse[i].hodiny += pr.hodiny;
                j--;
            }
        }
    }

    //vytisknout
    foreach(ClassDay::prace_t pr,vse)
    {
        ClassDay::prace_t * prace = &pr;
        sheet->writeNum(i,0,prace->hlaseni);
        sheet->writeNum(i,1,tool->ui->spinCislo->value());
        sheet->writeNum(i,2,prace->hodiny);
        i++;
    }

    QString file = QFileDialog::getSaveFileName(this,trUtf8("Uložit"),QString(),QString("*.xls"));
    if (file.isEmpty())
        return;

    if (!file.endsWith(".xls"))
        file.append(".xls");
    char jo[200];
    strcpy(jo,file.toUtf8().data());
    book->save(jo);
}

void MainWindow::on_checkSvatek_clicked(bool checked)
{
    PlonkDay->svatek = checked;
    fillForm();
}
