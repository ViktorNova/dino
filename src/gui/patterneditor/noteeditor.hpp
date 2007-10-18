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

#ifndef NOTEEDITOR_HPP
#define NOTEEDITOR_HPP

#include <utility>

#include <gtkmm.h>

#include "notecollection.hpp"
#include "noteselection.hpp"
#include "pattern.hpp"
#include "track.hpp"


namespace Dino {
  class CommandProxy;
}


class PluginInterface;


class NoteEditor : public Gtk::DrawingArea {
public:
  NoteEditor(Dino::CommandProxy& proxy);
  
  void set_pattern(const Dino::Track* track, const Dino::Pattern* pattern);
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
  void draw_outline(const Dino::NoteCollection& notes, int step,
		    int row, bool good);
  void update();
  void mode_changed(Dino::Track::Mode mode);
  void draw_background(Glib::RefPtr<Gdk::Window> win, int width, int height);
  void draw_grid(Glib::RefPtr<Gdk::Window> win, int width, int height);
  void draw_selection_box(Glib::RefPtr<Gdk::Window> win, int width, int height);
  void draw_paste_outline(Glib::RefPtr<Gdk::Window> win, int width, int height);
  void draw_move_outline(Glib::RefPtr<Gdk::Window> win, int width, int height);
  void draw_resize_outline(Glib::RefPtr<Gdk::Window> win, 
			   int width, int height);
  int pixel2row(int y);
  int pixel2step(int x);
  int row2pixel(int row);
  int step2pixel(int step);
  int row2key(int row);
  int key2row(int row);
  
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
    m_edge_color, m_bad_hl_color, m_good_hl_color, m_selbox_color;
  Gdk::Color m_note_colors[16];
  Gdk::Color m_selected_note_colors[16];
  int m_row_height;
  int m_col_width;
  int m_rows;
  
  std::pair<int, int> m_added_note;
  int m_drag_step;
  int m_drag_row;
  int m_drag_y;
  int m_drag_start_vel;
  int m_last_note_length;
  int m_move_offset_step;
  int m_move_offset_row;
  int m_sb_step, m_sb_row;
  bool m_resize_ok;
  
  Dino::NoteSelection m_selection;
  Dino::NoteCollection m_clipboard;
  Dino::NoteCollection m_moved_notes;
  
  Dino::NoteSelection& get_selection() {
    return m_selection;
  }
  
  int m_d_min_step, m_d_max_step, m_d_min_note, m_d_max_note;
  const Dino::Pattern* m_pat;
  const Dino::Track* m_trk;
  Glib::RefPtr<Pango::Layout> m_layout;
  Gtk::Adjustment* m_vadj;
  Gtk::Menu m_menu;
  
  Dino::CommandProxy& m_proxy;
  
  unsigned char m_keymap[128];
};


#endif

