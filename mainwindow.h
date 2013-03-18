#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "classday.h"
#include "classmonth.h"
#include "classyear.h"
#include "widgettoolbar.h"

namespace Ui {
class MainWindow;
}

class QTableWidgetItem;
class QTextTable;
class QLineEdit;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_actionSave_triggered();
    void on_actionPrehled_triggered();
    void on_table_itemChanged(QTableWidgetItem *item);
    void on_editOdchod2_textEdited(const QString &arg1);
    void on_editPrichod2_textEdited(const QString &arg1);
    void on_editOdchod1_textEdited(const QString &arg1);
    void on_editPrichod1_textEdited(const QString &arg1);
    void on_calendarWidget_clicked(const QDate &date);
    void on_checkDovolena_clicked(bool checked);
    void on_actionPrint_triggered();

    void on_dovolenaChanged(int value);

private:
    Ui::MainWindow *ui;

    void fillFormDay(const ClassDay & day);
    void fillFormMonth(ClassMonth & month);
    void recolorCalendar(ClassMonth & month,QDate date);
    void setDovolena(bool enabled);
    void AddRow();
    void printDay(const ClassDay & day, QTextTable * table);

    void fillForm(void);

    ClassDay * PlonkDay;
    ClassMonth * PlonkMonth;
    ClassYear * year;

    void SetTime(const QString & arg, QTime & time,QObject * widget);
    void LoadCombos(const ClassDay & day);

    WidgetToolBar * tool;
};

#endif // MAINWINDOW_H
