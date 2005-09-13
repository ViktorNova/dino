#ifndef CCEDITOR_HPP
#define CCEDITOR_HPP

#include <gtkmm.h>


namespace Dino {
  class Pattern;
  class Song;
}

using namespace Gdk;
using namespace Glib;
using namespace Gtk;
using namespace Dino;


class CCEditor : public DrawingArea {
public:
  CCEditor(Song& song);
  
  void set_pattern(int track, int pattern);
  void set_cc_number(int cc_number);
  void set_height(int height);  
  
protected:
  
  // event handlers
  virtual bool on_button_press_event(GdkEventButton* event);
  virtual bool on_button_release_event(GdkEventButton* event);
  virtual bool on_motion_notify_event(GdkEventMotion* event);
  virtual void on_realize();
  virtual bool on_expose_event(GdkEventExpose* event);
  
  void update(int step = -1);
  
private:
  Song& m_song;
  
  RefPtr<GC> m_gc;
  RefPtr<Colormap> m_colormap;
  Color m_bg_color, m_bg_color2, m_grid_color, m_fg_color, m_edge_color;
  int m_col_width;
  int m_width, m_height;
  double m_v_scale;
  
  Pattern* m_pat;
  int m_cc_number;
  
  int m_drag_step;
  int m_line_step;
  int m_line_value;
  
  Menu m_shape_menu;
};


#endif

