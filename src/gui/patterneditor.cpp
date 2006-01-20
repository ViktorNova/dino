#include <cstdlib>
#include <iostream>

#include "debug.hpp"
#include "noteevent.hpp"
#include "pattern.hpp"
#include "patterneditor.hpp"
#include "song.hpp"
#include "track.hpp"


using namespace Gdk;
using namespace Pango;
using namespace Gtk;
using namespace Dino;


PatternEditor::PatternEditor() 
  : m_drag_operation(NoOperation), m_row_height(8), 
    m_col_width(8), m_max_note(128), m_drag_y(-1), 
    m_drag_start_vel(-1), m_pat(NULL) {
  
  // initialise colours
  m_colormap = Colormap::get_system();
  m_bg_color.set_rgb(65535, 65535, 65535);
  m_bg_color2.set_rgb(60000, 60000, 65535);
  m_fg_color1.set_rgb(0, 0, 65535);
  m_grid_color.set_rgb(40000, 40000, 40000);
  m_edge_color.set_rgb(0, 0, 0);
  m_hl_color.set_rgb(65535, 0, 0);
  
  // initialise note colours for different velocities
  gushort red1 = 40000, green1 = 45000, blue1 = 40000;
  gushort red2 = 0, green2 = 30000, blue2 = 0;
  for (int i = 0; i < 16; ++i)
    m_note_colors[i].set_rgb(gushort((red1 * (15 - i) + red2 * i) / 15.0),
			     gushort((green1 * (15 - i) + green2 * i) / 15.0),
			     gushort((blue1 * (15 - i) + blue2 * i) / 15.0));
  
  // allocate all colours
  m_colormap->alloc_color(m_bg_color);
  m_colormap->alloc_color(m_bg_color2);
  m_colormap->alloc_color(m_fg_color1);
  m_colormap->alloc_color(m_grid_color);
  m_colormap->alloc_color(m_edge_color);
  m_colormap->alloc_color(m_hl_color);
  for (int i = 0; i < 16; ++i)
    m_colormap->alloc_color(m_note_colors[i]);

  m_layout = Layout::create(get_pango_context());
  m_layout->set_text("127");
  add_events(BUTTON_PRESS_MASK | BUTTON_RELEASE_MASK | BUTTON_MOTION_MASK);
  
  m_added_note = make_pair(-1, -1);
  m_drag_step = -1;
}


void PatternEditor::set_pattern(Pattern* pattern) {
  if (pattern != m_pat) {
    m_pat = pattern;
    if (m_pat) {
      int s = m_pat->get_steps();
      m_col_width = 8;
      m_pat->signal_note_added.
	connect(sigc::hide(sigc::hide(sigc::hide(mem_fun(*this, &PatternEditor::queue_draw)))));
      m_pat->signal_note_removed.
	connect(sigc::hide(sigc::hide(mem_fun(*this, &PatternEditor::queue_draw))));
      m_pat->signal_note_changed.
	connect(sigc::hide(sigc::hide(sigc::hide(mem_fun(*this, &PatternEditor::queue_draw)))));
      set_size_request(m_pat->get_length() * s * m_col_width + 1, 
		       m_max_note * m_row_height + 1);
      queue_draw();
    }
    else
      set_size_request();
  }
}


bool PatternEditor::on_button_press_event(GdkEventButton* event) {
  if (!m_pat)
    return false;
  
  // check that we're on the editor surface
  if (event->x < m_pat->get_length() * m_pat->get_steps() * m_col_width &&
      event->y < m_max_note * m_row_height) {
    
    int note = m_max_note - int(event->y) / m_row_height - 1;
    int step = int(event->x) / m_col_width;

    m_drag_y = int(event->y);
    m_drag_step = step;
    m_drag_note = note;
    
    switch (event->button) {
      
      // button 1 adds notes
    case 1: {
      m_pat->add_note(step, note, 64, 1);
      m_added_note = make_pair(step, note);
      m_drag_operation = ChangingNoteLength;
      break;
    }
    
    // button 2 changes the velocity or length
    case 2: {
      Pattern::NoteIterator iterator = m_pat->find_note(step, note);
      if (iterator) {
	if (event->state & GDK_CONTROL_MASK) {
	  m_drag_operation = ChangingNoteVelocity;
	  m_drag_start_vel = iterator->get_velocity();
	  queue_draw();
	}
	else {
	  m_pat->resize_note(iterator, step - iterator->get_step() + 1);
	  m_added_note = make_pair(iterator->get_step(), note);
	  m_drag_operation = ChangingNoteLength;
	}
      }
      break;
    }
    
    // button 3 deletes
    case 3: {
      Pattern::NoteIterator iterator = 
	m_pat->find_note(int(event->x) / m_col_width, 
			 m_max_note - int(event->y) / m_row_height - 1);
      if (iterator)
	m_pat->delete_note(iterator);
      m_drag_operation = DeletingNotes;
      break;
    }
    }
  }
  
  return true;
}


bool PatternEditor::on_button_release_event(GdkEventButton* event) {
  if (!m_pat)
    return false;

  if (m_drag_operation == ChangingNoteLength) {
    int step = int(event->x) / m_col_width;
    if (step < m_added_note.first)
      step = m_added_note.first;
    else if (step >= m_pat->get_length() * m_pat->get_steps())
      step = m_pat->get_length() * m_pat->get_steps() - 1;
    Pattern::NoteIterator iterator = 
      m_pat->find_note(m_added_note.first, m_added_note.second);
    m_pat->resize_note(iterator, step - m_added_note.first + 1);
    m_added_note = make_pair(-1, -1);
  }
  
  if (m_drag_operation == ChangingNoteVelocity) {
    m_drag_operation = NoOperation;
    queue_draw();
  }

  m_drag_operation = NoOperation;
  
  return true;
}


bool PatternEditor::on_motion_notify_event(GdkEventMotion* event) {
  if (!m_pat)
    return false;
  
  switch (m_drag_operation) {

  case ChangingNoteVelocity: {
    Pattern::NoteIterator iterator = m_pat->find_note(m_drag_step, m_drag_note);
    if (iterator) {
      double dy = m_drag_y - int(event->y);
      int velocity = int(m_drag_start_vel + dy);
      velocity = (velocity < 0 ? 0 : (velocity > 127 ? 127 : velocity));
      m_pat->set_velocity(iterator, velocity);
      queue_draw();
    }
    break;
  }
    
  case ChangingNoteLength: {
    int step = int(event->x) / m_col_width;
    int note = m_max_note - int(event->y) / m_row_height - 1;
    
    // or changing the note length
    if (step == m_drag_step && note == m_drag_note)
      return true;
    if (step < m_added_note.first)
      step = m_added_note.first;
    else if (step >= m_pat->get_length() * m_pat->get_steps())
      step = m_pat->get_length() * m_pat->get_steps() - 1;
    Pattern::NoteIterator iterator = 
      m_pat->find_note(m_added_note.first, m_added_note.second);
    m_pat->resize_note(iterator, step - m_added_note.first + 1);
    
    m_drag_step = step;
    m_drag_note = note;
    break;
  }
    
  case DeletingNotes: {
    int step = int(event->x) / m_col_width;
    int note = m_max_note - int(event->y) / m_row_height - 1;
    if (step == m_drag_step && note == m_drag_note)
      return true;
    if (step >= 0 && step < m_pat->get_length() * m_pat->get_steps() &&
	note >= 0 && note < m_max_note) {
      Pattern::NoteIterator iter = m_pat->find_note(step, note);
      if (iter)
	m_pat->delete_note(iter);
      m_drag_step = step;
      m_drag_note = note;
    }
    break;
  }

  default:
    break;
  }

  return true;
}


void PatternEditor::on_realize() {
  DrawingArea::on_realize();
  RefPtr<Gdk::Window> win = get_window();
  m_gc = GC::create(win);
  m_gc->set_background(m_bg_color);
  m_gc->set_foreground(m_fg_color1);
  FontDescription fd("helvetica bold 9");
  get_pango_context()->set_font_description(fd);
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
  
  // XXX remove 
  //const Pattern::NoteEventList& notes(m_pat->get_notes());
  Pattern::NoteIterator iterator = m_pat->notes_begin();
  for ( ; iterator != m_pat->notes_end(); ++iterator) {
    if (iterator->get_type() == MIDIEvent::NoteOn)
      draw_note(iterator);
  }
  
  if (m_drag_operation == ChangingNoteVelocity) {
    Pattern::NoteIterator iterator = m_pat->find_note(m_drag_step, m_drag_note);
    draw_velocity_box(iterator);
  }
  
  /*
    gc->set_foreground(hlColor);
    win->draw_rectangle(gc, false, event->area.x, event->area.y,
    event->area.width - 1, event->area.height - 1);
  */
  
  return true;
}


void PatternEditor::draw_note(Pattern::NoteIterator iterator) {

  RefPtr<Gdk::Window> win = get_window();
  
  int i = iterator->get_step();
  m_gc->set_foreground(m_note_colors[int(iterator->get_velocity() / 8)]);
  win->draw_rectangle(m_gc, true, i * m_col_width + 1, 
		      (m_max_note - iterator->get_note()- 1) * m_row_height + 1, 
		      iterator->get_length() * m_col_width, m_row_height - 1);
  m_gc->set_foreground(m_edge_color);
  win->draw_rectangle(m_gc, false, i * m_col_width, 
		      (m_max_note - iterator->get_note() - 1) * m_row_height, 
		      iterator->get_length() * m_col_width, m_row_height);
}


void PatternEditor::draw_velocity_box(Pattern::NoteIterator iterator) {
  RefPtr<Gdk::Window> win = get_window();
  m_gc->set_foreground(m_edge_color);
  int box_height = m_layout->get_pixel_logical_extents().get_height() + 6;
  box_height = (box_height < m_row_height * 2 ? m_row_height * 2 : box_height);
  int box_width = m_col_width * iterator->get_length() + 4;
  int l_width = m_layout->get_pixel_logical_extents().get_width();
  box_width = (box_width < l_width + 6 ? l_width + 6 : box_width);
  win->draw_rectangle(m_gc, false, iterator->get_step() * m_col_width - 2,
		      int((m_max_note - iterator->get_note() - 0.5) * 
			  m_row_height - box_height / 2), 
		      box_width, box_height);
  m_gc->set_foreground(m_note_colors[iterator->get_velocity() / 8]);
  win->draw_rectangle(m_gc, true, iterator->get_step() * m_col_width - 1, 
		      int((m_max_note - iterator->get_note() - 0.5) * 
			  m_row_height - box_height / 2) + 1, 
		      box_width - 1, box_height - 1);
  char buffer[10];
  sprintf(buffer, "%d", iterator->get_velocity());
  m_layout->set_text(buffer);
  m_gc->set_foreground(m_edge_color);
  win->draw_layout(m_gc, iterator->get_step() * m_col_width + 2,
		   int((m_max_note - iterator->get_note() - 0.5) * 
		       m_row_height - box_height / 2) + 3, m_layout);
}


void PatternEditor::update() {
  m_pat->get_dirty_rect(&m_d_min_step, &m_d_min_note, 
			&m_d_max_step, &m_d_max_note);
  m_pat->reset_dirty_rect();
  RefPtr<Gdk::Window> win = get_window();
  win->invalidate_rect(Gdk::Rectangle(m_d_min_step * m_col_width, 
				      (m_max_note - m_d_max_note - 1) * 
				      m_row_height, 
				      (m_d_max_step - m_d_min_step + 1) * 
				      m_col_width + 1,
				      (m_d_max_note - m_d_min_note + 1) * 
				      m_row_height + 1), false);
  win->process_updates(false);
}
