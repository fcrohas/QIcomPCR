#include "CDemodulatorFactory.h"

CDemodulatorBase* CDemodulatorFactory::Builder(CDemodulatorBase::Mode mode) {
    CDemodulatorBase *demod = NULL;
    switch(mode) {
        case CDemodulatorBase::eFM : demod = new CFm(NULL,CDemodulatorBase::eFM); break;
        case CDemodulatorBase::eAM : demod = new CAm(NULL,CDemodulatorBase::eAM); break;
        case CDemodulatorBase::eWFM : demod = new CFm(NULL,CDemodulatorBase::eWFM); break;
        case CDemodulatorBase::eLSB : demod = new CSsb(NULL,CDemodulatorBase::eLSB); break;
        case CDemodulatorBase::eUSB : demod = new CSsb(NULL,CDemodulatorBase::eUSB); break;
    }
    return demod;
}
