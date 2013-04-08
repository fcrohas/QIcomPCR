#include "CSoundFile.h"

CSoundFile::CSoundFile(QObject *parent) :
    ISound(parent),
    samplerate(0),
    channels(0),
    frames(0),
    running(false),
    loop(false)
{
}

CSoundFile::~CSoundFile()
{

    if (inputbuffer)
        delete inputbuffer;

}

bool CSoundFile::Load(QString &fileName)
{
    bool loaded = false;

    // Sound file info struct
    SF_INFO sfinfo;
    memset(&sfinfo, 0, sizeof(SF_INFO));

    //Load file
    pFile = sf_open(fileName.toLocal8Bit().data(), SFM_READ, &sfinfo);
    if (pFile != NULL)
    {
        channels = sfinfo.channels;
        samplerate = sfinfo.samplerate;
        frames = sfinfo.frames;
        int oversample_factor = SAMPLERATE / samplerate;
        /* we can only cope with integer submultiples */
        if(SAMPLERATE != oversample_factor*samplerate)
            qDebug() << "unsupported sample rate in input file";

        // Init buffer size to read
        inputbuffer = new int16_t[frames];

        loaded=true;
    } else
            loaded = false;

    return loaded;
}

bool CSoundFile::Read(int16_t *data, int offset)
{
    sf_count_t c = sf_readf_short(pFile, inputbuffer, BUFFER_SIZE);
    if (loop) {
        /* rewind */
        sf_seek(pFile, 0, SEEK_SET);
        c = sf_readf_short(pFile, inputbuffer, BUFFER_SIZE);
        loop = false;
    }
    size_t oversample_factor = SAMPLERATE / samplerate;
    for (size_t i = 0; i < (BUFFER_SIZE)/oversample_factor; i++)
    {
        for (size_t j = 0; j < oversample_factor; j++)
        {
            data[2*i+j] = inputbuffer[2*i];
            if (channels == 2)
                data[2*i+1+j] = inputbuffer[2*i+1];
            else
                data[2*i+1+j] = inputbuffer[i];
        }
    }
    this->DecodeBuffer(data,BUFFER_SIZE);
    msleep(100); // Give how much millisecond we wait before next salve of BUFFER_SIZE/2 samples per channels
    return true;
}

void CSoundFile::DecodeBuffer(int16_t *buffer, int size)
{
    ISound::DecodeBuffer(buffer,size);
}

void CSoundFile::setRunning(bool value)
{
    running = true;
}

void CSoundFile::run()
{
    // Allocate data file for decoder
    int16_t *data = new int16_t[BUFFER_SIZE];
    while(running) {
        // Split reading into chunk of buffer size
        // to simulate sound card reading
        // and better
        for(int i=0; i< frames; i+=BUFFER_SIZE)
        {
            Read(data,i);
        }
        loop=true;
    }
}


void CSoundFile::terminate()
{
    if(pFile==NULL)
        return;
    sf_close(pFile);
    pFile = NULL;

}
