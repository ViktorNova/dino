/****************************************************************************
   Dino - A simple pattern based MIDI sequencer
   
   Copyright (C) 2006  Lars Luthman <larsl@users.sourceforge.net>
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
****************************************************************************/

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
    m_pat(0),
    m_drag_step(-1) {

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
    int step = xpix2step(int(event->x));
    if (step <= int(m_pat->get_length() * m_pat->get_steps())) {
      int value = ypix2value(int(event->y));
      int min = m_pat->ctrls_find(m_controller)->get_min();
      int max = m_pat->ctrls_find(m_controller)->get_max();
      value = (value < min ? min : value);
      value = (value > max ? max : value);
      m_pat->add_cc(m_pat->ctrls_find(m_controller), step, value);
      if (event->button == 2)
	m_drag_step = step;
      else
	m_drag_step = -1;
    }
  }
  
  // remove a CC event
  else if (event->button == 3) {
    int step;
    if ((step = xpix2step(int(event->x))) < 
	int(m_pat->get_length() * m_pat->get_steps())) {
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
    int step = m_drag_step;
    if (step == -1)
      step = xpix2step(int(event->x));
    if (step <= int(m_pat->get_length() * m_pat->get_steps())) {
      int value = ypix2value(int(event->y));
      int min = m_pat->ctrls_find(m_controller)->get_min();
      int max = m_pat->ctrls_find(m_controller)->get_max();
      value = (value < min ? min : value);
      value = (value > max ? max : value);
      m_pat->add_cc(m_pat->ctrls_find(m_controller), step, value);
    }
  }
  
  // remove a CC event
  else if (event->state & GDK_BUTTON3_MASK) {
    int step;
    if ((step = xpix2step(int(event->x))) < 
	int(m_pat->get_length() * m_pat->get_steps())) {
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
      m_gc->set_foreground(m_fg_colour);
      win->draw_rectangle(m_gc, true, m_step_width * i - 2, 
			  value2ypix(event->get_start()) - 2, 5, 5);
      m_gc->set_foreground(m_edge_colour);
      win->draw_rectangle(m_gc, false, m_step_width * i - 2, 
			  value2ypix(event->get_start()) - 2, 5, 5);
    }
    if (event && i == steps - 1) {
      m_gc->set_foreground(m_fg_colour);
      win->draw_rectangle(m_gc, true, m_step_width * steps - 2, 
			  value2ypix(event->get_end()) - 2, 5, 5);
      m_gc->set_foreground(m_edge_colour);
      win->draw_rectangle(m_gc, false, m_step_width * steps - 2, 
			  value2ypix(event->get_end()) - 2, 5, 5);
    }
      
  }
  
  return true;
}


int CCEditor::value2ypix(int value) {
  if (m_pat) {
    int max = m_pat->ctrls_find(m_controller)->get_max();
    int min = m_pat->ctrls_find(m_controller)->get_min();
    return get_height() - int(get_height() * (value - min) / double(max - min));
  }
  return 0;
}


int CCEditor::ypix2value(int y) { 
  if (m_pat) {
    int max = m_pat->ctrls_find(m_controller)->get_max();
    int min = m_pat->ctrls_find(m_controller)->get_min();
    return min + int((get_height() - y) * (max - min) / double(get_height()));
  }
  return 0;
}


int CCEditor::step2xpix(int value) {
  return value * m_step_width;
}


int CCEditor::xpix2step(int value) {
  return value / m_step_width;
}
