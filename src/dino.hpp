#ifndef DINO_HPP
#define DINO_HPP

#include <gtkmm.h>
#include <libglademm.h>

#include "cceditor.hpp"
#include "patterneditor.hpp"
#include "ruler.hpp"
#include "sequencer.hpp"
#include "singletextcombo.hpp"
#include "song.hpp"


using namespace Gtk;
using namespace std;
using namespace Gnome::Glade;
using namespace Glib;


/** This is the main class. It connects our custom widgets to the rest of the
    GUI and sets up all signals and initial values. */
class Dino {
public:
  Dino(int argc, char** argv, RefPtr<Xml> xml);
  
  Gtk::Window* getWindow();
  
  // menu and toolbutton callbacks
  void slotFileNew();
  void slotFileOpen();
  void slotFileSave();
  void slotFileSaveAs();
  void slotFileQuit();
  
  void slotEditCut();
  void slotEditCopy();
  void slotEditPaste();
  void slotEditDelete();
  void slotEditAddTrack();
  void slotEditDeleteTrack();
  void slotEditAddPattern();
  void slotEditDeletePattern();
  
  void slotTransportPlay();
  void slotTransportStop();
  void slotTransportGoToStart();
  
  void slotHelpAboutDino();
  
  void dontresize(Allocation& a);
  
private:
  
  /** This is a convenience function that returns a pointer of type @c T* to
      the widget with name @c name. If there is no widget in @c xml with that
      name it returns NULL. */
  template <class T>
  inline static T* w(RefPtr<Xml> xml, const char* name) {
    return dynamic_cast<T*>(xml->get_widget(name));
  }
  
  void updateTrackWidgets();
  void updateTrackCombo();
  void updatePatternCombo(int activePattern = -1);
  void updateEditorWidgets();
  
  // internal callbacks
  void slotCCNumberChanged();
  void slotCCEditorSizeChanged();
  
  
  Gtk::Window* window;
  PatternEditor pe;
  CCEditor cce;
  VBox* mVbxTrackEditor;
  
  SingleTextCombo mCmbTrack;
  SingleTextCombo mCmbPattern;
  sigc::connection mTrackPatternConnection;
  sigc::connection mPatternEditorConnection;
  SpinButton* sbCCNumber;
  Label* lbCCDescription;
  SpinButton* sbCCEditorSize;
  ::Ruler mPatternRuler1;
  static char* mCCDescriptions[];
  
  Song mSong;
  
  Dialog* mAboutDialog;
  
  Sequencer mSeq;
};


#endif


// For the Doxygen main page:
/** @mainpage Dino source code documentation
    This source code documentation is generated in order to help people
    (including me) understand how Dino works, or is supposed to work. 
    But it's not meant to be a complete API reference, so many functions and
    types will be undocumented.
    
    Send questions and comments to larsl@users.sourceforge.net. 
*/
