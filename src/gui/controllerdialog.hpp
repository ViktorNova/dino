#ifndef CONTROLLERDIALOG_HPP
#define CONTROLLERDIALOG_HPP

#include <gtkmm.h>
#include <libglademm.h>

#include "singletextcombo.hpp"


class ControllerDialog : public Gtk::Dialog {
public:
  
  ControllerDialog(BaseObjectType* obj, 
		   const Glib::RefPtr<Gnome::Glade::Xml>& xml);
  
  std::string get_name() const;
  int get_controller() const;
  
  void set_name(const std::string& name);
  void set_controller(int controller);
  
  void refocus();
  
protected:
  
  void update_entry();
  
  Gtk::Entry* m_ent_name;
  SingleTextCombo m_cmb_controller;

  static std::string m_cc_desc[];
};


#endif
