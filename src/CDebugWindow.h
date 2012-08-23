#ifndef CDEBUGWINDOW_H
#define CDEBUGWINDOW_H

#include <QObject>
#include "ui_CMainWindow.h"

namespace Ui {
    class MainWindow;
}

class CDebugWindow : public QObject
{
    Q_OBJECT
public:
    explicit CDebugWindow(QObject *parent = 0, Ui::MainWindow *gui=0);

    void writeConsole(QString data);
signals:
    void sendData(QString &data);
public slots:
    void slotDebugSerial(QString data);
    // send
    void slotSendSerial();
    void slotSendSerial(QString data);
private:
    Ui::MainWindow *ui;
};

#endif // CDEBUGWINDOW_H
