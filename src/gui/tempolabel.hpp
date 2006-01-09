#ifndef TEMPOLABEL_HPP
#define TEMPOLABEL_HPP

#include <gtkmm.h>


namespace Dino {
  class Song;
}


class TempoLabel : public Gtk::DrawingArea {
public:
  
  TempoLabel(const Dino::Song* song = NULL);
  
  virtual void on_realize();
  virtual bool on_expose_event(GdkEventExpose* event);
  
  void update();
  
private:
  
  const Dino::Song* m_song;
  int m_width, m_height;

  Glib::RefPtr<Gdk::GC> m_gc;
  Glib::RefPtr<Gdk::Colormap> m_colormap;
  Gdk::Color m_bg_color, m_fg_color;
  Glib::RefPtr<Pango::Layout> m_layout;
};


#endif
