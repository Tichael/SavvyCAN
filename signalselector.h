#ifndef SIGNALSELECTOR_H
#define SIGNALSELECTOR_H

#include <QWidget>

namespace Ui {
class SignalSelector;
}

class DBC_SIGNAL;

class SignalSelector : public QWidget
{
    Q_OBJECT

public:
    explicit SignalSelector(QWidget *parent = nullptr);
    ~SignalSelector();

    DBC_SIGNAL* getSelectedSignal() const;
    void setSelectedSignal(const DBC_SIGNAL* signal);

    void setConfirmButtonText(const QString& text);

signals:
    void onSelectedSignalChange(const DBC_SIGNAL* signal);
    void onConfirm(DBC_SIGNAL* signal);

private slots:
    void loadMessages(int selectedNodeIndex);
    void loadSignals(int selectedMessageIndex);
    void selectedSignalChanged(int selectedSignalIndex);
    void confirmSelectedSignal();

private:
    void loadNodes();

    Ui::SignalSelector* ui;
};

#endif // SIGNALSELECTOR_H
