#ifndef SIGNALWRAPPERS_HPP
#define SIGNALWRAPPERS_HPP

#include <sigc++/sigc++.h>
#include <boost/python.hpp>


class Connection {
public:
  Connection(const sigc::connection& c) : m_connection(c) { }
  void disconnect() { m_connection.disconnect(); }
private:
  sigc::connection m_connection;
};


template <class R>
struct CallbackWrapper0 {
  boost::python::object obj;
  CallbackWrapper0(boost::python::object& _obj) : obj(_obj) { }
  R operator()() {
    obj();
  }
};


template <class R, class A1>
struct CallbackWrapper1 {
  boost::python::object obj;
  CallbackWrapper1(boost::python::object& _obj) : obj(_obj) { }
  R operator()(A1 a1) {
    obj(a1);
  }
};


template <class R, class A1, class A2>
struct CallbackWrapper2 {
  boost::python::object obj;
  CallbackWrapper2(boost::python::object& _obj) : obj(_obj) { }
  R operator()(A1 a1, A2 a2) {
    obj(a1, a2);
  }
};


template <class R, class A1, class A2, class A3>
struct CallbackWrapper3 {
  boost::python::object obj;
  CallbackWrapper3(boost::python::object& _obj) : obj(_obj) { }
  R operator()(A1 a1, A2 a2, A3 a3) {
    obj(a1, a2, a3);
  }
};


// Macros are ugly but efficient
#define SWR0(C,SIG) Connection C##_signal_##SIG(Dino::C& self, boost::python::object& obj) { \
    return Connection(self.signal_##SIG().connect(CallbackWrapper0<void>(obj))); \
  } enum {}
#define SWR1(C,SIG,A1) Connection C##_signal_##SIG(Dino::C& self, boost::python::object& obj) { \
    return Connection(self.signal_##SIG().connect(CallbackWrapper1<void, A1>(obj))); \
  } enum {}
#define SWR2(C,SIG,A1,A2) Connection C##_signal_##SIG(Dino::C& self, boost::python::object& obj) { \
    return Connection(self.signal_##SIG().connect(CallbackWrapper2<void, A1, A2>(obj))); \
  } enum {}
#define SWR3(C,SIG,A1,A2,A3) Connection C##_signal_##SIG(Dino::C& self, boost::python::object& obj) { \
    return Connection(self.signal_##SIG().connect(CallbackWrapper3<void, A1, A2, A3>(obj))); \
  } enum {}


#endif
