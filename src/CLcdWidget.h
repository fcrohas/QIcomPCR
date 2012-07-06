#ifndef CLCDWIDGET_H
#define CLCDWIDGET_H

#include <QWidget>
#include <QHBoxLayout>

class CLcdWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CLcdWidget(QWidget *parent = 0);
    void setFrequency(QString value);
    void setIncrement(int value);
    void drawFrequency();
    
signals:
    void frequencyChanged(QString&);
    
public slots:

private:
    QHBoxLayout *layout;
    QString frequency;
    bool keyEnterPressed;
    // ts
    int ts;
    // Format frequency for human readable
    QString format(QString &value, int increment);

protected:
    void paintEvent(QPaintEvent *event);
    void keyPressEvent ( QKeyEvent * event );
    void mousePressEvent (QMouseEvent * event);
    void wheelEvent(QWheelEvent *event);
};


#endif // CLCDWIDGET_H
