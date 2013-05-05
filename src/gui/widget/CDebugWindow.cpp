#include "CDebugWindow.h"
#include <QScrollBar>

CDebugWindow::CDebugWindow(QObject *parent, Ui::MainWindow *gui) :
    QObject(parent)
{
    ui = gui;
    connect(ui->pushSend, SIGNAL(clicked()), this, SLOT(slotSendSerial()));
    ui->serialOutput->setReadOnly(true);
}

void CDebugWindow::writeConsole(QString data)
{
    ui->serialOutput->insertPlainText(data+"\n");
    QScrollBar *sb = ui->serialOutput->verticalScrollBar();
    sb->setValue(sb->maximum());

}

void CDebugWindow::slotDebugSerial(QString data)
{
    ui->serialOutput->insertPlainText(data+"\n");
    QScrollBar *sb = ui->serialOutput->verticalScrollBar();
    sb->setValue(sb->maximum());
}


void CDebugWindow::slotSendSerial()
{
    QString data(ui->serialInput->text());
    ui->serialOutput->insertPlainText(data+"\n");
    ui->serialInput->clear();
    emit sendData(data);
}

void CDebugWindow::slotSendSerial(QString data)
{
    ui->serialOutput->insertPlainText(data+"\n");
    ui->serialInput->clear();
    emit sendData(data);
}
