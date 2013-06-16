#ifndef CMORSE_H
#define CMORSE_H

#include <QObject>
#include <QHash>
#include <IDemodulator.h>
#include <math.h>
#define FIR
#ifdef FIR
#include <CFir.h>
#else
#include <CIir.h>
#endif
#include <CWindowFunc.h>
#include <consts.h>

// x is sample index and f frequency


struct CW_TABLE {
        char chr;       /* The character(s) represented */
        const char *prt;        /* The printable representation of the character */
        const char *rpr;        /* Dot-dash shape of the character */
};


static CW_TABLE cw_table[] = {
        /* Prosigns */
        {'=',   "<BT>",   "-...-"       }, // 0
        {'~',   "<AA>",   ".-.-"        }, // 1
        {'<',   "<AS>",   ".-..."       }, // 2
        {'>',   "<AR>",   ".-.-."       }, // 3
        {'%',   "<SK>",   "...-.-"      }, // 4
        {'+',   "<KN>",   "-.--."       }, // 5
        {'&',   "<INT>",  "..-.-"       }, // 6
        {'{',   "<HM>",   "....--"      }, // 7
        {'}',   "<VE>",   "...-."       }, // 8
        /* ASCII 7bit letters */
        {'A',   "A",    ".-"    },
        {'B',   "B",    "-..."  },
        {'C',   "C",    "-.-."  },
        {'D',   "D",    "-.."   },
        {'E',   "E",    "."             },
        {'F',   "F",    "..-."  },
        {'G',   "G",    "--."   },
        {'H',   "H",    "...."  },
        {'I',   "I",    ".."    },
        {'J',   "J",    ".---"  },
        {'K',   "K",    "-.-"   },
        {'L',   "L",    ".-.."  },
        {'M',   "M",    "--"    },
        {'N',   "N",    "-."    },
        {'O',   "O",    "---"   },
        {'P',   "P",    ".--."  },
        {'Q',   "Q",    "--.-"  },
        {'R',   "R",    ".-."   },
        {'S',   "S",    "..."   },
        {'T',   "T",    "-"     },
        {'U',   "U",    "..-"   },
        {'V',   "V",    "...-"  },
        {'W',   "W",    ".--"   },
        {'X',   "X",    "-..-"  },
        {'Y',   "Y",    "-.--"  },
        {'Z',   "Z",    "--.."  },
        /* Numerals */
        {'0',   "0",    "-----" },
        {'1',   "1",    ".----" },
        {'2',   "2",    "..---" },
        {'3',   "3",    "...--" },
        {'4',   "4",    "....-" },
        {'5',   "5",    "....." },
        {'6',   "6",    "-...." },
        {'7',   "7",    "--..." },
        {'8',   "8",    "---.." },
        {'9',   "9",    "----." },
        /* Punctuation */
        {'\\',  "\\",   ".-..-."        },
        {'\'',  "'",    ".----."        },
        {'$',   "$",    "...-..-"       },
        {'(',   "(",    "-.--."         },
        {')',   ")",    "-.--.-"        },
        {',',   ",",    "--..--"        },
        {'-',   "-",    "-....-"        },
        {'.',   ".",    ".-.-.-"        },
        {'/',   "/",    "-..-."         },
        {':',   ":",    "---..."        },
        {';',   ";",    "-.-.-."        },
        {'?',   "?",    "..--.."        },
        {'_',   "_",    "..--.-"        },
        {'@',   "@",    ".--.-."        },
        {'!',   "!",    "-.-.--"        },
        {0, NULL, NULL}
};


class CMorse : public IDemodulator
{
    Q_OBJECT
public:
    explicit CMorse(QObject *parent = 0, uint channel=1);
    ~CMorse();
    void decode(int16_t *buffer, int size, int offset);
    uint getDataSize();
    uint getChannel();
    uint getBufferSize();
    void setThreshold(double value) override;
    void setCorrelationLength(int value) override;
    
signals:
    void dumpData(double*,double*,int);
public slots:
    void slotFrequency(double value);
    void slotBandwidth(double value);

private:
    // Correlation generator
    void GenerateCorrelation(int length);
    // Decoding timing buffer
    double *mark;
    double *space;
    // Correlation data
    double *mark_i;
    double *mark_q;
    // scope and signal data
    double *xval;
    double *yval;
    // Correlation result
    double *corr;
    // Correlation average
    double *avgcorr;
    // Correlation length
    int correlationLength;
    //Channel select
    uint channel;
    //Current frequency selected
    double frequency;
    // Filters
    CWindowFunc *winfunc;
#ifdef FIR
    CFIR *fbandpass;
#else
    CIIR *fbandpass;
#endif
    // Audio channel buffer
    double* audioData[1];
    double* audioBuffer[1];
    // Hash table for morse code
    QHash<QString,int> code;
    // Translate result
    void translate(int position, int position2);
    // Emit Letter or Word
    void CheckLetterOrWord(int position, int position2);
    // Symbol array
    QString symbols;
    // Set accumulator to class
    // So timing can continue on next buffer
    int accup; // accumulator sample high state
    int acclow; // accumulator sample low state
    int marks; // Count edge
    int spaces; // count space
    // Save timming of mark dash
    double markdash;
    // Word container
    QString word;
    // Threashold value
    double agc;
    double agclimit;
    int bandwidth;
    // Goertzel filter
    double goertzel(double *x, int N, double frequency, int samplerate);
    // for kalman
    double Pp; // P previous value
    //the noise in the system
    double Q;
    double R;
    double K;
    double P;
    double lastestimation;


};

#endif // CMORSE_H
