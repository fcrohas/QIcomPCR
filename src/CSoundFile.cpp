#include "CSoundFile.h"

CSoundFile::CSoundFile(QObject *parent) :
    ISound(parent),
    samplerate(0),
    channels(0),
    frames(0),
    running(false),
    loop(false),
    blankCount(0)
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
        qDebug() << "Information channels count is " << channels << " samplerate is " << samplerate << " frames count is " << frames;
        int oversample_factor = SAMPLERATE / samplerate;
        /* we can only cope with integer submultiples */
        if(SAMPLERATE != oversample_factor*samplerate)
            qDebug() << "unsupported sample rate in input file";

        // Init buffer size to read
        inputbuffer = new int16_t[BUFFER_SIZE];
        memset(inputbuffer,0,BUFFER_SIZE);
        loaded=true;
    } else
            loaded = false;

    return loaded;
}

bool CSoundFile::Read(int16_t *data, int offset)
{
    sf_count_t c;
    c = sf_readf_short(pFile, inputbuffer, BUFFER_SIZE/channels);
    if (c!=BUFFER_SIZE/channels) {
        /* rewind */
        sf_seek(pFile, 0, SEEK_SET);
        // At end of file send an empty buffer before next
        c = sf_readf_short(pFile, inputbuffer, BUFFER_SIZE/channels);
        // End of read
        loop = true;
    }
    size_t oversample_factor = SAMPLERATE / samplerate;
    if(channels==2)
    {
        for (size_t i = 0; i < BUFFER_SIZE/oversample_factor; i++)
        {
            for (size_t j = 0; j < oversample_factor; j++)
            {
                data[i+j] = inputbuffer[i];
                data[i+1+j] = inputbuffer[i+1];
            }
        }
    }
    else
    {
        for (size_t i = 0; i < BUFFER_SIZE/oversample_factor; i++)
        {
            for (size_t j = 0; j < oversample_factor; j++)
            {
                data[i+j] = inputbuffer[i];
                data[i+1+j] = inputbuffer[i];
            }
        }
    }

    this->DecodeBuffer(data,BUFFER_SIZE);
    msleep(50); // Give how much millisecond we wait before next salve of BUFFER_SIZE/2 samples per channels
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
    blankCount = 0;
    while(running) {
        // Split reading into chunk of buffer size
        // to simulate sound card reading
        // and better
        int i=0;
        // Read all buffer
        Read(data,i);

        // If reach end of buffer, do some blank before next loop
        while (loop) {
            memset(inputbuffer,0,BUFFER_SIZE);
            blankCount++;
            if (blankCount ==16) {
                loop=false;
                blankCount = 0;
            }
            this->DecodeBuffer(data,BUFFER_SIZE);
            msleep(50);

        }
    }
}


void CSoundFile::terminate()
{
    if(pFile==NULL)
        return;
    sf_close(pFile);
    pFile = NULL;

}

void CSoundFile::Record(QString &filename, bool start)
{

}
