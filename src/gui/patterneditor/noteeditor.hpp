/****************************************************************************
   Dino - A simple pattern based MIDI sequencer
   
   Copyright (C) 2006  Lars Luthman <lars.luthman@gmail.com>
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation, 
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
****************************************************************************/

#ifndef NOTEEDITOR_HPP
#define NOTEEDITOR_HPP

#include <utility>

#include <gtkmm.h>

#include "notecollection.hpp"
#include "pattern.hpp"
#include "patternselection.hpp"


class PluginInterface;


class NoteEditor : public Gtk::DrawingArea {
public:
  NoteEditor();
  
  void set_pattern(Dino::Pattern* pattern);
  void set_step_width(int width);
  void set_vadjustment(Gtk::Adjustment* adj);

  void cut_selection();
  void copy_selection();
  void paste();
  void delete_selection();
  void select_all();
  
  void update_menu(PluginInterface& plif);
  
protected:
  
  // event handlers
  virtual bool on_button_press_event(GdkEventButton* event);
  virtual bool on_button_release_event(GdkEventButton* event);
  virtual bool on_motion_notify_event(GdkEventMotion* event);
  virtual bool on_expose_event(GdkEventExpose* event);
  virtual bool on_scroll_event(GdkEventScroll* event);
  virtual void on_realize();

  void draw_note(Dino::Pattern::NoteIterator iterator, bool selected = false);
  void draw_velocity_box(Dino::Pattern::NoteIterator iterator, 
			 bool selected = false);
  void draw_outline(const Dino::NoteCollection& notes, unsigned int step,
		    unsigned char key);
  void update();
  
private:
  
  /** This is used to figure out what to do when a motion event is received.
      It is set on a button press event and reset (set to NoOperation) on a
      button release event. */
  enum DragOperation {
    DragNoOperation,
    DragChangingNoteLength,
    DragChangingNoteVelocity,
    DragDeletingNotes,
    DragMovingNotes,
    DragSelectBox
  } m_drag_operation;
  
  /** This is used to figure out what to do when a pointer motion event is 
      received. For the moment it's only used to paste things. */
  enum MotionOperation {
    MotionNoOperation,
    MotionPaste
  } m_motion_operation;
  
  Glib::RefPtr<Gdk::GC> m_gc;
  Glib::RefPtr<Gdk::Colormap> m_colormap;
  Gdk::Color m_bg_color, m_bg_color2, m_fg_color1, m_fg_color2, m_grid_color, 
    m_edge_color, m_hl_color, m_selbox_color;
  Gdk::Color m_note_colors[16];
  Gdk::Color m_selected_note_colors[16];
  int m_row_height;
  int m_col_width;
  int m_max_note;
  
  std::pair<int, int> m_added_note;
  int m_drag_step;
  int m_drag_note;
  int m_drag_y;
  int m_drag_start_vel;
  int m_last_note_length;
  int m_move_offset_step;
  int m_move_offset_note;
  int m_sb_step, m_sb_note;
  
  Dino::PatternSelection m_selection;
  Dino::NoteCollection m_clipboard;
  Dino::NoteCollection m_moved_notes;
  
  Dino::PatternSelection& get_selection() {
    return m_selection;
  }
  
  int m_d_min_step, m_d_max_step, m_d_min_note, m_d_max_note;
  Dino::Pattern* m_pat;
  Glib::RefPtr<Pango::Layout> m_layout;
  Gtk::Adjustment* m_vadj;
  Gtk::Menu m_menu;
};


#endif

