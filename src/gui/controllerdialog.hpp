#ifndef CONTROLLERDIALOG_HPP
#define CONTROLLERDIALOG_HPP

#include <gtkmm.h>
#include <libglademm.h>


class ControllerDialog : public Gtk::Dialog {
public:
  
  ControllerDialog(BaseObjectType* obj, 
		   const Glib::RefPtr<Gnome::Glade::Xml>& xml);
  
  std::string get_name() const;
  int get_controller() const;
  
  void set_name(const std::string& name);
  void set_controller(int controller);
  
protected:

  Gtk::Entry* m_ent_name;
  Gtk::SpinButton* m_sbn_controller;

};


#endif
