#ifndef PATTERNEDITOR_HPP
#define PATTERNEDITOR_HPP

#include <utility>

#include <gtkmm.h>


namespace Dino {
  class MIDIEvent;
  class Pattern;
}

using namespace Gdk;
using namespace Glib;
using namespace Gtk;
using namespace std;
using namespace Dino;
using namespace Pango;


class PatternEditor : public DrawingArea {
public:
  PatternEditor();
  
  //void set_pattern(int track, int pattern);
  void set_pattern(Pattern* pattern);
  
protected:
  
  // event handlers
  virtual bool on_button_press_event(GdkEventButton* event);
  virtual bool on_button_release_event(GdkEventButton* event);
  virtual bool on_motion_notify_event(GdkEventMotion* event);
  virtual void on_realize();
  virtual bool on_expose_event(GdkEventExpose* event);
  
  void draw_note(const MIDIEvent* event);
  void draw_velocity_box(const MIDIEvent* event);
  void update();
  
private:
  
  /** This is used to figure out what to do when a motion event is received.
      It is set on a button press event and reset (set to NoOperation) on a
      button release event. */
  enum DragOperation {
    NoOperation,
    ChangingNoteLength,
    ChangingNoteVelocity,
    DeletingNotes
  } m_drag_operation;
  
  RefPtr<GC> m_gc;
  RefPtr<Colormap> m_colormap;
  Gdk::Color m_bg_color, m_bg_color2, m_fg_color1, m_fg_color2, m_grid_color, 
    m_edge_color, m_hl_color;
  Gdk::Color m_note_colors[16];
  int m_row_height;
  int m_col_width;
  int m_max_note;
  
  pair<int, int> m_added_note;
  int m_drag_step;
  int m_drag_note;
  int m_drag_y;
  int m_drag_start_vel;
  
  RefPtr<Pixmap> m_pix;
  int m_d_min_step, m_d_max_step, m_d_min_note, m_d_max_note;
  Pattern* m_pat;
  RefPtr<Layout> m_layout;
};


#endif

