#ifndef EVILSCROLLEDWINDOW_HPP
#define EVILSCROLLEDWINDOW_HPP

#include <gtkmm.h>


using namespace Gtk;


class EvilScrolledWindow : public ScrolledWindow {
public:
  
  EvilScrolledWindow(bool horizontalEvilness = true, 
		     bool verticalEvilness = true) 
    : ScrolledWindow(),
      mHorEvil(horizontalEvilness), mVerEvil(verticalEvilness) {
    set_policy(POLICY_NEVER, POLICY_NEVER);
  }
  
  virtual void on_size_request(Requisition* req) {
    ScrolledWindow::on_size_request(req);
    if (mHorEvil)
      req->width = 0;
    if (mVerEvil)
      req->height = 0;
  }

private:
  bool mHorEvil;
  bool mVerEvil;
};


#endif

