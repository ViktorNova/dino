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

#ifndef NOTEEDITOR2_HPP
#define NOTEEDITOR2_HPP

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


class NoteEditor2 : public Gtk::DrawingArea {
public:
  NoteEditor2(Dino::CommandProxy& proxy);
  
  void set_pattern(const Dino::Track& track, const Dino::Pattern& pattern);
  void unset_pattern();
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

private:
  
  /** This is used to figure out what to do when a motion event is received.
      It is set on a button press event and reset (set to NoOperation) on a
      button release event. */
  enum DragOperation {
    DragNoOperation,
    DragResizeNotes,
    DragSelectBox
  } m_drag_operation;
  
  /** This is used to figure out what to do when a pointer motion event is 
      received. For the moment it's only used to paste things. */
  enum MotionOperation {
    MotionNoOperation,
    MotionPaste
  } m_motion_operation;
  
  
  void check_paste(const Dino::SongTime& time, unsigned char key);
  void draw_background(Glib::RefPtr<Gdk::Window>& win);
  void draw_note(Glib::RefPtr<Gdk::Window>& win, 
		 const Dino::Pattern::NoteIterator& iter, bool selected);
  void draw_outline(Glib::RefPtr<Gdk::Window>& win,
		    const Dino::SongTime& start, unsigned char key,
		    const Dino::SongTime& length, bool good);
  void end_drag(const Dino::SongTime& time, unsigned char key);
  void finish_paste(const Dino::SongTime& time, unsigned char key);
  int key2pixel(unsigned char key);
  unsigned char pixel2key(int y);
  Dino::SongTime pixel2time(int x);
  Dino::SongTime snap(const Dino::SongTime& time);
  void start_adding_note(const Dino::SongTime& time, unsigned char key);
  void start_removing_notes(const Dino::SongTime& time, unsigned char key);
  void start_selecting(const Dino::SongTime& time, unsigned char key, 
		       bool clear);
  int time2pixel(const Dino::SongTime& time);
  
  Dino::NoteSelection& get_selection() {
    return m_selection;
  }
  
  
  Glib::RefPtr<Gdk::GC> m_gc;
  Glib::RefPtr<Gdk::Colormap> m_colormap;
  Gdk::Color m_bg_color, m_bg_color2, m_fg_color1, m_fg_color2, m_grid_color, 
    m_edge_color, m_bad_hl_color, m_good_hl_color, m_selbox_color;
  Gdk::Color m_note_colors[16];
  Gdk::Color m_selected_note_colors[16];
  
  int m_row_height;
  Dino::SongTime::Tick m_ticks_per_pixel;
  int m_rows;
  Dino::SongTime::Tick m_snap;
  
  Dino::SongTime m_note_length;
  
  Dino::SongTime m_drag_start_time;
  unsigned char m_drag_start_key;
  Dino::SongTime m_drag_time;
  unsigned char m_drag_key;
  Dino::SongTime m_drag_max_time;
  unsigned char m_drag_min_key;
  Dino::SongTime m_paste_offset_time;
  unsigned char m_paste_offset_key;
  bool m_can_paste;
  
  Dino::NoteSelection m_selection;
  Dino::NoteCollection m_clipboard;
  Dino::NoteCollection m_moved_notes;
  
  const Dino::Track* m_track;
  const Dino::Pattern* m_pattern;
  Glib::RefPtr<Pango::Layout> m_layout;
  Gtk::Adjustment* m_vadj;
  Gtk::Menu m_menu;
  
  Dino::CommandProxy& m_proxy;
  
  unsigned char m_keymap[128];
  
  sigc::connection m_mode_conn;
  sigc::connection m_note_added_conn;
  sigc::connection m_note_removed_conn;
  sigc::connection m_note_changed_conn;
  sigc::connection m_length_changed_conn;
  sigc::connection m_steps_changed_conn;
  sigc::connection m_key_added_conn;
  sigc::connection m_key_removed_conn;
  sigc::connection m_key_changed_conn;
  sigc::connection m_key_moved_conn;
  
};


#endif

