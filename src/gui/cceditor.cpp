#include <iostream>

#include "cceditor.hpp"


using namespace Glib;
using namespace Gtk;
using namespace Gdk;
using namespace std;

  
CCEditor::CCEditor() 
  : m_bg_colour1("#FFFFFF"),
    m_bg_colour2("#EAEAFF"),
    m_grid_colour("#9C9C9C"),
    m_edge_colour("#000000"),
    m_fg_colour("#008000"),
    m_steps(32),
    m_step_width(10),
    m_beat_length(4) {

  RefPtr<Colormap> cmap = Colormap::get_system();
  cmap->alloc_color(m_bg_colour1);
  cmap->alloc_color(m_bg_colour2);
  cmap->alloc_color(m_grid_colour);
  cmap->alloc_color(m_edge_colour);
  cmap->alloc_color(m_fg_colour);
  
  set_size_request(m_steps * m_step_width);
}


bool CCEditor::on_button_press_event(GdkEventButton* event) {
  return false;
}


bool CCEditor::on_button_release_event(GdkEventButton* event) {
  return false;
}


bool CCEditor::on_motion_notify_event(GdkEventMotion* event) {
  return false;
}


void CCEditor::on_realize() {
  DrawingArea::on_realize();
  RefPtr<Gdk::Window> win = get_window();
  m_gc = GC::create(win);
  win->clear();
}


bool CCEditor::on_expose_event(GdkEventExpose* event) {
  RefPtr<Gdk::Window> win = get_window();
  for (unsigned i = 0; i < m_steps; i += m_beat_length) {
    if ((i / m_beat_length) % 2 == 0)
      m_gc->set_foreground(m_bg_colour1);
    else
      m_gc->set_foreground(m_bg_colour2);
    win->draw_rectangle(m_gc, true, i * m_step_width, 0, 
			m_beat_length * m_step_width, get_height());
  }
  m_gc->set_foreground(m_grid_colour);
  for (unsigned i = 0; i < m_steps; ++i) {
    win->draw_line(m_gc, (i + 1) * m_step_width, 0, 
		   (i + 1) * m_step_width, get_height());
  }
  m_gc->set_foreground(m_edge_colour);
  for (unsigned i = 0; i < m_steps; ++i)
    draw_cc(i, i);
  draw_cc(0, 127);
  return true;
}


void CCEditor::draw_cc(unsigned step, int value) {
  m_gc->set_foreground(m_fg_colour);
  get_window()->draw_rectangle(m_gc, true, 
			       m_step_width * step, value2ypix(value), 
			       m_step_width, 4);
  m_gc->set_foreground(m_edge_colour);
  get_window()->draw_rectangle(m_gc, false, 
			       m_step_width * step, value2ypix(value), 
			       m_step_width, 4);
}


int CCEditor::value2ypix(int value) {
  return int((127 - value) * (get_height() - 4) / 128.0);
}


int CCEditor::ypix2value(int y) { 
  return 128 - int(129.0 / get_height() * y);
}
