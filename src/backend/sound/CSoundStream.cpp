#include "CSoundStream.h"


CSoundStream::CSoundStream(QObject *parent) :
    QObject(parent),
    client(NULL),
    frame_size(0),
    offset(1),
    lastpos(0)
#ifdef WITH_SPEEX
  ,enc_state(NULL)
#endif
{
    qDebug() << "Connect server socker event newConnection()";
    server = new QTcpServer(this);
    server->listen(QHostAddress::Any, 8889);
    //connect(server, SIGNAL(newConnection()), this, SLOT(acceptConnection()),Qt::DirectConnection);
    qDebug() << "Listener started";
    connected = false;
#ifdef WITH_SPEEX
    byte_ptr = new char[MAX_NB_BYTES];
    audior = new int16_t[MAX_NB_BYTES*2];
    audiol = new int16_t[MAX_NB_BYTES*2];
#endif
}

CSoundStream::~CSoundStream()
{
#ifdef WITH_SPEEX
    speex_bits_destroy(&bits);
    if (enc_state != NULL)
        speex_encoder_destroy(enc_state);
    delete [] byte_ptr;
    delete [] audiol;
    delete [] audior;
#endif
    server->close();
}

void CSoundStream::acceptConnection()
{
  // Wait for connection in each loop
  if ((!connected) && (server->waitForNewConnection(10))) {
      client = server->nextPendingConnection();
      qDebug() << "Connect server socker event acceptConnection()";
      connect(client, SIGNAL(readyRead()), this, SLOT(startRead()));
      connect(client,SIGNAL(disconnected()), this, SLOT(disconnected()));
      connected = true;
#ifdef WITH_SPEEX
        int quality = 8; // Speex quality encoder
        int complexity = 2; // Speex complexity encoder
        int samplerate = SAMPLERATE;
        int nbBytes = 0;
        int enhance = 1;
        // Speex initalization
        speex_bits_init(&bits);
        enc_state = speex_encoder_init(speex_lib_get_mode(SPEEX_MODEID_NB));
        speex_encoder_ctl(enc_state,SPEEX_SET_QUALITY,&quality);
        speex_encoder_ctl(enc_state,SPEEX_SET_SAMPLING_RATE,&samplerate);
        //speex_encoder_ctl(enc_state,SPEEX_SET_COMPLEXITY,&complexity);
        speex_encoder_ctl(enc_state,SPEEX_GET_FRAME_SIZE,&frame_size);
        speex_encoder_ctl(enc_state,SPEEX_SET_ENH,&enhance);
        qDebug() << "frame size for speex is " << frame_size;
#endif
  }
}

void CSoundStream::startRead()
{
  char buffer[1024] = {0};
  if (connected) {
    client->read(buffer, client->bytesAvailable());
  }
}

void CSoundStream::encode(int16_t *data, int size)
{
#ifdef WITH_SPEEX
  if (connected) {
    // reset offset buffer before processing
    // offset in buffer size at the moment
    // 256 / 160 = 1.6 so only one loop send and store the remaining bytes
    // if previous buffer
    int monopos = 0;
    int delta = 0;
    if ( lastpos > 0) {
        // copy current audio and process
        for (int i=0,j=0; i<lastpos+1;i+=2,j++) {
            // copy to mono buffer
            audiol[j] = audior[i];
            // save position to concat new before compress
            monopos = j;
        }
    }
    //qDebug() << "Remaining samples mono count from last call is " << monopos << " buffer stereo size is " << size;
    // if remaining is > frame_size then compres and send it before next salve
    // compress buffer
    while (monopos > frame_size) {
        speex_bits_reset(&bits);
        speex_encode_int(enc_state, audiol, &bits);
        nbBytes = speex_bits_write(&bits, byte_ptr, MAX_NB_BYTES);
        client->write(byte_ptr,nbBytes);
        // no Event loop so do it manually
        client->flush();
        // Copy remaining
        delta = monopos - frame_size; // remaining sample on a mono buffer
        //qDebug() << "Remaining samples on processed audiol buffer  " << monopos-delta << " with a frame size " << frame_size;
        // copy not used
        for (int i=delta+1,j=0; i<size;i++,j++) {
            // copy to stereo buffer
            audiol[j] = audiol[i]; // copy end of buffer to next buffer
        }
        // new monopos and loop again if still another frame to process
        monopos = delta+1;
    }
    //qDebug() << "Remaining samples mono count after processed old buffer from last call is " << monopos << " buffer stereo size is " << size;
    // compress buffer
    speex_bits_reset(&bits);
    for (int i=0,j=monopos; i<size;i+=2,j++) {
        audiol[j] = data[i];
    }
    speex_encode_int(enc_state, audiol, &bits);
    nbBytes = speex_bits_write(&bits, byte_ptr, MAX_NB_BYTES);
    client->write(byte_ptr,nbBytes);
    // no Event loop so do it manually
    client->flush();
    delta = (frame_size-monopos)*2; // remaining sample on a stereo buffer
    //qDebug() << "Remaining samples on data buffer to save " << size-delta << " with a frame size " << frame_size;
    // copy not used
    for (int i=delta+1,j=0; i<size;i++,j++) {
        // copy to stereo buffer
        audior[j] = data[i]; // copy end of buffer to next buffer
        lastpos = j; // register how much bits leave in buffer to be processed
    }
    //qDebug() << "saved samples count for next call is " << lastpos;

  }
#endif
}

void CSoundStream::disconnected()
{
    qDebug() << "client disconnected";
    //client->close();
    connected = false;
}
