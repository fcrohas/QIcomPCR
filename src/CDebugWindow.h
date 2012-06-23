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
    
signals:
    void sendData(QString &data);
public slots:
    void slotDebugSerial(QString data);
    void slotSendSerial();
private:
    Ui::MainWindow *ui;
};

#endif // CDEBUGWINDOW_H
