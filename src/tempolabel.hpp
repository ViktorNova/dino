#ifndef TEMPOLABEL_HPP
#define TEMPOLABEL_HPP

#include <gtkmm.h>

#include "song.hpp"


using namespace Gdk;
using namespace Glib;
using namespace Gtk;
using namespace Pango;


class TempoLabel : public DrawingArea {
public:
  
  TempoLabel(const Song* song = NULL);
  
  virtual void on_realize();
  virtual bool on_expose_event(GdkEventExpose* event);
  
  void update();
  
private:
  
  const Song* m_song;
  int m_width, m_height;

  RefPtr<GC> m_gc;
  RefPtr<Colormap> m_colormap;
  Gdk::Color m_bg_color, m_fg_color;
  RefPtr<Layout> m_layout;
};


#endif
