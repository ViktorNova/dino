#ifndef PATTERNEDITOR_HPP
#define PATTERNEDITOR_HPP

#include <gtkmm.h>

#include "pattern.hpp"
#include "song.hpp"


using namespace Gdk;
using namespace Glib;
using namespace Gtk;


class PatternEditor : public DrawingArea {
public:
  PatternEditor(Song& song);
  
  void set_pattern(int track, int pattern);
  
protected:
  
  // event handlers
  virtual bool on_button_press_event(GdkEventButton* event);
  virtual bool on_button_release_event(GdkEventButton* event);
  virtual bool on_motion_notify_event(GdkEventMotion* event);
  virtual void on_realize();
  virtual bool on_expose_event(GdkEventExpose* event);
  
  void update();
  
private:
  Song& m_song;

  RefPtr<GC> m_gc;
  RefPtr<Colormap> m_colormap;
  Color m_bg_color, m_bg_color2, m_fg_color, m_grid_color, 
    m_edge_color, m_hl_color;
  int m_row_height;
  int m_col_width;
  int m_max_note;
  
  pair<int, int> m_added_note;
  int m_drag_step;
  int m_drag_note;
  
  RefPtr<Pixmap> m_pix;
  int m_d_min_step, m_d_max_step, m_d_min_note, m_d_max_note;
  Pattern* m_pat;
};


#endif

