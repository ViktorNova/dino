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
  m_song->signal_title_changed.connect(mem_fun(m_ent_title, &Entry::set_text));
  m_song->signal_author_changed.connect(mem_fun(m_ent_author,&Entry::set_text));
  slot<void> set_title = compose(mem_fun(*m_song, &Song::set_title),
				 mem_fun(m_ent_title, &Entry::get_text));
  m_ent_title->signal_changed().connect(set_title);
  slot<void> set_author = compose(mem_fun(*m_song, &Song::set_author),
				  mem_fun(m_ent_author, &Entry::get_text));
  m_ent_author->signal_changed().connect(set_author);
}


void InfoEditor::reset_gui() {
  m_ent_title->set_text(m_song->get_title());
  m_ent_author->set_text(m_song->get_author());
  //XXX fix this 
  //m_text_info->set_text(m_song->get_info());
}


