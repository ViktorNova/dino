/****************************************************************************
   Dino - A simple pattern based MIDI sequencer
   
   Copyright (C) 2006  Lars Luthman <larsl@users.sourceforge.net>
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
****************************************************************************/

#include <csignal>
#include <iostream>
#include <string>

#include <glibmm.h>
#include <gtkmm.h>
#include <sigc++/slot.h>
#include <libglademm.h>

#include "debug.hpp"
#include "dinogui.hpp"


using namespace std;
using namespace Gtk;
using namespace Gnome::Glade;
using namespace Glib;
using namespace Dino;


static void print_version() {
  cout<<"Dino "<<VERSION<<endl
      <<"Copyright (C) "<<CR_YEAR<<" Lars Luthman <larsl@users.sf.net>"<<endl
      <<"This program comes with ABSOLUTELY NO WARRANTY."<<endl
      <<"This is free software, and you are welcome to redistribute it"<<endl
      <<"under certain conditions; see the file COPYING or the "<<endl
      <<"\"About Dino\" dialog in the \"Help\" menu for details."<<endl;
}


static bool do_quit = false;
static int signum = 0;


static void signal_handler(int signal) {
  signum = signal;
  do_quit = true;
}


static bool signal_checker() {
  if (do_quit) {
    dbg0<<"Caught signal "<<signum<<endl;
    Main::quit();
  }
  return true;
}


int main(int argc, char** argv) {
  
  // print the version if requested
  if (argc > 1 && !strcmp(argv[1], "--version")) {
    print_version();
    return 0;
  }
  
  dbg1<<"Dino "<<VERSION<<" starting"<<endl;
  
  // load the GUI
  dbg1<<"Initialising gtkmm"<<endl;
  Main kit(argc, argv);
  dbg1<<"Loading GUI"<<endl;
  RefPtr<Xml> refXml;
  string filename = "dino.glade";
  if (!file_test(filename, FILE_TEST_EXISTS))
    filename = DATA_DIR "/dino.glade";
  refXml = Xml::create(filename);
    
  DinoGUI dino(argc, argv, refXml);
  
  // setup a signal handler and a timeout function that will let us
  // quit cleanly if the user terminates us with a signal
  std::signal(SIGHUP, &signal_handler);
  std::signal(SIGINT, &signal_handler);
  std::signal(SIGPIPE, &signal_handler);
  std::signal(SIGALRM, &signal_handler);
  std::signal(SIGTERM, &signal_handler);
  std::signal(SIGUSR1, &signal_handler);
  std::signal(SIGUSR2, &signal_handler);
  signal_timeout().connect(&signal_checker, 300);
  
  // run
  if (dino.get_window()) {
    dbg1<<"Starting GUI"<<endl;
    Main::run(*dino.get_window());
  }
  else {
    dbg0<<"Could not load the GUI"<<endl;
    return 1;
  }
  
  dbg1<<"Dino "<<VERSION<<" exiting"<<endl;
  
  return 0;
}
