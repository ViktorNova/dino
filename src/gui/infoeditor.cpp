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
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
****************************************************************************/

#include <gtkmm/textbuffer.h>

#include "infoeditor.hpp"
#include "song.hpp"


using namespace Gnome::Glade;
using namespace Glib;
using namespace Gtk;
using namespace sigc;
using namespace Dino;


InfoEditor::InfoEditor(BaseObjectType* cobject, 
		       const Glib::RefPtr<Gnome::Glade::Xml>& xml)
  : Gtk::Table(cobject),
    m_song(0) {

  m_ent_title = w<Entry>(xml, "ent_title");
  m_ent_author = w<Entry>(xml, "ent_author");
  m_text_info = w<TextView>(xml, "text_info");
}


void InfoEditor::set_song(Song* song) {
  m_song = song;
  
  /* ugly hack #1: TextBuffer::get_text is ambiguous so we need to disambiguate
     it like this */
  ustring (TextBuffer::*get_text)(bool) = &TextBuffer::get_text;
  
  /* ugly hack #2: we need the actual TextBuffer pointer instead of the RefPtr,
     so we have to use operator->() to get it */  
  TextBuffer* buf = m_text_info->get_buffer().operator->();
  
  m_song->signal_title_changed.connect(mem_fun(m_ent_title, &Entry::set_text));
  m_song->signal_author_changed.connect(mem_fun(m_ent_author,&Entry::set_text));
  m_song->signal_info_changed.connect(mem_fun(*this, &InfoEditor::update_info));
  
  slot<void> set_title = compose(mem_fun(*m_song, &Song::set_title),
				 mem_fun(m_ent_title, &Entry::get_text));
  slot<void> set_author = compose(mem_fun(*m_song, &Song::set_author),
				  mem_fun(m_ent_author, &Entry::get_text));
  slot<void> set_info = compose(mem_fun(*m_song, &Song::set_info),
				bind(mem_fun(buf, get_text), true));

  m_ent_title->signal_changed().connect(set_title);
  m_ent_author->signal_changed().connect(set_author);
  m_text_info->get_buffer()->signal_changed().connect(set_info);
}


void InfoEditor::reset_gui() {
  m_ent_title->set_text(m_song->get_title());
  m_ent_author->set_text(m_song->get_author());
  m_text_info->get_buffer()->set_text(m_song->get_info());
}


void InfoEditor::update_info(const string& info) {
  string view_text = m_text_info->get_buffer()->get_text();
  if (info != view_text)
    m_text_info->get_buffer()->set_text(info);
}
