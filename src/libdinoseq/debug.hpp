#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <iostream>
#include <iomanip>


using namespace std;


#ifndef NDEBUG
#define dbg0 (cerr<<"\033[31;1m"<<'['<<setw(16)<<setfill(' ')<<__FILE__<<':' \
	      <<setw(3)<<setfill('0')<<__LINE__<<"] "<<"\033[0m")
#define dbg1 (cerr<<"\033[32;1m"<<'['<<setw(16)<<setfill(' ')<<__FILE__<<':' \
	      <<setw(3)<<setfill('0')<<__LINE__<<"] "<<"\033[0m")
#else

#define dbg0 cerr

struct NoOpStream {

};

extern NoOpStream dbg1;


template <class T>
inline NoOpStream& operator<<(NoOpStream& a, const T&) { 
  return a;
}

inline NoOpStream& operator<<(NoOpStream& a, ostream& (*__pf)(ostream&)) { 
  return a;
}

#endif


#endif
