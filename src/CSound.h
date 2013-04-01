#ifndef CSOUND_H
#define CSOUND_H

#include <ISound.h>

class CSound : public ISound
{
    Q_OBJECT
public:
    explicit CSound(QObject *parent = 0);
    
signals:
    
public slots:
    
};

#endif // CSOUND_H
