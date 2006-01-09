#ifndef TRACKLABEL_HPP
#define TRACKLABEL_HPP

#include <string>

#include <gtkmm.h>


namespace Dino {
  class Song;
  class Track;
}


class TrackLabel : public Gtk::DrawingArea {
public:
  
  TrackLabel(const Dino::Song* song = NULL);
  
  void set_track(int id, Dino::Track* track);
  
  virtual void on_realize();
  virtual bool on_expose_event(GdkEventExpose* event);
  virtual bool on_button_press_event(GdkEventButton* event);
  virtual bool on_button_release_event(GdkEventButton* event);
  virtual bool on_motion_notify_event(GdkEventMotion* event);
  
  void update();
  void set_active_track(int id);
  
  sigc::signal<void, int> signal_clicked;
  
private:
  
  void slot_name_changed(const std::string& name);
  
  const Dino::Song* m_song;
  Dino::Track* m_track;
  sigc::connection m_name_connection;
  int m_id;
  int m_width, m_height;
  bool m_is_active;

  Glib::RefPtr<Gdk::GC> m_gc;
  Glib::RefPtr<Gdk::Colormap> m_colormap;
  Gdk::Color m_bg_color, m_fg_color;
  Glib::RefPtr<Pango::Layout> m_layout;
};


#endif
