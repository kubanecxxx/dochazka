#include "mojetable.h"
#include <QKeyEvent>
#include <QTimer>

MojeTable::MojeTable(QWidget *parent) :
    QTableWidget(parent)
{
    timer = new QTimer;

    connect(timer,SIGNAL(timeout()),this,SLOT(timeout()));
}

void MojeTable::timeout()
{
    //přidat jeden řádek a vyhodit ho z clipboardu
    if (clipmirror.isEmpty())
    {
        clipmirror = clipboard;
        if (selectedItems().isEmpty())
        {
            timer->stop();
            clipmirror.clear();
        }
        int row = selectedItems().at(0)->row();
        sel = row - 1;
    }

    int row = ++sel;
    line_t t = clipmirror.takeFirst();

    item(row,T_HODINY)->setData(Qt::DisplayRole,t.hodiny);
    if (t.mx)
        item(row,T_MX)->setCheckState(Qt::Checked);
    else
        item(row,T_MX)->setCheckState(Qt::Unchecked);
    item(row,T_HLASENI)->setData(Qt::DisplayRole,t.hlaseni);
    item(row,T_POZN)->setText(t.poznamka);

    emit itemChanged(item(row,T_HODINY));

    if (clipmirror.isEmpty())
        timer->stop();
}

void MojeTable::keyPressEvent(QKeyEvent *event)
{
    QList<QTableWidgetItem *> selected(selectedItems());

    if (event->key() == Qt::Key_C && event->modifiers() == Qt::ControlModifier)
    {
        //vyplnit novo tabulku
        clipboard.clear();


        line_t line;
        int selectedRows = selected.count() / 6;
        int j = 0;

        for (int i = 0 ; i < selected.count(); i += selectedRows)
        {
            QTableWidgetItem * item = selected[i];
            if (item->column() == T_HODINY)
                line.hodiny = item->data(Qt::DisplayRole).toDouble();


            if (item->column() == T_MX)
            {
                if (item->checkState() == Qt::Checked)
                    line.mx = true;
                else
                    line.mx = false;
            }


            if (item->column() == T_HLASENI)
                line.hlaseni = item->data(Qt::DisplayRole).toInt();


            if (item->column() == T_POZN)
            {
                line.poznamka = item->text();
                clipboard.push_back(line);
                j++;
                if (j == selectedRows)
                    break;
                i = j - selectedRows;
            }
        }
    }
    else if (event->key() == Qt::Key_V && event->modifiers() == Qt::ControlModifier )
    {
        //přihodit do tabulky
        timer->start(50);
    }
}
