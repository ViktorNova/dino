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

#include <gtkmm/liststore.h>
#include <gtkmm/cellrenderertext.h>
#include <gtk/gtkcombobox.h>
#include <gtk/gtkcelllayout.h>

#include "singletextcombo.hpp"


using namespace Gtk;
using namespace std;
using namespace Glib;


SingleTextCombo::SingleTextCombo() {
  m_store = ListStore::create(m_text_columns);
  set_model(m_store);
  pack_start(m_text_columns.m_text);
}


void SingleTextCombo::append_text(const ustring& text, long id) {
  TreeModel::iterator iter = m_store->append();
  TreeModel::Row row = *iter;
  row[m_text_columns.m_text] = text;
  row[m_text_columns.m_id] = id;
}


void SingleTextCombo::prepend_text(const ustring& text, long id) {
  TreeModel::iterator iter = m_store->prepend();
  TreeModel::Row row = *iter;
  row[m_text_columns.m_text] = text;
}


void SingleTextCombo::remove_id(long id) {
  TreeModel::iterator iter;
  for (iter = m_store->children().begin(); 
       iter != m_store->children().end(); ++iter) {
    if (id == (*iter)[m_text_columns.m_id]) {
      m_store->erase(iter);
      return;
    }
  }
}


long SingleTextCombo::get_active_id() const {
  long result = -1;
  TreeModel::iterator activeRow = get_active();
  if(activeRow) {
    TreeModel::Row row = *activeRow;
    result = row[m_text_columns.m_id];
  }
  return result;
}


void SingleTextCombo::clear() {
  m_store->clear();
}


bool SingleTextCombo::set_active_id(long ID) {
  if(m_store) {
    for(TreeModel::iterator iter = m_store->children().begin(); 
	iter != m_store->children().end(); ++iter) {
      long thisID = (*iter)[m_text_columns.m_id];
      if(thisID == ID) {
        set_active(iter);
        return true;
      }
    }
  }

  //Not found, so mark it as blank:
  unset_active();
  return false;
}


bool SingleTextCombo::set_active_text(const Glib::ustring& text) {
  if(m_store) {
    for(TreeModel::iterator iter = m_store->children().begin(); 
	iter != m_store->children().end(); ++iter) {
      const Glib::ustring& thisText = (*iter)[m_text_columns.m_text];
      if(thisText == text) {
        set_active(iter);
        return true;
      }
    }
  }

  //Not found, so mark it as blank:
  unset_active();
  return false;
}


string SingleTextCombo::get_active_text() const {
  TreeModel::iterator activeRow = get_active();
  ustring str;
  if(activeRow) {
    TreeModel::Row row = *activeRow;
    str = row[m_text_columns.m_text];
  }
  return str;
}
