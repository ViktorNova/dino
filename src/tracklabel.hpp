#ifndef TRACKLABEL_HPP
#define TRACKLABEL_HPP

#include <gtkmm.h>

#include "song.hpp"
#include "track.hpp"


using namespace Gdk;
using namespace Glib;
using namespace Gtk;
using namespace Pango;


class TrackLabel : public DrawingArea {
public:
  
  TrackLabel(const Song* song = NULL);
  
  void set_track(int id, Track* track);
  
  virtual void on_realize();
  virtual bool on_expose_event(GdkEventExpose* event);
  virtual bool on_button_press_event(GdkEventButton* event);
  virtual bool on_button_release_event(GdkEventButton* event);
  virtual bool on_motion_notify_event(GdkEventMotion* event);
  
  void update();
  
private:
  
  const Song* m_song;
  Track* m_track;
  int m_id;
  int m_width, m_height;

  RefPtr<GC> m_gc;
  RefPtr<Colormap> m_colormap;
  Gdk::Color m_bg_color, m_fg_color;
  RefPtr<Layout> m_layout;
};


#endif