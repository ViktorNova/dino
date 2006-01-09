#ifndef RULER_HPP
#define RULER_HPP

#include <gtkmm.h>


namespace Dino {
  class Song;
}


class Ruler : public Gtk::DrawingArea {
public:
  Ruler(int length, int subs, int interval, int size, int height);
  
  void set_length(int length);
  void set_subdivisions(int subs);
  void set_interval(int interval);
  void set_division_size(int size);

  virtual void on_realize();
  virtual bool on_expose_event(GdkEventExpose* event);
  virtual bool on_button_press_event(GdkEventButton* event);

public:
  
  sigc::signal<void, double, int> signal_clicked;
  
private:
  
  int m_length;
  int m_subs;
  int m_interval;
  int m_div_size;
  int m_height;
  
  Glib::RefPtr<Gdk::GC> m_gc;
  Glib::RefPtr<Gdk::Colormap> m_colormap;
  
};


class PatternRuler : public ::Ruler {
public:

  PatternRuler(const Dino::Song& song);

  void set_pattern(int track, int pattern);

private:

  const Dino::Song& m_song;

};


#endif
