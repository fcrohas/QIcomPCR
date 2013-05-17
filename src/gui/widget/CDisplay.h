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

    void setIF1(int value);
    void setIF2(int value);

    void paintEvent(QPaintEvent *event);

    
signals:
    
public slots:

private:

    QString frequency1;
    QString frequency2;
    int signal1;
    int signal2;
    int IF1;
    int IF2;

    // Frequency
    void drawFrequency(QPainter *p);

    // power
    void drawSignal(QPainter *p);

    // IF
    void drawIF(QPainter *p);
};

#endif // CDISPLAY_H
