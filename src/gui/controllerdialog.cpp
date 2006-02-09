#include "controllerdialog.hpp"

using namespace Glib;
using namespace Gtk;
using namespace Gnome::Glade;


ControllerDialog::ControllerDialog(BaseObjectType* obj, 
				   const RefPtr<Xml>& xml) 
  : Dialog(obj) {
  m_ent_name = dynamic_cast<Entry*>(xml->get_widget("dlgcont_ent_name"));
  m_sbn_controller =
    dynamic_cast<SpinButton*>(xml->get_widget("dlgcont_sbn_controller"));
  manage(m_ent_name);
  manage(m_sbn_controller);
}
  

std::string ControllerDialog::get_name() const {
  return m_ent_name->get_text();
}


int ControllerDialog::get_controller() const {
  return m_sbn_controller->get_value_as_int();
}
  

void ControllerDialog::set_name(const std::string& name) {
  m_ent_name->set_text(name);
}


void ControllerDialog::set_controller(int controller) {
  m_sbn_controller->set_value(controller);
}


void ControllerDialog::refocus() {
  m_ent_name->select_region(0, -1);
  m_ent_name->grab_focus();
}
