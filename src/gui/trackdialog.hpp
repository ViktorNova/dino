#ifndef TRACKDIALOG_HPP
#define TRACKDIALOG_HPP

#include <gtkmm.h>
#include <libglademm.h>

#include "singletextcombo.hpp"


namespace Dino {
  class Sequencer;
}

class TrackDialog : public Gtk::Dialog {
public:
  
  TrackDialog(BaseObjectType* obj, const Glib::RefPtr<Gnome::Glade::Xml>& xml);
  
  std::string get_name() const;
  std::string get_port() const;
  int get_channel() const;
  
  void set_name(const std::string& name);
  void set_channel(int channel);
  void update_ports(const Dino::Sequencer* seq);
  
protected:

  Gtk::Entry* m_ent_name;
  SingleTextCombo m_cmb_port;
  Gtk::SpinButton* m_sbn_channel;

};


#endif
