#include <cmath>
#include "libport/ufloat.hh"

#if 0
#include "libport/uffloat.cc"
#endif

namespace urbi
{
#if defined(FLOAT_FAST) || defined(FLOAT_TABULATED)
  void buildSinusTable(int powersize);
  class DummyInit
  {
  public:
    DummyInit() {buildSinusTable(SINTABLE_POWER);}
  };

  DummyInit _dummyInit__;


  static ufloat * sinTable=0; //we store [O, PI/2[
  static ufloat * asinTable=0; //we store [O, 1[

  static unsigned long tableSize; //must be a power of two
  static int tableShift;

  void buildSinusTable(int powersize) {
    int size = 1<<powersize;
    tableShift = powersize;
    //don't use a step-based generation or errors will accumulate
    if (sinTable) {
      delete [] sinTable;
      delete [] asinTable;
    }

    sinTable = new ufloat[size];
    asinTable = new ufloat[size];

    tableSize = size;
    for (int i=0;i<size;i++) {
      double idx = (double)i*(M_PI/2.0)/(double)size;
      float val = ::sin(idx);
      sinTable[i]=val;

      double idx2 = (double)i/(double)size;
      asinTable[i] = ::asin(idx2);
    }
  }

#endif
#ifdef FLOAT_TABULATED


  ufloat tabulatedSin(ufloat val) {
    ufloat fidx = (val*(ufloat)tableSize / (M_PI/2.0));
    int idx = (int) fidx;
    ufloat rem = fidx -(ufloat)idx;

    idx = idx & (tableSize-1);

    if (fmod(val, M_PI) >= M_PI/2)
      idx = (tableSize-idx-1); //sin(pi/2+x) = sin(pi/2-x)
    ufloat interp = sinTable(idx)*(1.0-rem)+sinTable[(idx+1)%tableSize]*rem;

    if (fmod(val, M_PI*2) > M_PI)
      return -interp;
    else
      return interp;
  };

  ufloat tabulatedCos(ufloat val) {
    ufloat fidx = (val*(ufloat)tableSize / (M_PI/2.0));
    int idx = (int) fidx;
    ufloat rem = fidx -(ufloat)idx;

    idx = idx & (tableSize-1);

    if (fmod(val, M_PI) < M_PI/2)
      idx = (tableSize-idx-1); //sin(pi/2+x) = sin(pi/2-x)

    ufloat interp = sinTable(idx)*(1.0-rem)+sinTable[(idx+1)%tableSize]*rem;

    if (fmod(val, M_PI*2) > M_PI)
      return -interp;
    else
      return interp;
  };


  ufloat tabulatedASin(ufloat val) {
    ufloat fidx = val *(ufloat)tableSize;
    int idx =(int) fidx;
    ufloat rem = fidx -(ufloat)idx;
    idx = idx & (tableSize-1);
    ufloat interp = asinTable(idx)*(1.0-rem)+asinTable[(idx+1)%tableSize]*rem;

    if (val < 0.0)
      return -interp;
    else
      return interp;
  }


#endif
#ifdef FLOAT_FAST


  ufloat tabulatedSin(ufloat val) {
    static ufloat factor(2.0 / M_PI);
    ufloat fidx = val * factor; //now we are 4-periodic: xy.zzz x:pi phase y:pi/2 phase

    //remove high part to avoid overflow, keeping 2 bits of intpart+decpart
    fidx = fidx - (fidx.getValue(-2)<<2);
    int extraidx = fidx.getValue(); //the two bits
    int idx = fidx.getValue(tableShift)& (tableSize-1); //discards the 2 bits

    //std::cerr <<"** "<<idx<<std::endl;

    if (extraidx&1) //sin(pi/2+x) = sin(pi/2-x)
      idx = (tableSize-idx-1)&(tableSize-1);
    ufloat v1 = sinTable[idx];
#if TABULATEDSIN_NO_INTERPOLATE
    return v1;
#else
    ufloat rem = fidx<<tableShift;
    rem = rem - rem.getValue();
    ufloat v2 = sinTable[(idx+1) & (tableSize-1)];
    /*
      std::cerr <<idx<<" "<<((idx+1) & (tableSize-1))<<"  "<<extraidx<<"  "<< v1<<" "<<v2<<std::endl;
      std::cerr << rem<<"  "<<(ufloat(1L)-rem)<<std::endl;
      //std::cerr << (v2*rem)<<"   "<<(v1*(ufloat(1L)-rem))<<std::endl;
      ufloat omr = (ufloat(1L)-rem);
      //std::cerr << omr<<"  "<<v1<<"  "<<(v1*omr)<<std::endl;
      */
    ufloat vi = (v1*(ufloat(1L)-rem))+(v2*rem);
    if (extraidx&2) {
      //std::cerr <<"glop "<<vi<<"  "<<(-vi)<<std::endl;
      return -vi;
    }
    else
      return vi;
#endif
  }


  ufloat tabulatedCos(ufloat val) { //just reverse pi/2 bit meaning
    static ufloat factor(2.0 / M_PI);
    ufloat fidx = val * factor; //now we are 4-periodic: xy.zzz x:pi phase y:pi/2 phase

    //remove high part to avoid overflow, keeping 2 bits of intpart+decpart
    fidx = fidx - (fidx.getValue(-2)<<2);
    int extraidx = fidx.getValue(); //the two bits
    int idx = fidx.getValue(tableShift)& (tableSize-1); //discards the 2 bits

    //std::cerr <<"** "<<idx<<std::endl;

    if (!(extraidx&1)) //sin(pi/2+x) = sin(pi/2-x)
      idx = (tableSize-idx-1)&(tableSize-1);
    ufloat v1 = sinTable[idx];
#if TABULATEDSIN_NO_INTERPOLATE
    return v1;
#else
    ufloat rem = fidx<<tableShift;
    rem = rem - rem.getValue();
    ufloat v2 = sinTable[(idx+1) & (tableSize-1)];

    /*
      std::cerr <<idx<<" "<<((idx+1) & (tableSize-1))<<"  "<<extraidx<<"  "<< v1<<" "<<v2<<std::endl;
      std::cerr << rem<<"  "<<(ufloat(1L)-rem)<<std::endl;
      //std::cerr << (v2*rem)<<"   "<<(v1*(ufloat(1L)-rem))<<std::endl;
      ufloat omr = (ufloat(1L)-rem);
      //std::cerr << omr<<"  "<<v1<<"  "<<(v1*omr)<<std::endl;
      */


    ufloat vi = (v1*(ufloat(1L)-rem))+(v2*rem);
    std::cerr<<vi<<std::endl;
    if (extraidx&2) {
      std::cerr <<"glop "<<vi<<"  "<<(-vi)<<std::endl;
      return -vi;
    }
    else
      return vi;
#endif
  }





  ufloat tabulatedASin(ufloat val) {
    //remove high part
    ufloat fidx = val;
    if (val<0)
      fidx = -fidx;
    fidx - fidx.getValue();
    int idx = fidx.getValue(tableShift);

    //std::cerr <<"** "<<idx<<std::endl;
    ufloat v1 = asinTable[idx];
#if TABULATEDSIN_NO_INTERPOLATE
    return v1;
#else
    ufloat rem = fidx << tableShift;
    rem = rem - rem.getValue();
    ufloat v2 = asinTable[(idx+1) & (tableSize-1)];
    /*
      std::cerr <<idx<<" "<<((idx+1) & (tableSize-1))<<"  "<<extraidx<<"  "<< v1<<" "<<v2<<std::endl;
      std::cerr << rem<<"  "<<(ufloat(1L)-rem)<<std::endl;
      //std::cerr << (v2*rem)<<"   "<<(v1*(ufloat(1L)-rem))<<std::endl;
      ufloat omr = (ufloat(1L)-rem);
      //std::cerr << omr<<"  "<<v1<<"  "<<(v1*omr)<<std::endl;
      */
    ufloat vi = (v1*(ufloat(1L)-rem))+(v2*rem);
    if (val<0) {
      //std::cerr <<"glop "<<vi<<"  "<<(-vi)<<std::endl;
      return -vi;
    }
    else
      return vi;
#endif
  }


  std::ostream& operator <<(std::ostream &s, ULLFixedPoint u)
  {
    long long uh  = (u.v >> LONG_NBIT);
    unsigned long long ul = u.v&((1LL<<LONG_NBIT)-1);
    bool neg=false;
    if (uh<0) {
      uh++;
      ul = (1LL<<LONG_NBIT)-ul;
      neg=true;
    }
    if (neg && (uh==0))
      s<<"-";
    s<< uh <<".";
    u.rawSet(ul);
    const int limit = 1+(LONG_NBIT*3)/10;
    ULLFixedPoint ten(10L);
    for (int i=0;i<limit;i++) {
      u=u*ten;
      s << (char)('0'+ u.getValue());
      u = u - u.getValue();
    }
    return s;
  }


  std::istream& operator >>(std::istream &s, ULLFixedPoint &u) {
    long ir=0;
    char c;
    bool negative = false;
    while (s.get()==' ')
      ;
    s.unget();
    //get sign if exists
    c=s.get();
    if (c=='-')
      negative = true;
    else if ((c>='0' && c<='9')||(c=='.'))
      s.unget();
    else if (c != '+') {
      s.setstate(std::ios_base::failbit);
      return s;
    }
    while (!s.eof()) {
      c=s.get();
      if (!s)
	break;
      if (c<'0' || c>'9') {
	s.unget();
	break;
      }
      ir=ir*10+(c-'0');
    }
    u = ir;

    if (!s)
      return s;
    if (!s.eof()) {
      c=s.get();
      if (c!='.') { //XXX ignoring locales
	s.unget();
	return s;
      }
    }
    //get decimal part, counting number of characters
    int nc=0;
    long dr=0;
    long factor=1;
    const int limit = (LONG_NBIT*3)/10; //avoid storing too many of them
    while (!s.eof()) {
      c=s.get();
      if (!s)
	break;
      if (c<'0' || c>'9') {
	s.unget();
	break;
      }
      if (nc != limit) {
	dr=dr*10+(c-'0');
	nc++;
	factor *=10;
      }
    }

    while (s.get()==' ')
      ;
    s.unget();
    ULLFixedPoint t(dr);
    t = t / ULLFixedPoint(factor);
    u = u+t;
    if (negative)
      u = ULLFixedPoint(0L)-u;
    return s;
  }



#endif

}
