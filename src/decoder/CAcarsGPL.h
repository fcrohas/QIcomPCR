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
#ifndef CACARSGPL_H
#define CACARSGPL_H

#include <QObject>
#include "IDecoder.h"

#define Fe 22050.0
#define Freqh 4800.0/Fe*2.0*M_PI
#define Freql 2400.0/Fe*2.0*M_PI
#define VFOPLL 0.7e-3
#define BITPLL 0.2
#define BITLEN ((int)Fe/1200)
#define SYN 0x16
#define SOH 0x01
/* CCITT 16 CRC */
#define POLY 0x1021

class CAcarsGPL : public IDecoder
{
    Q_OBJECT
public:
    explicit CAcarsGPL(QObject *parent = 0, uint channel=1);
    void decode(int16_t *data, int size, int offset);
    uint getChannel();
    uint getDataSize();
    uint getBufferSize();
    
signals:
    void dumpData(double*,double*,int);
public slots:
    void slotFrequency(double value);

private:
    uint channel;
    // Structure
    typedef struct {
        unsigned char mode;
        unsigned char addr[8];
        unsigned char ack;
        unsigned char label[3];
        unsigned char bid;
        unsigned char no[5];
        unsigned char fid[7];
        char txt[256];
    } msg_t;

    enum state_e { HEADL, HEADF, BSYNC1, BSYNC2, SYN1, SYN2, SOH1, TXT, CRC1,
            CRC2, END };

    struct mstat_s {
        state_e state;
        int ind;
        unsigned short crc;
        char txt[243];
    } mstat[2];

    float h[BITLEN];

    struct bstat_s {
        float hsample[BITLEN];
        float lsample[BITLEN];
        float isample[BITLEN];
        float qsample[BITLEN];
        float csample[BITLEN];
        int is;
        int clock;
        float lin;
        float phih,phil;
        float dfh,dfl;
        float pC,ppC;
        int sgI, sgQ;
        float ea;
    } bstat[2];

    // Methode
    int getmesg(unsigned char r, msg_t * msg, int ch);
    void init_mesg(void);
    int build_mesg(char *txt, int len, msg_t * msg);
    void update_crc(unsigned short *crc, unsigned char ch);
    int getbit(short sample, unsigned char *outbits, int ch);
    void resetbits(int ch);
    void init_bits(void);
    void print_mesg(msg_t * msg);
    int posconv(char *txt, unsigned char *label, char *pos);
    void toaprs(int la, char lac, int ln, char lnc, int prec, char *out);
};

#endif // CACARSGPL_H
