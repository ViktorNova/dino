#include "patterndialog.hpp"


using namespace Gtk;
using namespace Glib;
using namespace Gnome::Glade;


PatternDialog::PatternDialog(BaseObjectType* obj, const RefPtr<Xml>& xml) 
  : Dialog(obj) {
  m_ent_name = dynamic_cast<Entry*>(xml->get_widget("dlgpat_ent_name"));
  m_sbn_length =dynamic_cast<SpinButton*>(xml->get_widget("dlgpat_sbn_length"));
  m_sbn_steps = dynamic_cast<SpinButton*>(xml->get_widget("dlgpat_sbn_steps"));
  manage(m_ent_name);
  manage(m_sbn_length);
  manage(m_sbn_steps);
}


std::string PatternDialog::get_name() const {
  return m_ent_name->get_text();
}


int PatternDialog::get_length() const {
  return m_sbn_length->get_value_as_int();
}


int PatternDialog::get_steps() const {
  return m_sbn_steps->get_value_as_int();
}

  
void PatternDialog::set_name(const std::string& name) {
  m_ent_name->set_text(name);
}


void PatternDialog::set_length(int length) {
  m_sbn_length->set_value(length);
}


void PatternDialog::set_steps(int steps) {
  m_sbn_steps->set_value(steps);
}

