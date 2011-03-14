#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>

#include <dlfcn.h>

#include "dtest.hpp"


using std::cout;
using std::endl;
using std::runtime_error;
using std::string;
using std::unique_ptr;


namespace DTest {
  State state;
}

DTest::State* _dtest = 0;


namespace {
  
  /** The shell command used to generate the symbol list. It is a bit hacky to
      do it this way, but it lets us build the test as a single binary without
      wrapping scripts which is very convenient. The %s should be replaced by
      argv[0]. */
  char const* symbol_cmd = 
    "nm %s | grep ' T ' | awk '{print $3}' | "
    "xargs -I'{}' sh -c \"echo -n '{} '; c++filt {}\" \\; | "
    "grep -E '[a-zA-Z0-9_:]+::dtest_' | sed 's/(.*//'";
  
  
  /** This is not in the standard library for some reason. */
  template <typename T, typename D>
  unique_ptr<T, D> make_unique(T* ptr, D&& deleter) {
    return unique_ptr<T, D>(ptr, deleter);
  }

  
  /** A test suite is a set of tests and/or other suits. */
  struct TestSuite {
    typedef void(*Test)();
    
    /** Add a test to this test suite. The name should be a fully
	qualified C++ function name, namespaces will be added automatically
	as subsuites. */
    void add_test(string const& name, Test const& test) {
      size_t suite_end = name.find("::");
      if (suite_end == string::npos)
	m_tests[name] = test;
      else {
	m_suites[name.substr(0, suite_end)].
	  add_test(name.substr(suite_end + 2), test);
      }
    }
    
    /** Run all tests and suites and print some info. */
    bool run(size_t indent = 0) {
      bool passed = true;
      string indent_str(indent, ' ');
      int good = 0, bad = 0;
      for (auto ti = m_tests.begin(); ti != m_tests.end(); ++ti) {
	DTest::state.indent = indent;
	DTest::state.good = DTest::state.bad = 0;
	cout<<indent_str<<"* "<<ti->first<<": "<<endl;
	ti->second();
	if (DTest::state.bad)
	  ++bad;
	else
	  ++good;
      }
      if (!m_tests.empty())
	cout<<indent_str<<"Summary: "<<good<<"/"<<(good + bad)<<endl;
      if (bad)
	passed = false;
      good = bad = 0;
      for (auto si = m_suites.begin(); si != m_suites.end(); ++si) {
	cout<<indent_str<<"* Suite: "<<si->first<<endl;
	if (si->second.run(indent + 2))
	  ++bad;
	else
	  ++good;
      }
      if (!m_suites.empty())
	cout<<indent_str<<"Summary: "<<good<<"/"<<(good + bad)<<endl;
      if (bad)
	passed = false;
      return passed;
    }
    
  private:
    
    std::map<string, TestSuite> m_suites;
    std::map<string, Test> m_tests;
  };


  /** So we don't have to cast all the time. */
  TestSuite::Test dltestsym(void* handle, string const& symbol) {
    union {
      void* in;
      TestSuite::Test out;
    } u;
    u.in = dlsym(handle, symbol.c_str());
    if (!u.in)
      throw runtime_error(string("Could not find symbol ") + symbol);
    return u.out;
  }
  
  
}

  
int main(int argc, char** argv) {
  
  /* First, get the list of functions that we should run. */
  unique_ptr<char[]> real_symbol_cmd(new char[std::strlen(symbol_cmd) - 2 +
					      std::strlen(argv[0]) + 1]);
  std::sprintf(real_symbol_cmd.get(), symbol_cmd, argv[0]);
  auto cmd_pipe = make_unique(popen(real_symbol_cmd.get(), "r"), &pclose);
  
  /* Then, build the test suites. */
  char line[256];
  TestSuite root;
  auto self = make_unique(dlopen(argv[0], RTLD_LAZY | RTLD_GLOBAL), &dlclose);
  if (!self)
    throw runtime_error("Could not dlopen() myself");
  auto state_ptr = static_cast<DTest::State**>(dlsym(self.get(), "_dtest"));
  *state_ptr = &DTest::state;
  while (std::fgets(line, 255, cmd_pipe.get())) {
    char* space;
    for (space = line; *space != 0 && *space != ' '; ++space);
    if (space == 0)
      throw runtime_error(string("Invalid line: ") + line);
    *space = 0;
    char* i;
    for (i = space + 1; *i != 0 && *i != '\n'; ++i);
    *i = 0;
    root.add_test(space + 1, dltestsym(self.get(), line));
  }
  
  /* And run the test suites. */
  cout<<"Running tests..."<<endl;
  root.run();
  
  return 0;
}
