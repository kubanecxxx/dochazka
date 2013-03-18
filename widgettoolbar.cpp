#include "widgettoolbar.h"
#include "ui_widgettoolbar.h"
#include <QFile>

WidgetToolBar::WidgetToolBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetToolBar)
{
    ui->setupUi(this);
    LoadDataFromFile();

    connect(ui->spinDovolena,SIGNAL(valueChanged(int)),
            this,SIGNAL(DovolenaChanged(int)));
}

WidgetToolBar::~WidgetToolBar()
{
    delete ui;
}

void WidgetToolBar::SaveDataToFile()
{
    QFile fil("jmeno.txt");
    if (!fil.open(QFile::WriteOnly))
        return;

    fil.write(ui->editName->text().toUtf8() + '\n');
    fil.write(ui->spinCislo->text().toUtf8()+ '\n');
    fil.write(ui->spinDovolena->text().toUtf8()+ '\n');

    fil.close();
}

void WidgetToolBar::LoadDataFromFile()
{
    QFile fil("jmeno.txt");
    if (!fil.open(QFile::ReadOnly))
        return;

    QByteArray buffer;
    buffer = fil.readLine().trimmed();
    ui->editName->setText(QString::fromUtf8(buffer));
    buffer = fil.readLine().trimmed();
    bool ok;
    ui->spinCislo->setValue(buffer.toInt(&ok));
    Q_ASSERT(ok);
    buffer = fil.readLine().trimmed();
    ui->spinDovolena->setValue(buffer.toInt(&ok));
    Q_ASSERT(ok);

    fil.close();
}
