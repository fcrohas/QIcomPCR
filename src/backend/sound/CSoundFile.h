#ifndef CSOUNDFILE_H
#define CSOUNDFILE_H
#include "ISound.h"
#include <sndfile.h>
#include <consts.h>
#ifdef WITH_SAMPLERATE
#include <samplerate.h>
#endif
#include "CSoundStream.h"
#define FRAME_SIZE 256

class CSoundFile : public ISound
{
    Q_OBJECT
public:
    explicit CSoundFile(QObject *parent = 0);
    ~CSoundFile();
    bool Load(QString &fileName);
    bool Read(int16_t *data, int offset);
    bool ReadOverSample(int16_t *data, int offset, double ratio);
    void setRunning(bool value) override;
    void DecodeBuffer(int16_t *buffer, int size);
    void Record(QString &filename, bool start);

private:
    SNDFILE*	pFile;
    int channels;
    int samplerate;
    sf_count_t frames;
    int16_t *inputbuffer;
    void run();
    void terminate();
    bool running;
    bool loop;
    int blankCount;
    int timing;
    CSoundStream *soundStream;
#ifdef WITH_SAMPLERATE
    float *inputbufferf;
    float *outputbufferf;
    // Sample rate conversion
    SRC_STATE* converter;
    SRC_DATA dataconv;
#endif

};

#endif // CSOUNDFILE_H
