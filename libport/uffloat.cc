/*! \file uffloat.cc
 *******************************************************************************

 File: uffloat.cc\n
 Definition of the UUFloat classes.

 This file is part of
 %URBI Kernel, version __kernelversion__\n
 (c) Jean-Christophe Baillie, 2004-2005.

 This software is provided "as is" without warranty of any kind,
 either expressed or implied, including but not limited to the
 implied warranties of fitness for a particular purpose.

 For more information, comments, bug reports: http://www.urbiforge.net

 **************************************************************************** */

#include "libport/uffloat.hh"

namespace urbi
{

  std::istream& operator >>(std::istream & s,UFFloat &u)
  {
    long ir=0;
    int c;
    bool negative = false;
    while (s.get()==' ')
      ;
    s.unget();
    //no, eats one extra character s >> ir; // get integer part
    //get sign if exists
    c=s.get();
    if (c=='-')
      negative = true;
    else if (c>='0' && c<='9')
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

    long dr=0; //decimal part
    long factor=1; //multiplier factor

    c=s.get();
    if (c!='.')  //XXX ignoring locales
      s.unget();
    else {
      //get decimal part, counting number of characters
      const int limit = (sizeof(long)*8*3)/10; //avoid storing too many of them
      int nc=0;
      while (!s.eof()) {
	c=s.get();
	if (!s)
	  break;
	if (c<'0' || c>'9') {
	  s.unget();
	  break;
	}
	if (nc != limit && factor<(1<<sizeof(long)-3)) {
	  dr=dr*10+(c-'0');
	  nc++;
	  factor *=10;
	}
      }
    }
    //get exponent if any
    long exponent = 0;
    bool eneg=false;
    c=s.get();
    if (c!='e' && c!='E')
      s.unget();
    else {
      c=s.get();
      if (c=='-')
	eneg = true;
      else if (c>='0' && c<='9')
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
	exponent=exponent*10+(c-'0');
      }
      if (eneg)
	exponent *= (-1);
    }

    //now put the pieces back together
    if (DEBUG)
      std::cerr <<"in intpart="<<ir<<"  decpart="<<dr<<"  exponent="<<exponent<<std::endl;
    //first, we need to calculate 10^exponent
    //method, calculate 10, 10^2, 10^4, 10^8...and write 10^exp=10^(2^e1)*10^(2^e2)...
    UFFloat p10;
    UFFloat expfactor = 1;
    if (exponent>=0)
      p10 = 10;
    else
      p10 = UFFloat(1) / UFFloat(10);
    for (int i=0; (1<<i)<= abs(exponent);i++) {
      if (exponent & (1<<i))
	expfactor = expfactor * p10;
      p10 = p10*p10;
    }

    u=ir;
    UFFloat dec = dr;
    dec = dec/(UFFloat)factor;
    u = u + dec;
    u = u*expfactor;
    return s;
  }

  std::ostream& operator <<(std::ostream &s, UFFloat u) {

    //we need biggest power of 10 smaller than u
    long starte = (u.e+30)* 30103 / 100000; //(u.e*ln(3)/ln(10): solution of 2^u.e=10^start)
    //get 10^ start
    UFFloat p10;
    UFFloat start = 1;
    if (u.e>0)
      p10 = 10;
    else
      p10 = UFFloat(1) / UFFloat(10);
    for (int i=0; (1<<i)<= abs(starte);i++) {
      if (starte & (1<<i))
	start = start * p10;
      p10 = p10*p10;
    }
    while (start<u) {
      starte++;
      start = start*10;
    }
    while (start > u) {
      starte--;
      start = start/10;
    }

    //now loop
    if (u<0)
      s<<"-";
    for (int d=0;d<8;d++) {
      UFFloat ratio = u/start;
      int digit = ratio.getValue();
      u = u - (UFFloat(digit)*start);
      start = start/ UFFloat(10);
      s << (char)(abs(digit)+'0');
      if (d==0)
	s<<".";
    }
    s << "e"<<(starte);
    return s;
  }

}

#endif // ! LIBPORT_UFFLOAT_HH
