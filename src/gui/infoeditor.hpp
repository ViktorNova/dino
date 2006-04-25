#ifndef INFOEDITOR_HPP
#define INFOEDITOR_HPP

#include <gtkmm.h>
#include <libglademm.h>

#include "debug.hpp"


namespace Dino {
  class Song;
}


class InfoEditor : public Gtk::Table {
public:
  
  InfoEditor(BaseObjectType* cobject, 
		 const Glib::RefPtr<Gnome::Glade::Xml>& xml);
  
  void set_song(Dino::Song* song);
  
  void reset_gui();
  
protected:
  
  void update_info(const std::string& info);
  
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

  Gtk::Entry* m_ent_title;
  Gtk::Entry* m_ent_author;
  Gtk::TextView* m_text_info;
  
  Dino::Song* m_song;
};


#endif
