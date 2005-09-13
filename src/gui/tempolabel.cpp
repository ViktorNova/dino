#include <cassert>
#include <iostream>
#include <map>

#include "song.hpp"
#include "tempolabel.hpp"


using namespace std;


TempoLabel::TempoLabel(const Song* song) 
  : m_song(song), m_width(100), m_height(20) {
  assert(song);
  m_colormap  = Colormap::get_system();
  m_fg_color.set_rgb(0, 0, 0);
  m_colormap->alloc_color(m_fg_color);
  m_layout = Layout::create(get_pango_context());
  m_layout->set_text("BPM");
  set_size_request(m_width, m_height);
}
  

void TempoLabel::on_realize() {
  DrawingArea::on_realize();
  RefPtr<Gdk::Window> win = get_window();
  m_gc = GC::create(win);
  m_gc->set_background(m_bg_color);
  m_gc->set_foreground(m_fg_color);
  FontDescription fd("helvetica bold 9");
  get_pango_context()->set_font_description(fd);
  win->clear();
}


bool TempoLabel::on_expose_event(GdkEventExpose* event) {
  RefPtr<Gdk::Window> win = get_window();
  //win->clear();
  m_gc->set_foreground(m_fg_color);
  int lHeight = m_layout->get_pixel_logical_extents().get_height();
  win->draw_layout(m_gc, 2, (m_height - lHeight)/2, m_layout);
  return true;
}


void TempoLabel::update() {
  RefPtr<Gdk::Window> win = get_window();
  if (win) {
    win->invalidate_rect(Gdk::Rectangle(0, 0, m_width, m_height), false);
    win->process_updates(false);
  }
}


