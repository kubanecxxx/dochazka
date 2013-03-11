#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "classday.h"
#include "classmonth.h"
#include "classyear.h"

namespace Ui {
class MainWindow;
}

class QTableWidgetItem;

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

private:
    Ui::MainWindow *ui;

    void fillFormDay(const ClassDay & day);
    void fillFormDay() {fillFormDay(*PlonkDay);}
    void fillFormMonth(const ClassMonth & month);
    void fillFormMonth() {fillFormMonth(*PlonkMonth);}
    void AddRow();

    ClassDay * PlonkDay;
    ClassMonth * PlonkMonth;
    ClassYear * year;

    void SetTime(const QString & arg, QTime & time,QObject * widget);
    void LoadCombos(const ClassDay & day);
    void LoadCombos() {LoadCombos(*PlonkDay);}
};

#endif // MAINWINDOW_H
