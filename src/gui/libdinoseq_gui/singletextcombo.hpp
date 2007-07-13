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

#ifndef SINGLETEXTCOMBO_HPP
#define SINGLETEXTCOMBO_HPP

#include <string>

#include <gtkmm.h>


/** This class is needed because @c ComboBoxText in gtkmm does not provide a
    way of removing entries. */
class SingleTextCombo : public Gtk::ComboBox {
private:
  // noncopyable
  SingleTextCombo(const SingleTextCombo&);
  SingleTextCombo& operator=(const SingleTextCombo&);

public:
  SingleTextCombo();
  
  /** Add a string at the end of the combo box with the ID number @c id. */
  void append_text(const Glib::ustring& text, long id = -1);
  /** Add a string at the beginning of the combo box with the ID number 
      @c id. */
  void prepend_text(const Glib::ustring& text, long id = -1);
  /** Remove a string with the given ID. */
  void remove_id(long id);
  /** Returns the ID number for the active entry. */
  long get_active_id() const;
  /** Set the active entry to the one with ID number @c id. If there is no
      such entry, return @c false. */
  bool set_active_id(long id);
  /** Set the active entry to the one with the text @c text. If there is no
      such entry, return @c false. */
  bool set_active_text(const Glib::ustring& text);
  /** Returns the text for the active entry. */
  std::string get_active_text() const;
  
  /** Remove all entries. */
  void clear();

protected:

  /** These are the columns that are used in the model for this combo box.
      Only the @c mText column is displayed, the @c mID column is used to
      store an additional ID number. */
  class TextModelColumns : public Gtk::TreeModel::ColumnRecord {
  public:
    TextModelColumns() { add(m_text); add(m_id); }
    Gtk::TreeModelColumn<Glib::ustring> m_text;
    Gtk::TreeModelColumn<long> m_id;
  };

  TextModelColumns m_text_columns;
  Glib::RefPtr<Gtk::ListStore> m_store;
};


#endif 
