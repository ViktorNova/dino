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
  
  /* ugly hack #1: TextBuffer::set_text is ambiguous so we need to disambiguate
     it like this */
  void (TextBuffer::*set_text)(const ustring&) = &TextBuffer::set_text;
  
  /* ugly hack #2: we need the actual TextBuffer pointer instead of the RefPtr,
     so we have to use operator->() to get it */  
  TextBuffer* buf = m_text_info->get_buffer().operator->();
  
  /* ugly hack #3: TextBuffer::get_text is ambiguous too */
  //ustring (TextBuffer::*get_text)(bool) = &TextBuffer::get_text;
  
  m_song->signal_title_changed.connect(mem_fun(m_ent_title, &Entry::set_text));
  m_song->signal_author_changed.connect(mem_fun(m_ent_author,&Entry::set_text));
  m_song->signal_info_changed.connect(mem_fun(buf, set_text));
  
  slot<void> set_title = compose(mem_fun(*m_song, &Song::set_title),
				 mem_fun(m_ent_title, &Entry::get_text));
  slot<void> set_author = compose(mem_fun(*m_song, &Song::set_author),
				  mem_fun(m_ent_author, &Entry::get_text));
  /*slot<void> set_info = compose(mem_fun(*m_song, &Song::set_info),
    bind(mem_fun(buf, get_text), true));*/

  m_ent_title->signal_changed().connect(set_title);
  m_ent_author->signal_changed().connect(set_author);
  //m_text_info->get_buffer()->signal_changed().connect(set_info);
  
  // XXX only update the info when needed, and only update it once every second
  // or so
}


void InfoEditor::reset_gui() {
  m_ent_title->set_text(m_song->get_title());
  m_ent_author->set_text(m_song->get_author());
  //XXX fix this 
  //m_text_info->set_text(m_song->get_info());
}


