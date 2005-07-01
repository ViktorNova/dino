#ifndef EVILSCROLLEDWINDOW_HPP
#define EVILSCROLLEDWINDOW_HPP

#include <gtkmm.h>


using namespace Gtk;


class EvilScrolledWindow : public ScrolledWindow {
public:
  
  EvilScrolledWindow(bool horizontal_evilness = true, 
		     bool vertical_evilness = true) 
    : ScrolledWindow(),
      m_hor_evil(horizontal_evilness), m_ver_evil(vertical_evilness) {
    set_policy(POLICY_NEVER, POLICY_NEVER);
  }
  
  virtual void on_size_request(Requisition* req) {
    ScrolledWindow::on_size_request(req);
    if (m_hor_evil)
      req->width = 0;
    if (m_ver_evil)
      req->height = 0;
  }

private:
  bool m_hor_evil;
  bool m_ver_evil;
};


#endif

