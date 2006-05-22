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
   along with this program; if not, write to the Free Software Foundation, 
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
****************************************************************************/

#include <gtkmm/textbuffer.h>

#include "infoeditor.hpp"
#include "song.hpp"


using namespace Glib;
using namespace Gtk;
using namespace sigc;
using namespace Dino;


namespace {
  InfoEditor* m_ie;
  PluginInterface* m_plif;
}


extern "C" {
  string dino_get_name() { 
    return "Info editor"; 
  }
  
  void dino_load_plugin(PluginInterface& plif) {
    m_ie = manage(new InfoEditor(plif.get_song()));
    plif.add_page("Information", *m_ie);
    m_plif = &plif;
  }
  
  void dino_unload_plugin() {
    m_plif->remove_page(*m_ie);
  }
}


InfoEditor::InfoEditor(Dino::Song& song)
  : m_song(song) {
  
  Table* t = manage(new Table(3, 2, false));
  m_ent_title = manage(new Entry);
  m_ent_author = manage(new Entry);
  m_text_info = manage(new TextView);
  t->attach(*manage(new Label("Title:", 0.0f, 0.5f)), 0, 1, 0, 1, FILL, FILL);
  t->attach(*m_ent_title, 1, 2, 0, 1, FILL|EXPAND, FILL);
  t->attach(*manage(new Label("Author:", 0.0f, 0.5f)), 0, 1, 1, 2, FILL, FILL);
  t->attach(*m_ent_author, 1, 2, 1, 2, FILL|EXPAND, FILL);
  t->attach(*manage(new Label("Comment:", 0.0f, 0.0f)), 0, 1, 2, 3, FILL);
  ScrolledWindow* scw = manage(new ScrolledWindow);
  scw->set_policy(POLICY_ALWAYS, POLICY_ALWAYS);
  scw->add(*m_text_info);
  scw->set_shadow_type(SHADOW_IN);
  t->attach(*scw, 1, 2, 2, 3);
  t->set_col_spacings(5);
  t->set_row_spacings(5);
  t->set_border_width(5);
  pack_start(*t);

  /* ugly hack #1: TextBuffer::get_text is ambiguous so we need to disambiguate
     it like this */
  ustring (TextBuffer::*get_text)(bool) = &TextBuffer::get_text;
  
  /* ugly hack #2: we need the actual TextBuffer pointer instead of the RefPtr,
     so we have to use operator->() to get it */  
  TextBuffer* buf = m_text_info->get_buffer().operator->();
  
  m_song.signal_title_changed().connect(mem_fun(m_ent_title, &Entry::set_text));
  m_song.signal_author_changed().
    connect(mem_fun(m_ent_author, &Entry::set_text));
  m_song.signal_info_changed().
    connect(mem_fun(*this, &InfoEditor::update_info));
  
  slot<void> set_title = compose(mem_fun(m_song, &Song::set_title),
				 mem_fun(m_ent_title, &Entry::get_text));
  slot<void> set_author = compose(mem_fun(m_song, &Song::set_author),
				  mem_fun(m_ent_author, &Entry::get_text));
  slot<void> set_info = compose(mem_fun(m_song, &Song::set_info),
				bind(mem_fun(buf, get_text), true));

  m_ent_title->signal_changed().connect(set_title);
  m_ent_author->signal_changed().connect(set_author);
  m_text_info->get_buffer()->signal_changed().connect(set_info);
  
  reset_gui();
}


void InfoEditor::reset_gui() {
  m_ent_title->set_text(m_song.get_title());
  m_ent_author->set_text(m_song.get_author());
  m_text_info->get_buffer()->set_text(m_song.get_info());
}


void InfoEditor::update_info(const string& info) {
  string view_text = m_text_info->get_buffer()->get_text();
  if (info != view_text)
    m_text_info->get_buffer()->set_text(info);
}
