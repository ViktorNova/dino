/****************************************************************************
   Dino - A simple pattern based MIDI sequencer
   
   Copyright (C) 2004  Lars Luthman <larsl@users.sourceforge.net>
   
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


void printVersion() {
  cout<<"Dino "<<VERSION<<endl
      <<"Copyright (C) 2005 Lars Luthman <larsl@users.sourceforge.net>"<<endl
      <<"This program comes with ABSOLUTELY NO WARRANTY."<<endl
      <<"This is free software, and you are welcome to redistribute it"<<endl
      <<"under certain conditions; see the file COPYING for details."<<endl;
}


int main(int argc, char** argv) {
  
  // print the version if requested
  if (argc > 1 && !strcmp(argv[1], "--version")) {
    printVersion();
    return 0;
  }
  
  // load the GUI
  dbg1<<"Initialising gtkmm"<<endl;
  Main kit(argc, argv);
  dbg1<<"Loading GUI"<<endl;
  RefPtr<Xml> refXml = Xml::create("dino.glade");
  DinoGUI dino(argc, argv, refXml);
  
  // run
  if (dino.get_window()) {
    dbg1<<"Starting GUI"<<endl;
    Main::run(*dino.get_window());
  }
  else {
    cerr<<"Could not load the GUI!"<<endl;
    return 1;
  }
  
  return 0;
}




