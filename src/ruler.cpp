#include <iostream>
using namespace std;
#include "ruler.hpp"


::Ruler::Ruler(int length, int subs, int interval, int size, int height)
  : m_length(length), m_subs(subs), m_interval(interval), 
    m_div_size(size), m_height(height) {
  set_size_request(m_length * m_div_size + 1, m_height);
  add_events(BUTTON_PRESS_MASK | BUTTON_RELEASE_MASK | BUTTON_MOTION_MASK);
}

  
void ::Ruler::set_length(int length) {
  m_length = length;
  set_size_request(m_length * m_div_size + 1, m_height);
}


void ::Ruler::set_subdivisions(int subs) {
  m_subs = subs;
}


void ::Ruler::set_interval(int interval) {
  m_interval = interval;
}


void ::Ruler::set_division_size(int size) {
  m_div_size = size;
  set_size_request(m_length * m_div_size + 1, m_height);
}


void ::Ruler::on_realize() {
  Gtk::DrawingArea::on_realize();
  Glib::RefPtr<Gdk::Window> win = get_window();
  m_gc = GC::create(win);
  win->clear();
}


bool ::Ruler::on_expose_event(GdkEventExpose* event) {

  Glib::RefPtr<Gdk::Window> win = get_window();
  win->clear();
  if (m_length <= 0)
    return true;
  
  int width = m_length * m_div_size + 1, height = m_height;

  Pango::FontDescription fd("helvetica bold 9");
  get_pango_context()->set_font_description(fd);
  char tmp[10];
  
  for (int i = 0; i <= m_length; ++i) {
    win->draw_line(m_gc, i * m_div_size, height - 4, i * m_div_size, height);
    if (i % m_interval == 0 && i != 0) {
      Glib::RefPtr<Pango::Layout> l = 
	Pango::Layout::create(get_pango_context());
      sprintf(tmp, "%d", i);
      l->set_text(tmp);
      Pango::Rectangle rect = l->get_pixel_logical_extents();
      win->draw_layout(m_gc, i * m_div_size - rect.get_width() / 2, 
		       (height - 4 - rect.get_height()) / 2, l);
    }
    if (i == m_length)
      break;
    for (int j = 1; j < m_subs; ++j) {
      win->draw_line(m_gc, i * m_div_size + j * m_div_size / m_subs, height- 2,
		     i * m_div_size + j * m_div_size / m_subs, height);
    }
  }
  return true;
}


bool ::Ruler::on_button_press_event(GdkEventButton* event) {
  double pos = event->x / m_div_size;
  signal_clicked(pos, event->button);
}
