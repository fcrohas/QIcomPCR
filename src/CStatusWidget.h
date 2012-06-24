#ifndef CSTATUSWIDGET_H
#define CSTATUSWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QLayout>

class CStatusWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CStatusWidget(QWidget *parent = 0);
    
signals:
    
public slots:
    void slotUpdate(QString value);

private:
    QLabel status;
    QHBoxLayout *layout;
};
#endif // CSTATUSWIDGET_H

