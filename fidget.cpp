#include <algorithm>
#include "Arduino.h"
#include "fidget.h"

/*
 * the fidget is either spinning or not. The transition to spinning 
 * requires at least 2 measurements, so we just wait for the next onw
 *
 * but if the fidget stopped or pulled away suddenly, then count == 0
 */

SpinData::SpinData(int magnets) : _magnets(magnets), _rpm(0), _maxRPM(0), _count(0) {}
int SpinData::count()     { return _count;}
int SpinData::rpm()       { return _rpm;}
int SpinData::maxRPM()    { return _maxRPM;}
uint32_t SpinData::duration()  { return _duration;}

void SpinData::process(uint32_t count, uint32_t itime, uint32_t ctime) {
    int period = float( _magnets * (itime - _last)) / float(count);
    float hz   = 1000 * 1.0 / period;
    _last      = itime;
    _rpm       = int( 60 * hz );
    _maxRPM    = std::max( _maxRPM , _rpm );
    _count     += count;
    _duration  = ctime - _start;
}

void SpinData::start(uint32_t count, uint32_t stime) {
    Serial.printf("starting a new spin at %d\n",stime);
    _count = count;
    _start = stime;
    _last  = stime;
}

void SpinData::end(uint32_t etime) {
    _duration = etime - _start;
    Serial.printf("ending spin duration= %d count=%d maxrpm=%d\n", _duration, _count, _maxRPM);
}

/*******************************************************/

Fidget::Fidget() : _magnets(3), _spinning(false), _currSpin(NULL) {
    _nspins = 0; 
}
void Fidget::init(int magnets) {
}

bool Fidget::process(uint32_t count, uint32_t itime, uint32_t ctime) {
    bool refresh = false;
    if (count) {
        if (_spinning) {
            _currSpin->process(count, itime, ctime);
            _maxRPM = std::max( _maxRPM , _currSpin->maxRPM() );
            refresh = true;
        } else {
            _spinning = true;
            _currSpin = new SpinData(_magnets);
            _currSpin->start(count, itime);
        }
    } else {
        if (_spinning) {
            //  if we waited long enough, end this spin cycle
            if (ctime - itime > 2000) {
                _spinning = false;
                _currSpin->end(ctime);
                _maxRPM = std::max( _maxRPM , _currSpin->maxRPM() );
                // if this was too smaall, ignore it
                int count = _currSpin->count();
                if (count > 5) {
                    _spins[_nspins++] = _currSpin;
                } else {
                    delete _currSpin;
                }
                _currSpin = NULL;
                Serial.printf("this spin had %d, so far %d spin events\n",count,_nspins);
                refresh = true;
            }
        }
    }
    return refresh;
}

void Fidget::reset(void) { 
    if (_currSpin) {
        delete _currSpin;
        _currSpin = NULL;
    }
    for(int i=0; i< _nspins; i++) {
        delete _spins[i];
        _spins[i] = NULL;
    }
    _nspins = 0;
    _maxRPM = 0;
    _spinning = false;
}

int Fidget::rpm(void) { 
    if (spinning())
      return std::max( currSpin()->rpm() , 0 );
    return 0;
}
