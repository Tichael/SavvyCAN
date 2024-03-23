#ifndef NEWGRAPHDIALOG_H
#define NEWGRAPHDIALOG_H

#include <QDialog>
#include "graphingwindow.h"
#include "dbc/dbchandler.h"

namespace Ui {
class NewGraphDialog;
}

class NewGraphDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewGraphDialog(DBCHandler *handler, QWidget *parent = 0);
    ~NewGraphDialog();
    void showEvent(QShowEvent*);
    void getParams(GraphParams &);
    void setParams(GraphParams &);
    void clearParams();

private slots:
    void addButtonClicked();
    void colorSwatchClick();
    void fillSwatchClick();
    void bitfieldClicked(int);
    void handleDataLenUpdate();
    void drawBitfield();
    void copySignalToParamsUI(DBC_SIGNAL* signal);

private:
    bool eventFilter(QObject *obj, QEvent *event);
    void checkSignalAgreement();

    Ui::NewGraphDialog *ui;
    DBCHandler *dbcHandler;
    DBC_SIGNAL *assocSignal;
    int startBit, dataLen;
    bool shownFromPlotEdit;
};

#endif // NEWGRAPHDIALOG_H
