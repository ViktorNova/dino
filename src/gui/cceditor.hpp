#ifndef CCEDITOR_HPP
#define CCEDITOR_HPP

#include <utility>

#include <gtkmm.h>


namespace Dino {
  class Pattern;
}


class CCEditor : public Gtk::DrawingArea {
public:
  
  CCEditor();
  
  void set_controller(Dino::Pattern* pat, unsigned controller);
  void set_step_width(int width);
  
protected:
  
  virtual bool on_button_press_event(GdkEventButton* event);
  virtual bool on_button_release_event(GdkEventButton* event);
  virtual bool on_motion_notify_event(GdkEventMotion* event);
  virtual void on_realize();
  virtual bool on_expose_event(GdkEventExpose* event);
  
  int value2ypix(int value);
  int ypix2value(int value);
  int step2xpix(int value);
  int xpix2step(int value);
  
  Glib::RefPtr<Gdk::GC> m_gc;
  Gdk::Color m_bg_colour1, m_bg_colour2, m_grid_colour, 
    m_edge_colour, m_fg_colour;
  
  unsigned m_step_width;
  
  Dino::Pattern* m_pat;
  unsigned int m_controller;
};


#endif
