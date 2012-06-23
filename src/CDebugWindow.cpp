#include "CDebugWindow.h"

CDebugWindow::CDebugWindow(QObject *parent, Ui::MainWindow *gui) :
    QObject(parent)
{
    ui = gui;
    connect(ui->pushSend, SIGNAL(clicked()), this, SLOT(slotSendSerial()));
}

void CDebugWindow::slotDebugSerial(QString data)
{
    ui->serialOutput->insertPlainText(data+"\n");
}


void CDebugWindow::slotSendSerial()
{
    QString data(ui->serialInput->text());
    ui->serialOutput->insertPlainText(data+"\n");
    ui->serialInput->clear();
    emit sendData(data);
}
