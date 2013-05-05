#ifndef CSTATUSWIDGET_H
#define CSTATUSWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPixmap>
#include <QLayout>

class CStatusWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CStatusWidget(QWidget *parent = 0);
    void setState(bool value);
    
signals:

protected:
    void paintEvent(QPaintEvent *event);
public slots:
    void slotUpdate(QString value);

private:
    QLabel status;
    QWidget led;
    QHBoxLayout *layout;
    bool state;

};
#endif // CSTATUSWIDGET_H

