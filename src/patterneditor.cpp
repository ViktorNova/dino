#include <iostream>

#include "patterneditor.hpp"


PatternEditor::PatternEditor() : m_row_height(8), m_col_width(8), m_max_note(128), 
				 m_pat(NULL) {
  m_colormap  = Colormap::get_system();
  m_bg_color.set_rgb(65535, 65535, 65535);
  m_bg_color2.set_rgb(60000, 60000, 65535);
  m_fg_color.set_rgb(0, 0, 65535);
  m_grid_color.set_rgb(40000, 40000, 40000);
  m_edge_color.set_rgb(0, 0, 0);
  m_hl_color.set_rgb(65535, 0, 0);
  m_colormap->alloc_color(m_bg_color);
  m_colormap->alloc_color(m_bg_color2);
  m_colormap->alloc_color(m_fg_color);
  m_colormap->alloc_color(m_grid_color);
  m_colormap->alloc_color(m_edge_color);
  m_colormap->alloc_color(m_hl_color);
  
  add_events(BUTTON_PRESS_MASK | BUTTON_RELEASE_MASK | BUTTON_MOTION_MASK);
  
  m_added_note = make_pair(-1, -1);
  m_drag_step = -1;
}


void PatternEditor::set_pattern(Pattern* pat) {
  if (this->m_pat != pat) {
    this->m_pat = pat;
    if (m_pat) {
      set_size_request(m_pat->get_length() * m_pat->get_steps() * m_col_width + 1,
		       m_max_note * m_row_height + 1);
    }
    else
      set_size_request();
  }
}


bool PatternEditor::on_button_press_event(GdkEventButton* event) {
  if (!m_pat)
    return false;
  
  if (event->x < m_pat->get_length() * m_pat->get_steps() * m_col_width &&
      event->y < m_max_note * m_row_height) {
    int note = m_max_note - int(event->y) / m_row_height - 1;
    int step = int(event->x) / m_col_width;

    // button one adds notes
    if (event->button == 1) {
      m_added_note = make_pair(step, note);
      m_pat->add_note(m_added_note.first, m_added_note.second, 1);
      update();
    }
    
    // button 2 changes the length
    else if (event->button == 2) {
      int start = m_pat->delete_note(step, note);
      if (start != -1) {
	m_added_note = make_pair(start, note);
	m_pat->add_note(m_added_note.first, m_added_note.second, step - start + 1);
	update();
      }
    }
    
    // button 3 deletes
    else if (event->button == 3) {
      m_pat->delete_note(int(event->x) / m_col_width, 
		      m_max_note - int(event->y) / m_row_height - 1);
      update();
    }

    m_drag_step = step;
    m_drag_note = note;
  }
  return true;
}


bool PatternEditor::on_button_release_event(GdkEventButton* event) {
  if (!m_pat)
    return false;

  if (m_added_note.first != -1 && event->button == 1) {
    int step = int(event->x) / m_col_width;
    if (step < m_added_note.first)
      step = m_added_note.first;
    else if (step >= m_pat->get_length() * m_pat->get_steps())
      step = m_pat->get_length() * m_pat->get_steps() - 1;
    m_pat->add_note(m_added_note.first, m_added_note.second, step - m_added_note.first +1);
    m_added_note = make_pair(-1, -1);
    update();
  }
  return true;
}


bool PatternEditor::on_motion_notify_event(GdkEventMotion* event) {
  if (!m_pat)
    return false;

  if (m_added_note.first != -1 && 
      (event->state & (GDK_BUTTON1_MASK | GDK_BUTTON2_MASK))) {
    int step = int(event->x) / m_col_width;
    int note = m_max_note - int(event->y) / m_row_height - 1;
    if (step == m_drag_step && note == m_drag_note)
      return true;
    if (step < m_added_note.first)
      step = m_added_note.first;
    else if (step >= m_pat->get_length() * m_pat->get_steps())
      step = m_pat->get_length() * m_pat->get_steps() - 1;
    m_pat->add_note(m_added_note.first, m_added_note.second, step - m_added_note.first +1);
    update();
    m_drag_step = step;
    m_drag_note = note;
  }
  else if (event->state & GDK_BUTTON3_MASK) {
    int step = int(event->x) / m_col_width;
    int note = m_max_note - int(event->y) / m_row_height - 1;
    if (step == m_drag_step && note == m_drag_note)
      return true;
    if (step >= 0 && step < m_pat->get_length() * m_pat->get_steps() &&
	note >= 0 && note < m_max_note) {
      m_pat->delete_note(step, note);
      update();
      m_drag_step = step;
      m_drag_note = note;
    }
  }

  return true;
}


void PatternEditor::on_realize() {
  DrawingArea::on_realize();
  RefPtr<Gdk::Window> win = get_window();
  m_gc = GC::create(win);
  m_gc->set_background(m_bg_color);
  m_gc->set_foreground(m_fg_color);
  win->clear();
}


bool PatternEditor::on_expose_event(GdkEventExpose* event) {
  RefPtr<Gdk::Window> win = get_window();
  win->clear();
  if (!m_pat)
    return true;
  
  int width = m_pat->get_length() * m_pat->get_steps() * m_col_width;
  int height = m_max_note * m_row_height;
  for (int b = 0; b < m_pat->get_length(); ++b) {
    if (b % 2 == 0)
      m_gc->set_foreground(m_bg_color);
    else
      m_gc->set_foreground(m_bg_color2);
    win->draw_rectangle(m_gc, true, b * m_pat->get_steps() * m_col_width, 0, 
			m_pat->get_steps() * m_col_width, height);
  }
  m_gc->set_foreground(m_grid_color);
  for (int r = -1; r < m_max_note; ++r) {
    win->draw_line(m_gc, 0, (m_max_note - r - 1) * m_row_height, 
		   width, (m_max_note - r - 1) * m_row_height);
    if (r % 12 == 1 || r % 12 == 3 || 
	r % 12 == 6 || r % 12 == 8 || r % 12 == 10) {
	win->draw_rectangle(m_gc, true, 0, (m_max_note - r - 1) * m_row_height,
			    width, m_row_height);
    }
  }
  for (int c = 0; c < m_pat->get_steps() * m_pat->get_length() + 1; ++c) {
    win->draw_line(m_gc, c * m_col_width, 0, c * m_col_width, height);
  }
  
  const Pattern::NoteMap& notes(m_pat->get_notes());
  Pattern::NoteMap::const_iterator iter;
  for (iter = notes.begin(); iter != notes.end(); ++iter) {
    m_gc->set_foreground(m_fg_color);
    win->draw_rectangle(m_gc, true, iter->first.first * m_col_width + 1, 
			(m_max_note - iter->first.second - 1) * m_row_height + 1, 
			iter->second->length * m_col_width - 1, m_row_height - 1);
    m_gc->set_foreground(m_edge_color);
    win->draw_rectangle(m_gc, false, iter->first.first * m_col_width, 
			(m_max_note - iter->first.second - 1) * m_row_height, 
			iter->second->length * m_col_width, m_row_height);
  }
  
  /*
    gc->set_foreground(hlColor);
    win->draw_rectangle(gc, false, event->area.x, event->area.y,
    event->area.width - 1, event->area.height - 1);
  */
  
  return true;
}


void PatternEditor::update() {
  m_pat->get_dirty_rect(&m_d_min_step, &m_d_min_note, &m_d_max_step, &m_d_max_note);
  RefPtr<Gdk::Window> win = get_window();
  int width = m_pat->get_length() * m_pat->get_steps() * m_col_width+ 1;
  int height = m_max_note * m_row_height + 1;
  win->invalidate_rect(Rectangle(m_d_min_step * m_col_width, 
				 (m_max_note - m_d_max_note - 1) * m_row_height, 
				 (m_d_max_step - m_d_min_step + 1) * m_col_width + 1,
				 (m_d_max_note - m_d_min_note + 1) * m_row_height + 1),
		       false);
  win->process_updates(false);
}
