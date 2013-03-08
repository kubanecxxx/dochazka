#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "classday.h"

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

private:
    Ui::MainWindow *ui;

    void fillFormDay(const ClassDay & day);
    void AddRow();

    ClassDay day1;
    ClassDay * PlonkDay;

    void SetTime(const QString & arg, QTime & time,QObject * widget);
};

#endif // MAINWINDOW_H
