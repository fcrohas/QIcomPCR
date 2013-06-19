#ifndef CSOUNDSTREAM_H
#define CSOUNDSTREAM_H

#include "ISound.h"
#include "consts.h"

class CSoundStream : public ISound
{
public:
    CSoundStream(QObject *parent = 0);
    ~CSoundStream();
};

#endif // CSOUNDSTREAM_H
