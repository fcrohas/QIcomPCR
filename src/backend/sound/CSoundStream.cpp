/**********************************************************************************************
  Copyright (C) 2012 Fabrice Crohas <fcrohas@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

**********************************************************************************************/
#include "CSoundStream.h"


CSoundStream::CSoundStream(QObject *parent) :
    QObject(parent),
    client(NULL),
    frame_size(0),
    offset(1),
    speexPos(0),
    speexSize(0),
    stereoPos(0),
    stereoSize(0),
    ringBufferData(NULL),
    channel(0)
#ifdef WITH_SPEEX
    ,enc_state(NULL),
    resample(false)
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
    stereoBuffer = new int16_t[MAX_NB_BYTES*2];
    speexBuffer = new int16_t[MAX_NB_BYTES*2];
    resampler = new int16_t[BUFFER_SIZE];
#endif
    // Initialize a ring buffer
    if (ringBufferData)
        delete[] ringBufferData;
    ringBufferData = new int16_t[524288];
    PaUtil_InitializeRingBuffer(&ringBuffer, sizeof(int16_t) , 524288, ringBufferData);
    // Init a resampler
    if (resample) {
        int speex_err = 0;
        mspeex_src_state = speex_resampler_init(2,SAMPLERATE,DOWNSAMPLE,5,&speex_err);
        qDebug() << "Speex resampler error : " << speex_resampler_strerror(speex_err);
    }
}

CSoundStream::~CSoundStream()
{
#ifdef WITH_SPEEX
    speex_bits_destroy(&bits);
    if (enc_state != NULL)
        speex_encoder_destroy(enc_state);
    delete [] byte_ptr;
    delete [] speexBuffer;
    delete [] stereoBuffer;
#endif
    server->close();
}

void CSoundStream::acceptConnection()
{
  // Wait for connection in each loop
  if ((!connected) && (server->waitForNewConnection(1000))) {
      client = server->nextPendingConnection();
      qDebug() << "Connect server socket event acceptConnection()";
      connect(client, SIGNAL(readyRead()), this, SLOT(startRead()));
      connect(client,SIGNAL(disconnected()), this, SLOT(disconnected()));
      connected = true;
#ifdef WITH_SPEEX
        int quality = 5; // Speex quality encoder
        int complexity = 2; // Speex complexity encoder
        int samplerate = 0;
        if (resample==true) {
            samplerate = DOWNSAMPLE;
        } else {
            samplerate = SAMPLERATE;
        }
        int nbBytes = 0;
        int vbr = 0;
        float vbrquality = 5.0;
        int bitrate = 0;
        int dtx = 1;
        // Speex initalization
        speex_bits_init(&bits);
        enc_state = speex_encoder_init(speex_lib_get_mode(SPEEX_MODEID_NB));
        speex_encoder_ctl(enc_state,SPEEX_SET_QUALITY,&quality);
        //speex_encoder_ctl(enc_state,SPEEX_SET_VBR,&vbr);
        speex_encoder_ctl(enc_state,SPEEX_SET_DTX,&dtx);
        speex_encoder_ctl(enc_state,SPEEX_SET_SAMPLING_RATE,&samplerate);
        speex_encoder_ctl(enc_state,SPEEX_SET_COMPLEXITY,&complexity);
        speex_encoder_ctl(enc_state,SPEEX_GET_FRAME_SIZE,&frame_size);
        speex_encoder_ctl(enc_state,SPEEX_GET_BITRATE,&bitrate);
        //speex_encoder_ctl(enc_state,SPEEX_SET_VBR_QUALITY,&vbrquality);
        qDebug() << "frame size for speex is " << frame_size << " bitrate " << bitrate;
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
    if ( speexSize > 0) {
        // Now if needed loop to sent chunk of frame_size
        while (speexSize >= frame_size) {
            // compress buffer
            speex_bits_reset(&bits);
            speex_encode_int(enc_state, speexBuffer, &bits);
            nbBytes = speex_bits_write(&bits, byte_ptr, MAX_NB_BYTES);
            client->write(byte_ptr,nbBytes);
            // no Event loop so do it manually
            client->flush();
            // new size
            // Copy remaining
            for (int i=frame_size,j=0; i<speexSize;i++,j++) {
                // copy to stereo buffer
                speexBuffer[j] = speexBuffer[i]; // remove latest frame_size sent from the buffer
                speexPos = j;
            }
            // block but should never append the case of speexPos = 0
            speexPos = speexPos + 1;
            speexSize = speexPos;
        }

    }
    speex_bits_reset(&bits);
    for (int i=0,j=speexPos; i<size;i+=2,j++) {
        speexBuffer[j] = data[i+channel];
        speexPos = j;
    }
    speexPos = speexPos + 1;  // newt sample will go there
    speexSize = speexPos; // size is +1 from last position
    // Encode and send
    int loop =0;
    while(loop < (size/2)/frame_size) { // do loop to compress max frame in one packet
        // compress buffer
        speex_encode_int(enc_state, speexBuffer, &bits);
        // copy from l
        for (int i=frame_size,j=0; i<speexSize;i++,j++) { // copy from end of frame to speex buffer size
            // copy to stereo buffer
            speexBuffer[j] = speexBuffer[i]; // remove latest frame_size sent from the buffer
            speexPos = j;
        }
        speexPos = speexPos + 1;  // next sample will go there
        speexSize = speexPos;
        speex_bits_insert_terminator(&bits);
        loop++;
    }
    nbBytes = speex_bits_write(&bits, byte_ptr, MAX_NB_BYTES);
    client->write(byte_ptr,nbBytes);
    // no Event loop so do it manually
    client->flush();
  }
#endif
}

void CSoundStream::disconnected()
{
    qDebug() << "client disconnected";
    //client->close();
    connected = false;
}

void CSoundStream::setData(int16_t *data, int size)
{
    // Write data to ringbuffer
    if (connected) {
        long avail = PaUtil_GetRingBufferWriteAvailable(&ringBuffer);
        if (resample) {
            spx_uint32_t buf_len = size;
            spx_uint32_t bufout_len;
            // other code eg. using bass to feed "buf" and set buf_len etc. but this part is correct.
            spx_uint32_t speex_err = 0;
            speex_err = speex_resampler_process_interleaved_int(mspeex_src_state,data,&buf_len,resampler,&bufout_len);
            //qDebug() << "output buffer size " << bufout_len;
            PaUtil_WriteRingBuffer(&ringBuffer, resampler, (avail<bufout_len)?avail:bufout_len);
        } else {
            PaUtil_WriteRingBuffer(&ringBuffer, data, (avail<size)?avail:size);
        }
    }
}

void CSoundStream::doWork()
{
    acceptConnection();
    if (connected) {
        // do some byte available for work ?
        int avail = PaUtil_GetRingBufferReadAvailable(&ringBuffer);
        // Encode only if enought is available
        if (avail > BUFFER_SIZE) {
            PaUtil_ReadRingBuffer(&ringBuffer,(void*)stereoBuffer,(avail<BUFFER_SIZE)?avail:BUFFER_SIZE);
            // Encode the data and send it
            encode(stereoBuffer,(avail<BUFFER_SIZE)?avail:BUFFER_SIZE);
        }
    }
}

void CSoundStream::setChannel(uint value)
{
    channel = value;
}
