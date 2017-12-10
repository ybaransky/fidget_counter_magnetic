#ifndef FIDGET_H
#define FIDGET_H

//#include <vector>

class SpinData {
    public:
        SpinData(int magnets);
        void        process(uint32_t, uint32_t, uint32_t);
        int         count();
        int         rpm();
        int         maxRPM();
        uint32_t    duration();
        void        start(uint32_t, uint32_t);
        void        end(uint32_t);

    private:
        int _count;
        int _rpm;
        int _maxRPM;
        int _magnets;
        uint32_t _last;
        uint32_t _start;
        uint32_t _duration;
};
//typedef std::vector<SpinData*> Spins;
typedef SpinData* Spins;
class Fidget {
  public:
    Fidget(void);
    void        init(int);
    void        reset(void);
    bool        process(uint32_t count, uint32_t itime, uint32_t ctime);
    int         rpm(void);
    int         maxRPM(void)   { return _maxRPM;}
    SpinData*   currSpin(void) { return _currSpin;};
    Spins*      spins(void)    { return _spins;};
    int         nspins(void)   { return _nspins;};

    // settt/gettr
    bool&       spinning(void)          { return _spinning;}
    void        spinning(const bool& b) { _spinning = b;} 

  private :
    int         _nspins;
    Spins       _spins[100];
    SpinData*   _currSpin;
    int         _maxRPM;
    int         _magnets;
    bool        _spinning;
};

#endif
