#include "signalselector.h"

#include "ui_signalselector.h"

#include "dbc/dbchandler.h"
#include "utility.h"

SignalSelector::SignalSelector(QWidget *parent)
    : QWidget{parent}
    , ui(new Ui::SignalSelector)
{
    ui->setupUi(this);

    connect(ui->cbNodes, SIGNAL(currentIndexChanged(int)), this, SLOT(loadMessages(int)));
    connect(ui->cbMessages, SIGNAL(currentIndexChanged(int)), this, SLOT(loadSignals(int)));
    connect(ui->cbSignals, SIGNAL(currentIndexChanged(int)), this, SLOT(selectedSignalChanged(int)));
    connect(ui->confirmButton, SIGNAL(clicked(bool)), this, SLOT(confirmSelectedSignal()));

    loadNodes();
}

SignalSelector::~SignalSelector()
{
    delete ui;
}

DBC_SIGNAL* SignalSelector::getSelectedSignal() const
{
    DBCHandler* dbcHandler = DBCHandler::getReference();
    if (dbcHandler == nullptr)
    {
        Q_ASSERT_X(false, "SignalSelector", "DBCHandler::getReference() returned a nullptr.");
        return nullptr;
    }

    const QString selectedNodeName = ui->cbNodes->currentText();
    const QString selectedMessageName = ui->cbMessages->currentText();

    const DBC_MESSAGE* message = dbcHandler->findMessage(selectedMessageName, selectedNodeName);
    if (message == nullptr)
    {
        return nullptr;
    }

    DBCSignalHandler* signalHandler = message->sigHandler;
    if (signalHandler == nullptr)
    {
        Q_ASSERT_X(false, "SignalSelector", "DBC_MESSAGE have a null DBCSignalHandler.");
        return nullptr;
    }

    const QString selectedSignalName = ui->cbSignals->currentText();

    return signalHandler->findSignalByName(selectedSignalName);
}

void SignalSelector::setSelectedSignal(const DBC_SIGNAL* signal)
{
    if (signal == nullptr)
        return;

    const DBC_MESSAGE* message = signal->parentMessage;
    const DBC_NODE* node = message->sender;

    bool isNodeFound = false;
    for (int n = 0; n < ui->cbNodes->count(); n++)
    {
        if (ui->cbNodes->itemText(n) == node->getFullyQualifiedNodeName())
        {
            ui->cbNodes->setCurrentIndex(n);
            isNodeFound = true;
            break;
        }
    }

    if (!isNodeFound)
        return;

    bool isMessageFound = false;
    for (int m = 0; m < ui->cbMessages->count(); m++)
    {
        if (ui->cbMessages->itemText(m) == message->name)
        {
            ui->cbMessages->setCurrentIndex(m);
            isMessageFound = true;
            break;
        }
    }

    if (!isMessageFound)
        return;

    for (int s = 0; s < ui->cbSignals->count(); s++)
    {
        if (ui->cbSignals->itemText(s) == signal->name)
        {
            ui->cbSignals->setCurrentIndex(s);
            break;
        }
    }
}

void SignalSelector::setConfirmButtonText(const QString& text)
{
    ui->confirmButton->setText(text);
}

void SignalSelector::loadNodes()
{
    DBCHandler* dbcHandler = DBCHandler::getReference();
    if (dbcHandler == nullptr)
    {
        Q_ASSERT_X(false, "SignalSelector", "DBCHandler::getReference() returned a nullptr.");
        return;
    }

    int dbcFileCount = dbcHandler->getFileCount();
    if (dbcFileCount == 0)
        return;

    ui->cbNodes->clear();
    ui->cbMessages->clear();
    ui->cbSignals->clear();

    for (int f = 0; f < dbcFileCount; f++)
    {
        const DBCFile* file = dbcHandler->getFileByIdx(f);
        if (file == nullptr)
        {
            Q_ASSERT_X(false, "SignalSelector", "DBCFile from DBCHandler is null.");
            continue;
        }

        DBCMessageHandler* messageHandler = file->messageHandler;
        if (messageHandler == nullptr)
        {
            Q_ASSERT_X(false, "SignalSelector", "DBCMessageHandler from DBCFile is null.");
            continue;
        }

        QList<QString> names;

        for (int n = 0; n < file->dbc_nodes.count(); n++)
        {
            const DBC_NODE& node = file->dbc_nodes[n];

            bool nodeHasMessages = false;

            for (int m = 0; m < messageHandler->getCount(); m++)
            {
                const DBC_MESSAGE* message = messageHandler->findMsgByIdx(m);
                if (message == nullptr)
                {
                    Q_ASSERT_X(false, "SignalSelector", QString("DBCMessageHandler contained a null message at ID %1.").arg(m).toLatin1().constData());
                    continue;
                }

                if (message->sender == nullptr)
                {
                    Q_ASSERT_X(false, "SignalSelector", "sender in DBC_MESSAGE is null.");
                    continue;
                }

                if (message->sender->name == node.name)
                {
                    nodeHasMessages = true;
                    break;
                }
            }

            if (nodeHasMessages)
            {
                QString fullyQualifiedNodeName = node.getFullyQualifiedNodeName();
                names.append(fullyQualifiedNodeName);
            }
        }

        if (names.count() > 0)
        {
            // Add disabled choice with file name, to separate the nodes between different files.
            ui->cbNodes->addItem("----" + file->getFilename());
            Utility::SetComboBoxItemEnabled(ui->cbNodes, ui->cbNodes->count() - 1, false);

            names.sort();
            ui->cbNodes->addItems(names);
        }
    }
}

void SignalSelector::loadMessages(int selectedNodeIndex)
{
    DBCHandler* dbcHandler = DBCHandler::getReference();
    if (dbcHandler == nullptr)
    {
        Q_ASSERT_X(false, "SignalSelector", "DBCHandler::getReference() returned a nullptr.");
        return;
    }

    int dbcFileCount = dbcHandler->getFileCount();
    if (dbcFileCount == 0)
        return;

    ui->cbMessages->clear();
    ui->cbSignals->clear();

    const QString selectedNodeName = ui->cbNodes->currentText();

    QList<QString> names;

    for (int f = 0; f < dbcFileCount; f++)
    {
        const DBCFile* file = dbcHandler->getFileByIdx(f);
        if (file == nullptr)
        {
            Q_ASSERT_X(false, "SignalSelector", "DBCFile from DBCHandler is null.");
            continue;
        }

        DBCMessageHandler* messageHandler = file->messageHandler;
        if (messageHandler == nullptr)
        {
            Q_ASSERT_X(false, "SignalSelector", "DBCMessageHandler from DBCFile is null.");
            continue;
        }

        for (int m = 0; m < messageHandler->getCount(); m++)
        {
            const DBC_MESSAGE* message = messageHandler->findMsgByIdx(m);
            if (message == nullptr)
            {
                Q_ASSERT_X(false, "SignalSelector", QString("DBCMessageHandler contained a null message at ID %1.").arg(m).toLatin1().constData());
                continue;
            }

            if (message->sender == nullptr)
            {
                Q_ASSERT_X(false, "SignalSelector", "sender in DBC_MESSAGE is null.");
                continue;
            }

            const QString fullyQualifiedNodeName = message->sender->getFullyQualifiedNodeName();

            if (fullyQualifiedNodeName == selectedNodeName && !names.contains(message->name))
            {
                names.append(message->name);
            }
        }
    }

    if (names.count() > 0)
    {
        names.sort();
        ui->cbMessages->addItems(names);
    }
}

void SignalSelector::loadSignals(int selectedMessageIndex)
{
    DBCHandler* dbcHandler = DBCHandler::getReference();
    if (dbcHandler == nullptr)
    {
        Q_ASSERT_X(false, "SignalSelector", "DBCHandler::getReference() returned a nullptr.");
        return;
    }

    ui->cbSignals->clear();

    const QString selectedNodeName = ui->cbNodes->currentText();
    const QString selectedMessageName = ui->cbMessages->currentText();

    // Search through all DBC files in order to try to find a message with the given name
    const DBC_MESSAGE* message = dbcHandler->findMessage(selectedMessageName, selectedNodeName);
    if (message == nullptr)
    {
        return;
    }

    DBCSignalHandler* signalHandler = message->sigHandler;
    if (signalHandler == nullptr)
    {
        Q_ASSERT_X(false, "SignalSelector", "DBC_MESSAGE have a null DBCSignalHandler.");
        return;
    }

    QList<QString> names;

    for (int s = 0; s < signalHandler->getCount(); s++)
    {
        const DBC_SIGNAL* signal = signalHandler->findSignalByIdx(s);
        if (signal == nullptr)
        {
            Q_ASSERT_X(false, "SignalSelector", QString("DBCSignalHandler contained a null signal at ID %1.").arg(s).toLatin1().constData());
            continue;
        }

        names.append(signal->name);
    }

    if (names.count() > 0)
    {
        names.sort();
        ui->cbSignals->addItems(names);
    }
}

void SignalSelector::selectedSignalChanged(int selectedSignalIndex)
{
    Q_UNUSED(selectedSignalIndex);

    const DBC_SIGNAL* signal = getSelectedSignal();
    ui->confirmButton->setEnabled(signal != nullptr);

    emit onSelectedSignalChange(signal);
}

void SignalSelector::confirmSelectedSignal()
{
    DBC_SIGNAL* signal = getSelectedSignal();
    if (signal == nullptr)
    {
        return;
    }

    emit onConfirm(signal);
}
