#ifndef TRACKWIDGET_HPP
#define TRACKWIDGET_HPP

#include <gtkmm.h>


namespace Dino {
  class Song;
  class Track;
}

using namespace Gdk;
using namespace Glib;
using namespace Gtk;
using namespace sigc;
using namespace Dino;


class TrackWidget : public DrawingArea {
public:
  
  TrackWidget(const Song* song = NULL);
  
  void set_track(Track* track);

  virtual void on_realize();
  virtual bool on_expose_event(GdkEventExpose* event);
  virtual bool on_button_press_event(GdkEventButton* event);
  virtual bool on_button_release_event(GdkEventButton* event);
  virtual bool on_motion_notify_event(GdkEventMotion* event);
  
  void slot_insert_pattern(int pattern, int position);
  void slot_length_changed(int length);
  
  void update();
  
  void set_current_beat(int beat);
  
  signal<void, int> signal_clicked;
  
private:
  
  const Song* m_song;
  Track* m_track;
  int m_col_width;

  RefPtr<GC> m_gc;
  RefPtr<Colormap> m_colormap;
  Gdk::Color m_bg_color, m_bg_color2, m_fg_color, m_grid_color, m_edge_color, 
    m_hl_color;
  
  int m_drag_beat, m_drag_pattern;
  
  int m_current_beat;
  
  Menu m_pattern_menu;
};


#endif
