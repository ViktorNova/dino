#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <streambuf>

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


  /** Need this to avoid C I/O. */
  struct CFileBuf : public std::streambuf {
    CFileBuf(FILE* file)
      : m_file(file) {
      char* end = &m_buffer[0] + m_buffer.size();
      setg(end, end, end);
    }
    
  private:
    
    int_type underflow() {
      
      if (gptr() == egptr()) {
	
	char* base = &m_buffer[0];
	char* start = base;
	
	/* Copy previously read (or put back) data to the beginning. */
	if (eback() == base) {
	  std::memmove(base, egptr() - m_putback, m_putback);
	  start += m_putback;
	}
	
	/* Actually read from the FILE. */
	size_t n = std::fread(start, 1,
			      m_buffer.size() - (start - base), m_file);
	if (n == 0)
	  return traits_type::eof();
	
	/* Reset buffer pointers. */
	setg(base, start, start + n);
      }
      
      return traits_type::to_int_type(*gptr());
    }
    
    FILE* m_file;
    static size_t const m_putback = 8;
    std::array<char, 256> m_buffer;
  };


  struct TestSuite {
    typedef void(*Test)();
    
    void add_test(string const& name, Test const& test) {
      size_t suite_end = name.find("::");
      if (suite_end == string::npos)
	m_tests[name] = test;
      else {
	m_suites[name.substr(0, suite_end)].
	  add_test(name.substr(suite_end + 2), test);
      }
    }
    
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
  auto cmd_pipe = make_unique(popen(real_symbol_cmd.get(), "r"),
			      [](FILE* f) { pclose(f); });
  std::istream cmd(new CFileBuf(cmd_pipe.get()));
  
  /* Then, build the test suites. */
  string line;
  TestSuite root;
  auto self = dlopen(argv[0], RTLD_LAZY | RTLD_GLOBAL);  
  if (!self)
    throw runtime_error("Could not dlopen() myself");
  auto state_ptr = reinterpret_cast<DTest::State**>(dlsym(self, "_dtest"));
  *state_ptr = &DTest::state;
  while (std::getline(cmd, line)) {
    string symbol = line.substr(0, line.find(' '));
    if (symbol.size() == line.size())
      throw runtime_error(string("Invalid line: ") + line);
    root.add_test(line.substr(symbol.size() + 1), dltestsym(self, symbol));
  }
  
  /* And run the test suites. */
  cout<<"Running tests..."<<endl;
  root.run();
  
  return 0;
}

