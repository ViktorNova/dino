/*****************************************************************************
    DTest - a simple testing framework for C++
    Copyright (C) 2011  Lars Luthman <mail@larsluthman.net>
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

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

      if (!m_tests.empty()) {
	for (auto ti = m_tests.begin(); ti != m_tests.end(); ++ti) {
	  DTest::state.indent = indent;
	  DTest::state.good = DTest::state.bad = 0;
	  cout<<indent_str<<"* "<<ti->first<<": "<<endl;
	  try {
	    ti->second();
	  }
	  catch (std::exception& e) {
	    DTEST_MSG(std::string("  Unexpected ") + typeid(e).name() +
		      " thrown outside test code: '" + e.what() + "'");
	  }
	  catch (...) {
	    DTEST_MSG("  Unexpected exception of unknown type "
		      "thrown outside test code");
	  }
	  if (DTest::state.bad)
	    ++bad;
	  else
	    ++good;
	}
	cout<<indent_str<<"Summary (tests): "<<good<<"/"<<(good + bad)<<endl;
	if (bad)
	  passed = false;
      }

      if (!m_suites.empty()) {
	good = 0; bad = 0;
	for (auto si = m_suites.begin(); si != m_suites.end(); ++si) {
	  cout<<indent_str<<"* Suite: "<<si->first<<endl;
	  if (si->second.run(indent + 2))
	    ++good;
	  else
	    ++bad;
	}
	cout<<indent_str<<"Summary (suites): "<<good<<"/"<<(good + bad)<<endl;
	if (bad)
	  passed = false;
      }

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
  
  /* If there's an argument, load that. Otherwise, load ourself. */
  char const* lib = (argc > 1 ? argv[1] : argv[0]);
  
  /* First, get the list of functions that we should run. */
  unique_ptr<char[]> real_symbol_cmd(new char[std::strlen(symbol_cmd) - 2 +
					      std::strlen(lib) + 1]);
  std::sprintf(real_symbol_cmd.get(), symbol_cmd, lib);
  auto cmd_pipe = make_unique(popen(real_symbol_cmd.get(), "r"), &pclose);
  
  /* Then, build the test suites. */
  char line[256];
  TestSuite root;
  auto self = make_unique(dlopen(lib, RTLD_LAZY | RTLD_GLOBAL), &dlclose);
  if (!self)
    throw runtime_error(string("Could not dlopen() ") + lib);
  auto state_ptr = static_cast<DTest::State**>(dlsym(self.get(), "_dtest"));
  if (state_ptr)
    *state_ptr = &DTest::state;
  _dtest = &DTest::state;
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
  if (root.run())
    return 0;
  return 1;
}
