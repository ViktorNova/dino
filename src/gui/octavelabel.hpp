#ifndef OCTAVELABEL_HPP
#define OCTAVELABEL_HPP

#include <gtkmm.h>


class OctaveLabel : public Gtk::DrawingArea {
public:
  OctaveLabel(int width, int note_height);
  
  virtual void on_realize();
  virtual bool on_expose_event(GdkEventExpose* event);

private:
  
  int m_width;
  int m_note_height;
  
  Glib::RefPtr<Gdk::GC> m_gc;
  Glib::RefPtr<Gdk::Colormap> m_colormap;
  Gdk::Color m_bg_color, m_fg_color;  
};


#endif
