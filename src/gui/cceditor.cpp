#include <iostream>

#include "cceditor.hpp"
#include "pattern.hpp"

using namespace Glib;
using namespace Gtk;
using namespace Gdk;
using namespace std;
using namespace Dino;

  
CCEditor::CCEditor() 
  : m_bg_colour1("#FFFFFF"),
    m_bg_colour2("#EAEAFF"),
    m_grid_colour("#9C9C9C"),
    m_edge_colour("#000000"),
    m_fg_colour("#008000"),
    m_step_width(8),
    m_pat(NULL) {

  RefPtr<Colormap> cmap = Colormap::get_system();
  cmap->alloc_color(m_bg_colour1);
  cmap->alloc_color(m_bg_colour2);
  cmap->alloc_color(m_grid_colour);
  cmap->alloc_color(m_edge_colour);
  cmap->alloc_color(m_fg_colour);

  add_events(BUTTON_PRESS_MASK | BUTTON_RELEASE_MASK | BUTTON_MOTION_MASK);
}


void CCEditor::set_controller(Dino::Pattern* pat, unsigned controller) {
  m_pat = pat;
  m_controller = controller;
  if (m_pat) {
    set_size_request(m_pat->get_length() * m_pat->get_steps() * m_step_width,
		     -1);
    slot<void> draw = mem_fun(*this, &CCEditor::queue_draw);
    m_pat->signal_cc_added.connect(sigc::hide(sigc::hide(sigc::hide(draw))));
    m_pat->signal_cc_changed.connect(sigc::hide(sigc::hide(sigc::hide(draw))));
    m_pat->signal_cc_removed.connect(sigc::hide(sigc::hide(draw)));
    m_pat->signal_length_changed.connect(sigc::hide(draw));
    m_pat->signal_steps_changed.connect(sigc::hide(draw));
  }
  queue_draw();
}


void CCEditor::set_step_width(int width) {
  assert(width > 0);
  m_step_width = width;
  if (m_pat) {
    set_size_request(m_pat->get_length() * m_pat->get_steps() * m_step_width, 
		     -1);
    queue_draw();
  }
}


bool CCEditor::on_button_press_event(GdkEventButton* event) {
  if (!m_pat)
    return false;
  
  // add a CC event
  if (event->button == 1 || event->button == 2) {
    int step;
    if ((step = xpix2step(int(event->x))) < 
	m_pat->get_length() * m_pat->get_steps()) {
      int value = ypix2value(int(event->y));
      value = (value < 0 ? 0 : value);
      value = (value >127 ? 127 : value);
      m_pat->add_cc(m_pat->ctrls_find(m_controller), step, value);
    }
  }
  
  // remove a CC event
  else if (event->button == 3) {
    int step;
    if ((step = xpix2step(int(event->x))) < 
	m_pat->get_length() * m_pat->get_steps()) {
      m_pat->remove_cc(m_pat->ctrls_find(m_controller), step);
    }
  }

  return true;
}


bool CCEditor::on_button_release_event(GdkEventButton* event) {
  return false;
}


bool CCEditor::on_motion_notify_event(GdkEventMotion* event) {
  if (!m_pat)
    return false;
  
  // add a CC event
  if (event->state & GDK_BUTTON1_MASK || event->state & GDK_BUTTON2_MASK) {
    int step;
    if ((step = xpix2step(int(event->x))) < 
	m_pat->get_length() * m_pat->get_steps()) {
      int value = ypix2value(int(event->y));
      value = (value < 0 ? 0 : value);
      value = (value >127 ? 127 : value);
      m_pat->add_cc(m_pat->ctrls_find(m_controller), step, value);
    }
  }
  
  // remove a CC event
  else if (event->state & GDK_BUTTON3_MASK) {
    int step;
    if ((step = xpix2step(int(event->x))) < 
	m_pat->get_length() * m_pat->get_steps()) {
      m_pat->remove_cc(m_pat->ctrls_find(m_controller), step);
    }
  }

  return true;
}


void CCEditor::on_realize() {
  DrawingArea::on_realize();
  RefPtr<Gdk::Window> win = get_window();
  m_gc = GC::create(win);
  win->clear();
}


bool CCEditor::on_expose_event(GdkEventExpose* event) {
  
  if (!m_pat)
    return true;
  
  RefPtr<Gdk::Window> win = get_window();
  
  unsigned steps = m_pat->get_length() * m_pat->get_steps();
  unsigned spb = m_pat->get_steps();
  
  for (unsigned i = 0; i < steps; i += spb) {
    if ((i / spb) % 2 == 0)
      m_gc->set_foreground(m_bg_colour1);
    else
      m_gc->set_foreground(m_bg_colour2);
    win->draw_rectangle(m_gc, true, i * m_step_width, 0, 
			spb * m_step_width, get_height());
  }
  
  m_gc->set_foreground(m_grid_colour);
  for (unsigned i = 0; i < steps; ++i) {
    win->draw_line(m_gc, (i + 1) * m_step_width, 0, 
		   (i + 1) * m_step_width, get_height());
  }
  
  
  for (unsigned i = 0; i < steps; ++i) {
    const InterpolatedEvent* event = 
      m_pat->ctrls_find(m_controller)->get_event(i);
    if (event && event->get_step() == i) {
      m_gc->set_foreground(m_edge_colour);
      win->draw_line(m_gc, m_step_width * i, value2ypix(event->get_start()),
		     m_step_width * (i + event->get_length()),
		     value2ypix(event->get_end()));
    }
  }
  
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


int CCEditor::step2xpix(int value) {
  return value * m_step_width;
}


int CCEditor::xpix2step(int value) {
  return value / m_step_width;
}
