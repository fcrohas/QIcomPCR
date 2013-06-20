#include "CSoundStream.h"


CSoundStream::CSoundStream(QObject *parent) :
    QObject(parent)
{
    connect(&server, SIGNAL(newConnection()), this, SLOT(acceptConnection()));
    server.listen(QHostAddress::Any, 8889);
    connected = false;
#ifdef WITH_SPEEX
    byte_ptr = new char[MAX_NB_BYTES];
#endif
}

CSoundStream::~CSoundStream()
{
#ifdef WITH_SPEEX
    speex_bits_destroy(&bits);
    speex_encoder_destroy(enc_state);
    delete [] byte_ptr;
#endif
    server.close();
}

void CSoundStream::acceptConnection()
{
  client = server.nextPendingConnection();
  connect(client, SIGNAL(readyRead()), this, SLOT(startRead()));
  connected = true;
#ifdef WITH_SPEEX
    // Frame size for speex
    int frame_size = 0;
    int quality = 2; // Speex quality encoder
    int nbBytes = 0;
    // Speex initalization
    speex_bits_init(&bits);
    enc_state = speex_encoder_init(speex_lib_get_mode(SPEEX_MODEID_NB));
    speex_encoder_ctl(enc_state,SPEEX_SET_QUALITY,&quality);
    speex_encoder_ctl(enc_state,SPEEX_GET_FRAME_SIZE,&frame_size);
    qDebug() << "frame size for speex is " << frame_size;
#endif
}

void CSoundStream::startRead()
{
  char buffer[1024] = {0};
  if (connected)
    client->read(buffer, client->bytesAvailable());
}

void CSoundStream::encode(int16_t *data, int size)
{
#ifdef WITH_SPEEX
  if (connected) {
    speex_bits_reset(&bits);
    speex_encode_int(enc_state, data, &bits);
    nbBytes = speex_bits_write(&bits, byte_ptr, MAX_NB_BYTES);
    client->write(byte_ptr,nbBytes);
  }
#endif
}
