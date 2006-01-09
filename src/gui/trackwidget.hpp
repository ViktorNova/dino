#ifndef TRACKWIDGET_HPP
#define TRACKWIDGET_HPP

#include <gtkmm.h>


namespace Dino {
  class Song;
  class Track;
}


class TrackWidget : public Gtk::DrawingArea {
public:
  
  TrackWidget(const Dino::Song* song = NULL);
  
  void set_track(Dino::Track* track);

  virtual void on_realize();
  virtual bool on_expose_event(GdkEventExpose* event);
  virtual bool on_button_press_event(GdkEventButton* event);
  virtual bool on_button_release_event(GdkEventButton* event);
  virtual bool on_motion_notify_event(GdkEventMotion* event);
  
  void slot_insert_pattern(int pattern, int position);
  void slot_length_changed(int length);
  
  void update();
  
  void set_current_beat(int beat);
  
  sigc::signal<void, int> signal_clicked;
  
private:
  
  const Dino::Song* m_song;
  Dino::Track* m_track;
  int m_col_width;

  Glib::RefPtr<Gdk::GC> m_gc;
  Glib::RefPtr<Gdk::Colormap> m_colormap;
  Gdk::Color m_bg_color, m_bg_color2, m_fg_color, m_grid_color, m_edge_color, 
    m_hl_color;
  
  int m_drag_beat, m_drag_pattern;
  
  int m_current_beat;
  
  Gtk::Menu m_pattern_menu;
};


#endif
