#ifndef CSOUNDFILE_H
#define CSOUNDFILE_H

#include "ISound.h"

class CSoundFile : public ISound
{
    Q_OBJECT
public:
    explicit CSoundFile(QObject *parent = 0);
    ~CSoundFile();
};

#endif // CSOUNDFILE_H
