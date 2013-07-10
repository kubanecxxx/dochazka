#include "widgettoolbar.h"
#include "ui_widgettoolbar.h"
#include <QFile>
#include <QCoreApplication>
#include <QDir>

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
    QString cesta = QFileInfo( QCoreApplication::applicationFilePath() ).absolutePath();
    cesta += "/jmeno.txt";
    QFile fil(cesta);
    if (!fil.open(QFile::WriteOnly))
        return;

    fil.write(ui->editName->text().toUtf8() + '\n');
    fil.write(ui->spinCislo->text().toUtf8()+ '\n');
    fil.write(ui->spinDovolena->text().toUtf8()+ '\n');

    fil.close();
}

void WidgetToolBar::LoadDataFromFile()
{
    QString cesta = QFileInfo( QCoreApplication::applicationFilePath() ).absolutePath();
    cesta += "/jmeno.txt";
    QFile fil(cesta);
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
