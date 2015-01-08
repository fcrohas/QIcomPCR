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
#ifndef CRTTY_H
#define CRTTY_H

#include <QObject>
#include <IDecoder.h>
#include <math.h>
#include <qdebug.h>
#include <CWindowFunc.h>
#include <CFir.h>
#include <consts.h>

#define STOPBITS 1.5
#define DATABITS 5
#define STARTBITS 1

static unsigned char lettersr[] = {'\0','T','\r','O',' ','H','N','M','\n','L','R','G','I','P','C','V','E','Z','D','B','S','Y','F','X','A','W','J','\0','U','Q','K','\0'};
static unsigned char figuresr[] = {'\0','5','\r','9',' ','#',',','.','\n',')','4','&','8','0',':',';','3','\"','$','?','\a','6','!','/','-','2','\'','\0','7','1',';','\0'};
static unsigned char letters[] = {'\0','E','\n','A',' ','S','I','U','\r','D','R','J','N','F','C','K','T','Z','L','W','H','Y','P','Q','O','B','G','\0','M','X','V','\0'};
static unsigned char figures[] = {'\0','3','\n','-',' ','\a','8','7','\r','$','4','\'',',','!',':','(','5','\"',')','2','#','6','0','1','9','?','&','\0','.','/',';','\0'};


class CRtty : public IDecoder
{
    Q_OBJECT
public:
    explicit CRtty(QObject *parent = 0, uint channel=1);
    ~CRtty();
    void decode(int16_t *buffer, int size, int offset);
    uint getDataSize();
    uint getChannel();
    uint getBufferSize();
    void setCorrelationLength(int value) override;

signals:
    void dumpData(double*,double*,int);

public slots:
    void slotFrequency(double value);
    void slotBandwidth(double value);
    void setThreshold(double value);

private:
    int bandwidth;
    int channel;
    double frequency; // Center frequency
    double freqlow; // Low mark frequency
    double freqhigh; // High space frequency
    int correlationLength;
    double baudrate;
    double inverse;
    int accmark;
    int accspace;
    // letter
    QString letter;
    int bitcount;
    bool started;
    int bit;
    int counter;
    bool sync;
    bool isletters;

    // Audio channel buffer
    double* audioData[2];
    double* audioBuffer[2];

    // Table
    double *xval;
    double *yval;
    double *corrmark;
    double *corrspace;
    double *avgcorr;
    double *mark_q;
    double *mark_i;
    double *space_i;
    double *space_q;

    // Filters
    CWindowFunc *winfunc;
    CFIR<double> *fmark;
    CFIR<double> *fspace;
    CFIR<double> *flow;

    // Correlation length generator
    void GenerateCorrelation(int length);
};

#endif // CRTTY_H
