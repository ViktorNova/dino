#include <cassert>
#include <cmath>
#include <iostream>

#include "cceditor.hpp"
#include "pattern.hpp"


CCEditor::CCEditor(Song& song) 
  : m_song(song), m_height(64 + 3), m_col_width(8), m_pat(NULL), 
    m_line_step(-1), m_v_scale((m_height - 3) / 128.0), m_cc_number(0) {
  m_colormap  = Colormap::get_system();
  m_bg_color.set_rgb(65535, 65535, 65535);
  m_bg_color2.set_rgb(60000, 60000, 65535);
  m_grid_color.set_rgb(40000, 40000, 40000);
  m_edge_color.set_rgb(0, 0, 0);
  m_fg_color.set_rgb(0, 40000, 0);
  m_colormap->alloc_color(m_bg_color);
  m_colormap->alloc_color(m_bg_color2);
  m_colormap->alloc_color(m_grid_color);
  m_colormap->alloc_color(m_fg_color);
  m_colormap->alloc_color(m_edge_color);
  
  add_events(BUTTON_PRESS_MASK | BUTTON_RELEASE_MASK | BUTTON_MOTION_MASK);
  set_height(64);
}


void CCEditor::set_pattern(int track, int pattern) {
  Pattern* pat = NULL;
  if (pattern != -1)
    pat = &(m_song.get_tracks()[track].get_patterns()[pattern]);
  if (m_pat != pat) {
    m_pat = pat;
    if (m_pat) {
      int s = m_pat->get_steps(), cc = m_pat->get_cc_steps();
      if (s == cc)
	m_col_width = 8;
      else
	m_col_width = 4;
      m_width = m_col_width * m_pat->get_length() * m_pat->get_cc_steps();
      set_size_request(m_pat->get_length() * m_pat->get_cc_steps() * 
		       m_col_width + 1, m_height);
    }
    else
      set_size_request(-1, m_height);
    update();
  }
}


void CCEditor::set_cc_number(int ccNumber) {
  assert(ccNumber >= 0 && ccNumber < 128);
  m_cc_number = ccNumber;
  update();
}


void CCEditor::set_height(int height) {
  assert(height > 0);
  m_height = height + 3;
  m_v_scale = height / 128.0;
  if (m_pat)
    set_size_request(m_pat->get_length() * m_pat->get_cc_steps() 
		     * m_col_width + 1, m_height);
  else
    set_size_request(-1, m_height);
  
  update();
}


bool CCEditor::on_button_press_event(GdkEventButton* event) {
  if (!m_pat)
    return false;
  
  if (event->button == 1) {
    int step = int(event->x) / m_col_width;
    int value = int(127 - event->y / m_v_scale);
    value = value > 0 ? value : 0;
    m_drag_step = step;
    if (step >= 0 && step < m_pat->get_length() * m_pat->get_cc_steps()) {
      m_pat->add_cc(m_cc_number, step, value);
      update(step);
    }
  }
  else if (event->button == 2) {
    int step = int(event->x) / m_col_width;
    int value = int(127 - event->y / m_v_scale);
    value = value > 0 ? value : 0;
    if (step >= 0 && step < m_pat->get_length() * m_pat->get_cc_steps()) {
      m_line_step = step;
      m_line_value = value;
    }
  }
  else if (event->button == 3) {
    int step = int(event->x) / m_col_width;
    m_drag_step = step;
    if (step >= 0 && step < m_pat->get_length() * m_pat->get_cc_steps()) {
      m_pat->delete_cc(m_cc_number, step);
      update(step);
    }
  }
  return true;
}


bool CCEditor::on_button_release_event(GdkEventButton* event) {
  if (!m_pat)
    return false;
  
  if (event->button == 2 && m_line_step != -1) {
    int step = int(event->x) / m_col_width;
    int value = int(127 - event->y / m_v_scale);
    step = step >= 0 ? step : 0;
    int totalSteps = m_pat->get_length() * m_pat->get_cc_steps();
    step = step < totalSteps ? step : totalSteps - 1;
    value = value > 0 ? value : 0;
    value = value < 128 ? value : 127;
    if (step == m_line_step) {
      m_pat->add_cc(m_cc_number, step, value);
      update();
      m_line_step = -1;
      return true;
    }
    else if (step < m_line_step) {
      int tmp = m_line_step;
      m_line_step = step;
      step = tmp;
      tmp = m_line_value;
      m_line_value = value;
      value = tmp;
    }
    for (int i = 0; i <= step - m_line_step; ++i) {
      int iValue;
      double id = double(i) / (step - m_line_step + 1);
      if (event->state & GDK_SHIFT_MASK)
	iValue = m_line_value + int((value - m_line_value) * pow(id, 3));
      else if (event->state & GDK_CONTROL_MASK)
	iValue = m_line_value - int((value - m_line_value) * (pow(1 - id, 3) - 1));
      else if (event->state & GDK_MOD1_MASK)
	iValue = m_line_value + 
	  int((value - m_line_value) * (-cos(id * 3.141592)/2 + 0.5));
      else
	iValue = m_line_value + int(i*double(value-m_line_value)/(step-m_line_step+1));
      m_pat->add_cc(m_cc_number, m_line_step + i, iValue);
    }
    update();
    m_line_step = -1;
  }
  return true;
}


bool CCEditor::on_motion_notify_event(GdkEventMotion* event) {
  if (!m_pat)
    return false;

  if (event->state & GDK_BUTTON1_MASK) {
    int step = int(event->x) / m_col_width;
    if (step >= 0 && step < m_pat->get_length() * m_pat->get_cc_steps()) {
      int value = int(127 - event->y / m_v_scale);
      value = value > 127 ? 127 : value;
      value = value < 0 ? 0 : value;
      m_pat->add_cc(m_cc_number, step, value);
      update(step);
      m_drag_step = step;
    }
  }
  else if (event->state & GDK_BUTTON3_MASK) {
    int step = int(event->x) / m_col_width;
    if (step == m_drag_step)
      return true;
    if (step >= 0 && step < m_pat->get_length() * m_pat->get_cc_steps()) {
      m_pat->delete_cc(m_cc_number, step);
      update(step);
      m_drag_step = step;
    }
  }

  return true;
}


void CCEditor::on_realize() {
  DrawingArea::on_realize();
  RefPtr<Gdk::Window> win = get_window();
  m_gc = GC::create(win);
  m_gc->set_background(m_bg_color);
  m_gc->set_foreground(m_fg_color);
  win->clear();
}


bool CCEditor::on_expose_event(GdkEventExpose* event) {
  RefPtr<Gdk::Window> win = get_window();
  win->clear();
  if (!m_pat)
    return true;
  
  int width = m_pat->get_length() * m_pat->get_cc_steps() * m_col_width;
  int height = m_height;
  
  // draw background
  for (int b = 0; b < m_pat->get_length(); ++b) {
    if (b % 2 == 0)
      m_gc->set_foreground(m_bg_color);
    else
      m_gc->set_foreground(m_bg_color2);
    win->draw_rectangle(m_gc, true, b * m_pat->get_cc_steps() * m_col_width, 
			0, m_pat->get_cc_steps() * m_col_width, height);
  }
  m_gc->set_foreground(m_grid_color);
  for (int c = 0; c < m_pat->get_cc_steps() * m_pat->get_length() + 1; ++c) {
    win->draw_line(m_gc, c * m_col_width, 0, c * m_col_width, height);
  }
  
  const Pattern::CCData& data = m_pat->get_cc(m_cc_number);
  std::map<int, Pattern::CCEvent*>::const_iterator iter;
  iter = data.changes.upper_bound(event->area.x / m_col_width - 2);
  for ( ; iter != data.changes.end() && 
	  iter->first <= (event->area.x + event->area.width) / m_col_width + 1;
	++iter) {
    m_gc->set_foreground(m_fg_color);
    win->draw_rectangle(m_gc, true, iter->first * m_col_width, 
			int((128 - iter->second->value - 1) * m_v_scale), 
			m_col_width, 3);
    m_gc->set_foreground(m_edge_color);
    win->draw_rectangle(m_gc, false, iter->first * m_col_width, 
			int((128 - iter->second->value - 1) * m_v_scale), 
			m_col_width, 3);
  }
  
  return true;
}


void CCEditor::update(int step) {
  RefPtr<Gdk::Window> win = get_window();
  if (win) {
    if (step == -1) {
      win->invalidate_rect(Rectangle(0, 0, m_width, m_height), false);
      win->process_updates(false);
    }
    else {
      win->invalidate_rect(Rectangle(step * m_col_width, 0, m_col_width + 1, m_height),
			   false);
      win->process_updates(false);
    }
  }
}
