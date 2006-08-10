/****************************************************************************
   Dino - A simple pattern based MIDI sequencer
   
   Copyright (C) 2006  Lars Luthman <lars.luthman@gmail.com>
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation, 
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
****************************************************************************/

#include <csignal>
#include <iostream>
#include <string>

#include <glibmm.h>
#include <gtkmm.h>
#include <sigc++/slot.h>

#include "debug.hpp"
#include "dinogui.hpp"


using namespace std;
using namespace Gtk;
using namespace Glib;
using namespace Dino;


static void print_version() {
  cout<<"Dino "<<VERSION<<endl
      <<"Copyright (C) "<<CR_YEAR<<" Lars Luthman <lars.luthman@gmail.com>"<<endl
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
  
  // create the GUI
  dbg1<<"Initialising gtkmm"<<endl;
  Main kit(argc, argv);
  dbg1<<"Creating GUI"<<endl;
  DinoGUI dino(argc, argv);

  if (dino.is_valid()) {
    
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
    dbg1<<"Starting GUI"<<endl;
    dino.get_window().show_all();
    Main::run(dino.get_window());
  }
  
  dbg1<<"Dino "<<VERSION<<" exiting"<<endl;
  
  return 0;
}
