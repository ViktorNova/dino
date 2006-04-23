#ifndef PATTERNEDITOR_HPP
#define PATTERNEDITOR_HPP

#include <gtkmm.h>
#include <libglademm.h>

#include "cceditor.hpp"
#include "debug.hpp"
#include "octavelabel.hpp"
#include "noteeditor.hpp"
#include "ruler.hpp"
#include "singletextcombo.hpp"


namespace Dino {
  class Song;
}

class PatternDialog;
class ControllerDialog;


class PatternEditor : public Gtk::VBox {
public:
  
  PatternEditor(BaseObjectType* cobject, 
		const Glib::RefPtr<Gnome::Glade::Xml>& xml);
  
  void set_song(Dino::Song* song);
  void reset_gui();
  
protected:
  
  template <class T>
  static inline T* w(const Glib::RefPtr<Gnome::Glade::Xml>& xml, 
		     const std::string& name) {
    using namespace Dino;
    T* widget = dynamic_cast<T*>(xml->get_widget(name));
    if (widget == 0)
      dbg0<<"Could not load widget "<<name<<" of type "
	  <<demangle(typeid(T).name())<<endl;
    return widget;
  }

  void update_track_combo();
  void update_pattern_combo();
  void update_controller_combo();
  
  void set_active_track(int track);
  void set_active_pattern(int pattern);
  void set_active_controller(long pattern);
  
  void add_controller();
  void delete_controller();
  void add_pattern();
  void delete_pattern();
  void duplicate_pattern();
  void edit_pattern_properties();

  SingleTextCombo m_cmb_track;
  SingleTextCombo m_cmb_pattern;
  SingleTextCombo m_cmb_controller;
  PatternRuler m_pattern_ruler;
  OctaveLabel m_octave_label;

  NoteEditor m_ne;
  CCEditor m_cce;

  PatternDialog* m_dlg_pattern;
  ControllerDialog* m_dlg_controller;

  std::map<std::string, Gtk::ToolButton*> m_toolbuttons;

  sigc::connection m_track_combo_connection;
  sigc::connection m_pattern_combo_connection;
  sigc::connection m_conn_pat_added;
  sigc::connection m_conn_pat_removed;
  sigc::connection m_conn_cont_added;
  sigc::connection m_conn_cont_removed;

  int m_active_track;
  int m_active_pattern;
  long m_active_controller;

  Dino::Song* m_song;
};


#endif
