#ifndef CDISPLAY_H
#define CDISPLAY_H

#include <QWidget>
#include <QPainter>

class CDisplay : public QWidget
{
    Q_OBJECT
public:
    explicit CDisplay(QWidget *parent = 0);
    ~CDisplay();

    // Frequency
    void setFrequency1(QString value);
    void setFrequency2(QString value);

    void setSignal1(int value);
    void setSignal2(int value);

    void paintEvent(QPaintEvent *event);

    
signals:
    
public slots:

private:

    QString frequency1;
    QString frequency2;
    int signal1;
    int signal2;

    // Frequency
    void drawFrequency(QPainter *p);

    // power
    void drawSignal(QPainter *p);
};

#endif // CDISPLAY_H
