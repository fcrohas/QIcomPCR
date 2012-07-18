#ifndef CPOGSAG_H
#define CPOGSAG_H

/* ---------------------------------------------------------------------- */

#define CHARSET_LATIN1

#include <QObject>

class CPogsag : public QObject
{
    Q_OBJECT
public:
    explicit CPogsag(QObject *parent = 0);
signals:
    
public slots:

private:
    unsigned char service_mask = 0x87;
};

#endif // CPOGSAG_H
