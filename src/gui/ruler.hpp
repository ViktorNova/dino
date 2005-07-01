#ifndef RULER_HPP
#define RULER_HPP

#include <gtkmm.h>

#include "pattern.hpp"
#include "song.hpp"


using namespace Gtk;
using namespace Gdk;
using namespace Glib;
using namespace sigc;


class Ruler : public DrawingArea {
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
  
  signal<void, double, int> signal_clicked;
  
private:
  
  int m_length;
  int m_subs;
  int m_interval;
  int m_div_size;
  int m_height;
  
  RefPtr<GC> m_gc;
  RefPtr<Colormap> m_colormap;
  
};


class PatternRuler : public ::Ruler {
public:
  PatternRuler(const Song& song) : ::Ruler(0, 1, 1, 20, 20), m_song(song) { }
  void set_pattern(int track, int pattern) {
    if (track != -1 && pattern != -1) {
      const Pattern* pat(m_song.get_tracks().find(track)->second->
			 get_patterns().find(pattern)->second);
      set_length(pat->get_length());
      set_subdivisions(pat->get_steps());
      set_interval(1);
      if (pat->get_steps() == pat->get_cc_steps())
	set_division_size(8 * pat->get_steps());
      else
	set_division_size(4 * pat->get_cc_steps());
    }
    else
      set_length(0);
  }
private:
  const Song& m_song;
};


#endif
