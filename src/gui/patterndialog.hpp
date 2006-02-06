#ifndef PATTERNDIALOG_HPP
#define PATTERNDIALOG_HPP

#include <gtkmm.h>
#include <libglademm.h>

#include "singletextcombo.hpp"


class PatternDialog : public Gtk::Dialog {
public:
  
  PatternDialog(BaseObjectType* obj, 
		const Glib::RefPtr<Gnome::Glade::Xml>& xml);
  
  std::string get_name() const;
  int get_length() const;
  int get_steps() const;
  
  void set_name(const std::string& name);
  void set_length(int length);
  void set_steps(int length);
  
protected:

  Gtk::Entry* m_ent_name;
  Gtk::SpinButton* m_sbn_length;
  Gtk::SpinButton* m_sbn_steps;

};


#endif
