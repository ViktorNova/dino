#ifndef TEMPOWIDGET_HPP
#define TEMPOWIDGET_HPP

#include <gtkmm.h>

#include "song.hpp"


using namespace Gdk;
using namespace Glib;
using namespace Gtk;


class TempoWidget : public DrawingArea {
public:
  
  TempoWidget(const Song* song = NULL);
  
  virtual void on_realize();
  virtual bool on_expose_event(GdkEventExpose* event);
  virtual bool on_button_press_event(GdkEventButton* event);
  virtual bool on_button_release_event(GdkEventButton* event);
  virtual bool on_motion_notify_event(GdkEventMotion* event);
  
  void update();
  
private:
  
  const Song* m_song;
  int m_col_width;

  RefPtr<GC> m_gc;
  RefPtr<Colormap> m_colormap;
  Gdk::Color m_bg_color, m_bg_color2, m_fg_color, m_grid_color, m_edge_color, 
    m_hl_color;
  
  int m_drag_beat;
};


#endif
