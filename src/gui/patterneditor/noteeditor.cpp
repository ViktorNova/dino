/****************************************************************************
   Dino - A simple pattern based MIDI sequencer
   
   Copyright (C) 2006  Lars Luthman <lars.luthman@gmail.com>
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation, 
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
****************************************************************************/

#include <cstdlib>
#include <iostream>

#include "commandproxy.hpp"
#include "debug.hpp"
#include "note.hpp"
#include "noteeditor.hpp"
#include "plugininterface.hpp"


using namespace Dino;
using namespace Gdk;
using namespace Glib;
using namespace Gtk;
using namespace Pango;
using namespace sigc;
using namespace std;


NoteEditor::NoteEditor(Dino::CommandProxy& proxy) 
  : m_drag_operation(DragNoOperation), 
    m_motion_operation(MotionNoOperation),
    m_row_height(8), 
    m_col_width(8), 
    m_max_note(128), 
    m_drag_y(-1), 
    m_drag_start_vel(-1), 
    m_last_note_length(1),
    m_resize_ok(false),
    m_pat(0),
    m_vadj(0),
    m_proxy(proxy) {
  
  // initialise colours
  m_colormap = Colormap::get_system();
  m_bg_color.set_rgb(65535, 65535, 65535);
  m_bg_color2.set_rgb(60000, 60000, 65535);
  m_fg_color1.set_rgb(0, 0, 65535);
  m_grid_color.set_rgb(40000, 40000, 40000);
  m_edge_color.set_rgb(0, 0, 0);
  m_bad_hl_color.set_rgb(65535, 0, 0);
  m_good_hl_color.set_rgb(0, 65535, 0);
  m_selbox_color.set_rgb(20000, 20000, 40000);
  
  // initialise note colours for different velocities
  gushort red1 = 40000, green1 = 45000, blue1 = 40000;
  gushort red2 = 0, green2 = 30000, blue2 = 0;
  gushort red3 = 50000, green3 = 40000, blue3 = 40000;
  gushort red4 = 60000, green4 = 0, blue4 = 0;
  for (int i = 0; i < 16; ++i) {
    m_note_colors[i].set_rgb(gushort((red1 * (15 - i) + red2 * i) / 15.0),
			     gushort((green1 * (15 - i) + green2 * i) / 15.0),
			     gushort((blue1 * (15 - i) + blue2 * i) / 15.0));
    m_selected_note_colors[i].
      set_rgb(gushort((red3 * (15 - i) + red4 * i) / 15.0),
	      gushort((green3 * (15 - i) + green4 * i) / 15.0),
	      gushort((blue3 * (15 - i) + blue4 * i) / 15.0));
  }
  
  // allocate all colours
  m_colormap->alloc_color(m_bg_color);
  m_colormap->alloc_color(m_bg_color2);
  m_colormap->alloc_color(m_fg_color1);
  m_colormap->alloc_color(m_grid_color);
  m_colormap->alloc_color(m_edge_color);
  m_colormap->alloc_color(m_bad_hl_color);
  m_colormap->alloc_color(m_good_hl_color);
  m_colormap->alloc_color(m_selbox_color);
  for (int i = 0; i < 16; ++i) {
    m_colormap->alloc_color(m_note_colors[i]);
    m_colormap->alloc_color(m_selected_note_colors[i]);
  }
  m_layout = Pango::Layout::create(get_pango_context());
  m_layout->set_text("127");
  add_events(BUTTON_PRESS_MASK | BUTTON_RELEASE_MASK | 
	     BUTTON_MOTION_MASK | SCROLL_MASK | POINTER_MOTION_MASK);
  
  m_added_note = make_pair(-1, -1);
  m_drag_step = -1;
}


void NoteEditor::set_pattern(int track, const Pattern* pattern) {
  m_track = track;
  if (pattern != m_pat) {
    m_pat = pattern;
    if (m_pat) {
      
      m_last_note_length = m_pat->get_steps();
      
      m_selection = NoteSelection(m_pat);
      
      namespace s = sigc;
      sigc::slot<void> draw = mem_fun(*this, &NoteEditor::queue_draw);
      m_pat->signal_note_added().connect(s::hide(draw));
      m_pat->signal_note_removed().connect(s::hide(draw));
      m_pat->signal_note_changed().connect(s::hide(draw));
      m_pat->signal_length_changed().connect(s::hide(draw));
      m_pat->signal_steps_changed().connect(s::hide(draw));
      set_size_request(m_pat->get_length() * m_pat->get_steps() * 
		       m_col_width + 1, m_max_note * m_row_height + 1);
      queue_draw();
    }
    else
      set_size_request();
  }
}


void NoteEditor::set_step_width(int width) {
  assert(width > 0);
  m_col_width = width;
  if (m_pat) {
    set_size_request(m_pat->get_length() * m_pat->get_steps() * m_col_width + 1,
		     m_max_note * m_row_height + 1);
    queue_draw();
  }
}


void NoteEditor::set_vadjustment(Gtk::Adjustment* adj) {
  m_vadj = adj;
}


void NoteEditor::cut_selection() {
  m_clipboard = NoteCollection(m_selection);
  m_proxy.start_atomic("Cut seleced notes");
  delete_selection();
  m_proxy.end_atomic();
}


void NoteEditor::copy_selection() {
  m_clipboard = NoteCollection(m_selection);
}


void NoteEditor::paste() {
  if (m_clipboard.begin() != m_clipboard.end()) {
    m_drag_operation = DragNoOperation;
    m_motion_operation = MotionPaste;
    int x, y;
    ModifierType mt;
    get_window()->get_pointer(x, y, mt);
    x = x < 0 ? 0 : x;
    y = y < 0 ? 0 : y;
    m_drag_step = x / m_col_width;
    m_drag_note = m_max_note - y / m_row_height - 1;
    queue_draw();
  }
}


void NoteEditor::delete_selection() {
  if (m_pat) {
    NoteSelection::Iterator iter1, iter2;
    iter1 = m_selection.begin();
    if (iter1 == m_selection.end())
      return;
    iter2 = iter1;
    m_proxy.start_atomic("Delete selected notes");
    while (iter2 != m_selection.end()) {
      ++iter1;
      m_proxy.delete_note(m_track, m_pat->get_id(), iter2->get_step(),
			  iter2->get_key());
      //m_pat->delete_note(iter2);
      iter2 = iter1;
    }
    m_proxy.end_atomic();
  }
}


void NoteEditor::select_all() {
  if (m_pat) {
    m_selection.clear();
    Pattern::NoteIterator iter;
    for (iter = m_pat->notes_begin(); iter != m_pat->notes_end(); ++iter)
      m_selection.add_note(iter);
    queue_draw();
  }
}


bool NoteEditor::on_button_press_event(GdkEventButton* event) {
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
    
    // if we are pasting and clicking button 1, insert the clipboard
    // if we click another button, abort the paste
    if (m_motion_operation == MotionPaste) {
      m_motion_operation = MotionNoOperation;
      if (event->button == 1) {
	m_proxy.start_atomic("Paste notes");
	m_proxy.add_notes(m_track, m_pat->get_id(), m_clipboard, step, note);
	m_proxy.end_atomic();
	return true;
      }
      queue_draw();
    }
    
    
    switch (event->button) {
      
      // button 1 adds or selects notes
    case 1: {
      
      // Ctrl-Button1 adds notes
      if (event->state & GDK_CONTROL_MASK) {
	unsigned int max = m_pat->check_maximal_free_space(step, note, 
							   m_last_note_length);
	if (max > 0) {
	  m_last_note_length = max;
	  if (m_proxy.add_note(m_track, m_pat->get_id(), step, note, 64, 
			       m_last_note_length)) {
	    Pattern::NoteIterator iter = m_pat->find_note(step, note);
	    m_selection.clear();
	    m_selection.add_note(iter);
	    m_added_note = make_pair(step, note);
	    m_drag_operation = DragChangingNoteLength;
	  }
	}
      }
      
      // Button1 without Ctrl selects and moves
      else {
	Pattern::NoteIterator iterator = m_pat->find_note(step, note);
	if (iterator != m_pat->notes_end()) {
	  
	  // Shift adds or removes from selection
	  if (event->state & GDK_SHIFT_MASK) {
	    if (m_selection.find(iterator) == m_selection.end())
	      m_selection.add_note(iterator);
	    else
	      m_selection.remove_note(iterator);
	  }
	  
	  // No shift, drag the selection
	  else {
	    if (m_selection.find(iterator) == m_selection.end()) {
	      m_selection.clear();
	      m_selection.add_note(iterator);
	    }
	    m_moved_notes = NoteCollection(m_selection);
	    m_drag_operation = DragMovingNotes;
	    NoteSelection::Iterator i;
	    unsigned int minstep = step;
	    unsigned int maxkey = note;
	    for (i = m_selection.begin(); i != m_selection.end(); ++i) {
	      minstep = minstep < i->get_step() ? minstep : i->get_step();
	      maxkey = maxkey > i->get_key() ? maxkey : i->get_key();
	    }
	    m_move_offset_step = minstep - step;
	    m_move_offset_note = maxkey - note;
	    queue_draw();
	  }
	}
	
	// outside a note, select a box
	else {
	  if (!(event->state & GDK_SHIFT_MASK))
	    m_selection.clear();
	  m_drag_operation = DragSelectBox;
	  m_sb_note = note;
	  m_sb_step = step;
	}
	queue_draw();
      }
      break;
    }
    
    // button 2 changes the velocity or length or inserts a copy of the 
    // selection
    case 2: {
      Pattern::NoteIterator iterator = m_pat->find_note(step, note);
      if (iterator != m_pat->notes_end()) {
	
	// if the click is outside the selection, clear the selection first
	if (m_selection.find(iterator) == m_selection.end()) {
	  m_selection.clear();
	  m_selection.add_note(iterator);
	}
	
	if (event->state & GDK_CONTROL_MASK) {
	  m_drag_operation = DragChangingNoteVelocity;
	  m_drag_start_vel = iterator->get_velocity();
	  queue_draw();
	}
	else {
	  NoteSelection::Iterator iter;
	  unsigned new_size = step - iterator->get_step() + 1;
	  //m_proxy.start_atomic("Resize notes");
	  //for (iter = m_selection.begin(); iter != m_selection.end(); ++iter)
	  //  m_proxy.set_note_size(m_track, m_pat->get_id(), iter->get_step(),
	  //                        iter->get_key(), new_size);
	  //m_proxy.end_atomic();
	  //m_pat->resize_note(iter, new_size);
	  m_last_note_length = new_size;
	  m_added_note = make_pair(iterator->get_step(), note);
	  m_drag_operation = DragChangingNoteLength;
	}
      }
      else {
	// outside a note, add the selection here
	step = (step < int(m_pat->get_length() * m_pat->get_steps()) ? step : 
		m_pat->get_length() * m_pat->get_steps() - 1);
	NoteCollection nc(m_selection);
	m_proxy.add_notes(m_track, m_pat->get_id(), nc, step, note);
	//m_pat->add_notes(nc, step, note);
      }
      
      break;
    }
    
    // Ctrl-Button3 deletes or pops up menu
    case 3: {
      Pattern::NoteIterator iterator = m_pat->find_note(step, note);
      if (iterator != m_pat->notes_end()) {
	// if shift isn't pressed the selection is set to this single note
	if (m_selection.find(iterator) == m_selection.end()) {
	  m_selection.clear();
	  m_selection.add_note(iterator);
	}
	if (event->state & GDK_CONTROL_MASK) {
	  m_proxy.start_atomic("Delete notes");
	  delete_selection();
	  m_proxy.end_atomic();
	}
	else {
	  queue_draw();
	  m_menu.popup(event->button, event->time);
	}
      }
      if (event->state & GDK_CONTROL_MASK)
	m_drag_operation = DragDeletingNotes;
      break;
    }
      
    }
  }
  
  return true;
}


bool NoteEditor::on_button_release_event(GdkEventButton* event) {
  if (!m_pat)
    return false;
  
  dbg1<<__PRETTY_FUNCTION__<<endl;
  
  if (m_drag_operation == DragChangingNoteLength) {
    if (m_resize_ok) {
      NoteSelection::Iterator iter;
      m_proxy.start_atomic("Resize notes");
      for (iter = m_selection.begin(); iter != m_selection.end(); ++iter)
	m_proxy.set_note_size(m_track, m_pat->get_id(), iter->get_step(),
			      iter->get_key(), m_last_note_length);
      m_proxy.end_atomic();
      m_added_note = make_pair(-1, -1);
    }
    queue_draw();
  }
  
  if (m_drag_operation == DragChangingNoteVelocity) {
    m_drag_operation = DragNoOperation;
    queue_draw();
  }
  
  if (m_drag_operation == DragMovingNotes) {
    m_drag_operation = DragNoOperation;
    int step = m_drag_step + m_move_offset_step;
    step = step < 0 ? 0 : step;
    int note = int(m_drag_note) + m_move_offset_note;
    note = note < 0 ? 0 : note;
    note = note > 127 ? 127 : note;
    if (unsigned(step) < m_pat->get_steps() * m_pat->get_length()) {
      m_proxy.start_atomic("Move notes");
      delete_selection();
      m_proxy.add_notes(m_track, m_pat->get_id(), m_moved_notes, 
			step, note, &m_selection);
      m_proxy.end_atomic();
    }
  }
  
  if (m_drag_operation == DragSelectBox) {
    m_drag_operation = DragNoOperation;
    Pattern::NoteIterator iter;
    unsigned int minstep = m_drag_step < m_sb_step ? m_drag_step : m_sb_step;
    unsigned int maxstep = m_drag_step > m_sb_step ? m_drag_step : m_sb_step;
    unsigned int minnote = m_drag_note < m_sb_note ? m_drag_note : m_sb_note;
    unsigned int maxnote = m_drag_note > m_sb_note ? m_drag_note : m_sb_note;
    for (iter = m_pat->notes_begin(); iter != m_pat->notes_end(); ++iter) {
      if (iter->get_step() <= maxstep && 
	  iter->get_step() + iter->get_length() >= minstep &&
	  iter->get_key() >= minnote && iter->get_key() <= maxnote) {
	m_selection.add_note(iter);
      }
    }
    queue_draw();
  }

  m_drag_operation = DragNoOperation;
  
  return true;
}


bool NoteEditor::on_motion_notify_event(GdkEventMotion* event) {
  if (!m_pat)
    return false;
  
  switch (m_drag_operation) {

  case DragChangingNoteVelocity: {
    double dy = m_drag_y - int(event->y);
    int velocity = int(m_drag_start_vel + dy);
    velocity = (velocity < 0 ? 0 : (velocity > 127 ? 127 : velocity));
    NoteSelection::Iterator iter;
    m_proxy.start_atomic("Change note velocities");
    for (iter = m_selection.begin(); iter != m_selection.end(); ++iter)
      m_proxy.set_note_velocity(m_track, m_pat->get_id(), iter->get_step(),
				iter->get_key(), velocity);
    m_proxy.end_atomic();
    //m_pat->set_velocity(iter, velocity);
    queue_draw();
    break;
  }
    
  case DragChangingNoteLength: {
    int step = int(event->x) / m_col_width;
    int note = m_max_note - int(event->y) / m_row_height - 1;
    
    // or changing the note length
    if (step < m_added_note.first)
      step = m_added_note.first;
    if (step == m_drag_step)
      return true;
    else if (step >= int(m_pat->get_length() * m_pat->get_steps()))
      step = m_pat->get_length() * m_pat->get_steps() - 1;
    Pattern::NoteIterator iterator = 
      m_pat->find_note(m_added_note.first, m_added_note.second);
    unsigned new_size = step - m_added_note.first + 1;
    NoteSelection::Iterator iter;
    //m_proxy.start_atomic("Resize notes");
    //for (iter = m_selection.begin(); iter != m_selection.end(); ++iter)
    //  m_proxy.set_note_size(m_track, m_pat->get_id(), iter->get_step(), 
    //                        iter->get_key(), new_size);
    //m_proxy.end_atomic();
    //m_pat->resize_note(iter, new_size);
    m_last_note_length = step - m_added_note.first + 1;
    
    m_drag_step = step;
    m_drag_note = note;
    queue_draw();
    break;
  }
    
  case DragDeletingNotes: {
    int step = int(event->x) / m_col_width;
    int note = m_max_note - int(event->y) / m_row_height - 1;
    if (step == m_drag_step && note == m_drag_note)
      return true;
    if (step >= 0 && step < int(m_pat->get_length() * m_pat->get_steps()) &&
	note >= 0 && note < m_max_note) {
      Pattern::NoteIterator iter = m_pat->find_note(step, note);
      if (iter != m_pat->notes_end())
	m_proxy.delete_note(m_track, m_pat->get_id(), iter->get_step(),
			    iter->get_key());
	//m_pat->delete_note(iter);
      m_drag_step = step;
      m_drag_note = note;
    }
    break;
  }

  case DragMovingNotes: {
    int step = int(event->x) / m_col_width;
    int note = m_max_note - int(event->y) / m_row_height - 1;
    if (m_drag_step != step || m_drag_note != note) {
      m_drag_step = step;
      m_drag_note = note;
      queue_draw();
    }
    break;
  }
    
  case DragSelectBox: {
    int step = int(event->x) / m_col_width;
    int note = m_max_note - int(event->y) / m_row_height - 1;
    if (m_drag_step != step || m_drag_note != note) {
      m_drag_note = note;
      m_drag_step = (step >= int(m_pat->get_steps() * m_pat->get_length()) ?
		     m_pat->get_steps() * m_pat->get_length() - 1: step);
      
      queue_draw();
    }
    break;
  }
    
  case DragNoOperation:
    if (m_motion_operation == MotionPaste) {
      int step = int(event->x) / m_col_width;
      int note = m_max_note - int(event->y) / m_row_height - 1;
      if (m_drag_step != step || m_drag_note != note) {
	m_drag_step = step;
	m_drag_note = note;
	queue_draw();
      }
    }
    break;
    
  default:
    break;
  }

  return true;
}


bool NoteEditor::on_scroll_event(GdkEventScroll* event) {
  
  if (!m_vadj)
    return true;
  
  double value = m_vadj->get_value();
  if (event->direction == GDK_SCROLL_UP) {
    value -= m_vadj->get_step_increment();
    if (value < m_vadj->get_lower())
      value = m_vadj->get_lower();
  }
  else if (event->direction == GDK_SCROLL_DOWN) {
    value += m_vadj->get_step_increment();
    if (value > m_vadj->get_upper() - m_vadj->get_page_size())
      value = m_vadj->get_upper() - m_vadj->get_page_size();
  }
  m_vadj->set_value(value);
    
  return true;
}


void NoteEditor::on_realize() {
  DrawingArea::on_realize();
  RefPtr<Gdk::Window> win = get_window();
  m_gc = GC::create(win);
  m_gc->set_background(m_bg_color);
  m_gc->set_foreground(m_fg_color1);
  FontDescription fd("helvetica bold 9");
  get_pango_context()->set_font_description(fd);
  win->clear();
}


bool NoteEditor::on_expose_event(GdkEventExpose* event) {
  RefPtr<Gdk::Window> win = get_window();
  win->clear();
  if (!m_pat)
    return true;
  
  // draw background
  int width = m_pat->get_length() * m_pat->get_steps() * m_col_width;
  int height = m_max_note * m_row_height;
  for (unsigned int b = 0; b < m_pat->get_length(); ++b) {
    if (b % 2 == 0)
      m_gc->set_foreground(m_bg_color);
    else
      m_gc->set_foreground(m_bg_color2);
    win->draw_rectangle(m_gc, true, b * m_pat->get_steps() * m_col_width, 0, 
			m_pat->get_steps() * m_col_width, height);
  }
  
  // draw grid
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
  for (unsigned c = 0; c < m_pat->get_steps() * m_pat->get_length() + 1; ++c)
    win->draw_line(m_gc, c * m_col_width, 0, c * m_col_width, height);
  
  // draw selection box
  if (m_drag_operation == DragSelectBox) {
    int minx = m_sb_step < m_drag_step ? m_sb_step : m_drag_step;
    int miny = m_sb_note > m_drag_note ? m_sb_note : m_drag_note;
    miny = 127 - miny;
    int w = abs(m_sb_step - m_drag_step) + 1;
    int h = abs(m_sb_note - m_drag_note) + 1;
    m_gc->set_foreground(m_selbox_color);
    win->draw_rectangle(m_gc, true, minx * m_col_width, miny * m_row_height,
			w * m_col_width + 1, h * m_row_height + 1);
  }
  
  // draw notes
  Pattern::NoteIterator iter;
  for (iter = m_pat->notes_begin(); iter != m_pat->notes_end(); ++iter)
    draw_note(iter, m_selection.find(iter) != m_selection.end());
  
  // draw box for editing note velocity
  if (m_drag_operation == DragChangingNoteVelocity) {
    iter = m_pat->find_note(m_drag_step, m_drag_note);
    draw_velocity_box(iter, m_selection.find(iter) != m_selection.end());
  }
  
  // draw outline for notes to be pasted
  if (m_motion_operation == MotionPaste) {
    bool ok = false;
    if (m_drag_step > 0) {
      ok = true;
      NoteCollection::ConstIterator iter;
      for (iter = m_clipboard.begin(); iter != m_clipboard.end(); ++iter) {
	if (!m_pat->check_free_space(m_drag_step + iter->start,
				     iter->key - 127 + m_drag_note, 
				     iter->length)) {
	  ok = false;
	  break;
	}
      }
    }
    dbg1<<"ok = "<<ok<<endl;
    draw_outline(m_clipboard, m_drag_step, m_drag_note, ok);
  }
  
  // draw outline for notes to be moved
  else if (m_drag_operation == DragMovingNotes) {
    int step = m_drag_step + m_move_offset_step;
    step = step < 0 ? 0 : step;
    int note = m_drag_note + m_move_offset_note;
    note = note < 0 ? 0 : note;
    note = note > 127 ? 127 : note;
    bool ok = false;
    if (m_drag_step > 0) {
      ok = true;
      NoteCollection::ConstIterator iter;
      for (iter = m_moved_notes.begin(); iter != m_moved_notes.end(); ++iter) {
	if (!m_pat->check_free_space(step + iter->start,
				     iter->key - 127 + note, 
				     iter->length, m_selection)) {
	  ok = false;
	  break;
	}
      }
    }
    dbg1<<"ok = "<<ok<<endl;
    draw_outline(m_moved_notes, step, note, ok);
  }
  
  // draw outline for notes to be resized
  // XXX awful, awful, awful. should have a note collection member instead
  // of a local one, or even better, a function to draw outlines of a selection
  // instead of a collection
  else if (m_drag_operation == DragChangingNoteLength) {
    m_resize_ok = true;
    unsigned int step = numeric_limits<unsigned int>::max();
    int key = 0;
    NoteSelection::Iterator iter;
    for (iter = m_selection.begin(); iter != m_selection.end(); ++iter) {
      if (iter->get_step() < step)
	step = iter->get_step();
      if (iter->get_key() > key)
	key = iter->get_key();
      if (m_last_note_length <= iter->get_length())
	continue;
      if (!m_pat->check_free_space(iter->get_step() + iter->get_length(),
				   iter->get_key(), 
				   m_last_note_length - iter->get_length())) {
	m_resize_ok = false;
      }
    }

    dbg1<<"m_resize_ok = "<<m_resize_ok<<endl;
    NoteCollection tmp(m_selection);
    NoteCollection::Iterator iter2;
    for (iter2 = tmp.begin(); iter2 != tmp.end(); ++iter2)
      iter2->length = m_last_note_length;
    draw_outline(tmp, step, key, m_resize_ok);
  }
    
  
  /*
    gc->set_foreground(hlColor);
    win->draw_rectangle(gc, false, event->area.x, event->area.y,
    event->area.width - 1, event->area.height - 1);
  */
  
  return true;
}


void NoteEditor::draw_note(Pattern::NoteIterator iterator, bool selected) {

  RefPtr<Gdk::Window> win = get_window();
  
  int i = iterator->get_step();
  if (!selected)
    m_gc->set_foreground(m_note_colors[int(iterator->get_velocity() / 8)]);
  else
    m_gc->set_foreground(m_selected_note_colors[int(iterator->get_velocity() / 8)]);
  win->draw_rectangle(m_gc, true, i * m_col_width + 1, 
		      (m_max_note - iterator->get_key()- 1) * m_row_height + 1, 
		      iterator->get_length() * m_col_width, m_row_height - 1);
  m_gc->set_foreground(m_edge_color);
  win->draw_rectangle(m_gc, false, i * m_col_width, 
		      (m_max_note - iterator->get_key() - 1) * m_row_height, 
		      iterator->get_length() * m_col_width, m_row_height);
}


void NoteEditor::draw_velocity_box(Pattern::NoteIterator iterator,
				   bool selected) {
  RefPtr<Gdk::Window> win = get_window();
  m_gc->set_foreground(m_edge_color);
  int box_height = m_layout->get_pixel_logical_extents().get_height() + 6;
  box_height = (box_height < m_row_height * 2 ? m_row_height * 2 : box_height);
  int box_width = m_col_width * iterator->get_length() + 4;
  int l_width = m_layout->get_pixel_logical_extents().get_width();
  box_width = (box_width < l_width + 6 ? l_width + 6 : box_width);
  win->draw_rectangle(m_gc, false, iterator->get_step() * m_col_width - 2,
		      int((m_max_note - iterator->get_key() - 0.5) * 
			  m_row_height - box_height / 2), 
		      box_width, box_height);
  if (!selected)
    m_gc->set_foreground(m_note_colors[iterator->get_velocity() / 8]);
  else
    m_gc->set_foreground(m_selected_note_colors[iterator->get_velocity() / 8]);
  win->draw_rectangle(m_gc, true, iterator->get_step() * m_col_width - 1, 
		      int((m_max_note - iterator->get_key() - 0.5) * 
			  m_row_height - box_height / 2) + 1, 
		      box_width - 1, box_height - 1);
  char buffer[10];
  sprintf(buffer, "%d", iterator->get_velocity());
  m_layout->set_text(buffer);
  m_gc->set_foreground(m_edge_color);
  win->draw_layout(m_gc, iterator->get_step() * m_col_width + 2,
		   int((m_max_note - iterator->get_key() - 0.5) * 
		       m_row_height - box_height / 2) + 3, m_layout);
}


void NoteEditor::update() {
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


void NoteEditor::draw_outline(const Dino::NoteCollection& notes, 
			      unsigned int step, unsigned char key, bool good) {
  RefPtr<Gdk::Window> win = get_window();
  if (good)
    m_gc->set_foreground(m_good_hl_color);
  else
    m_gc->set_foreground(m_bad_hl_color);
  NoteCollection::ConstIterator iter;
  for (iter = notes.begin(); iter != notes.end(); ++iter) {
    if (step + iter->start >= m_pat->get_length() * m_pat->get_steps())
      continue;
    int length = iter->length;
    if (step + iter->start + iter->length >= 
	m_pat->get_length() * m_pat->get_steps())
      length = m_pat->get_length() * m_pat->get_steps() - step - iter->start;
    win->draw_rectangle(m_gc, false, (step + iter->start) * m_col_width, 
			(2 * m_max_note - iter->key - 2 - key) * m_row_height, 
			iter->length * m_col_width, m_row_height);
  }
}


void NoteEditor::update_menu(PluginInterface& plif) {
  using namespace Menu_Helpers;
  m_menu.items().clear();
  // XXX implement this
  /*
  PluginInterface::action_iterator iter;
  NoteSelectionAction* psa;
  const Song& song = plif.get_song();
  for (iter = plif.actions_begin(); iter != plif.actions_end(); ++iter) {
    if ((psa = dynamic_cast<NoteSelectionAction*>(*iter))) {
      slot<void> aslot = 
	compose(bind<1>(mem_fun(*psa, &NoteSelectionAction::run),
			ref(m_proxy)),
			mem_fun(*this, &NoteEditor::get_selection));
      m_menu.items().push_back(MenuElem((*iter)->get_name(), aslot));
    }
    }*/
}

