#include "CAcars.h"
#include <QDebug>

CAcars::CAcars(QObject *parent, uint channel, int bufferSize, int sampleRate, int passes) :
    IDemodulator(parent)
{
    if (!(L = acarsd_init(bufferSize,sampleRate,passes))) {
        qDebug() << "acarsd_init() error";
        return;
    }

    /* Something about libacarsd */
    qDebug() <<  "Libacarsd version: " << QString("%1.%2Rev%3").arg(libacarsdmajorversion).arg(libacarsdminorversion).arg(libacarsdrevision);

    /* CRC/FCB Check is activ? */
    qDebug() <<  "CRC MODE IS ENABLED: " << QString("%1").arg(acarsd_option(L,ACARSD_GET|ACARSD_CRCMODE,0)?"YES":"NO");

    /* Disable CRC check */
    asetopt(L,ACARSD_CRCMODE,0);

    /* CRC/FCB Check is activ? */
    qDebug() <<  "CRC MODE IS ENABLED: " << QString("%1").arg(acarsd_option(L,ACARSD_GET|ACARSD_CRCMODE,0)?"YES":"NO");

    /* Set 10 passes on each buffer */
    asetopt(L,ACARSD_PASSES,passes);

    /* Use 9 codetables */
    asetopt(L,ACARSD_CODETABLES,9);

    this->channel = channel;

}

void CAcars::decode(uchar *buffer, int size, int offset)
{
    int i=0, j=0, succ=0, err= 0, num=0;
    char *a;
    /* Try to find all messages from soundfile */
    while (ACARS_Decoder(L,buffer)) {
        /* Get the best message */
        num = acarsd_goodoffset(L,&i);
        qDebug() << "Decode buffer offest num=" << num << "offset=" << i;
        qDebug() << "Volume info " << ACARS_VInfo(L);
        if (num != ACARSD_NONE) {
            if (!L->codeholder[i].crc) {
                qDebug() << "CHECKSUM VALIDATED ON MESSAGE";
                succ++;
            } else {
                if ((a = acarsd_errors_of_msg(L, i))) {
                    qDebug() << "Errors: " << a;
                    //printf("Errors: %s\n",a); free(a);
                }
                err++;
            }
            QString rawData("");
            for (j=0;j<=L->codeholder[i].len;j++) {
                if (L->codeholder[i].c[j].data < 32)
                {
                    //emit sendData(QString(spec[L->codeholder[i].c[j].data]));
                    rawData.append("<");
                    rawData.append(spec[L->codeholder[i].c[j].data]);
                    rawData.append(">");
                    //qDebug() << QString(spec[L->codeholder[i].c[j].data]);
                }
                else
                {
                    rawData.append(L->codeholder[i].c[j].data);
                    //qDebug() << QString(L->codeholder[i].c[j].data);
                }
            }
            emit sendData("----------- Raw Data -------------");
            emit sendData(rawData);
            emit sendData("----------- Human Data -----------");
            emit sendData(QString(acarsd_human_readable(L,1)));
            //qDebug() << "\n---\n";
        }
        /* Print the volume of the soundstream */

        qDebug() << QString("Volume: %1 / Buffers: %2").arg(agetopt(L,ACARSD_VOLUME)).arg(agetopt(L,ACARSD_BUFFERS));

        for (i=0;i<agetopt(L,ACARSD_CODETABLES);i++) {
            qDebug() << QString("Codetable #%1 - %2 good messages").arg(i+1).arg(L->acarsd_utable[i]);
        }

        qDebug() << QString("+----+----+\n| %1 | %2 |\n+----+----+\n").arg(succ).arg(err);
    }

}

uint CAcars::getDataSize()
{
    return 8;
}

uint CAcars::getChannel()
{
    return channel;
}

uint CAcars::getBufferSize()
{
    return ACARSBUFFER;
}
