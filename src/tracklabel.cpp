#include <cassert>
#include <iostream>
#include <map>

#include "tracklabel.hpp"


using namespace std;


TrackLabel::TrackLabel(const Song* song) 
  : m_song(song), m_width(100), m_height(20) {
  assert(song);
  m_colormap  = Colormap::get_system();
  m_bg_color.set_rgb(20000, 20000, 50000);
  m_fg_color.set_rgb(0, 0, 0);
  m_colormap->alloc_color(m_bg_color);
  m_colormap->alloc_color(m_fg_color);
  
  add_events(BUTTON_PRESS_MASK | BUTTON_RELEASE_MASK | BUTTON_MOTION_MASK);
  set_size_request(m_width, m_height);
}
  

void TrackLabel::set_track(int id, Track* track) {
  assert(track);
  m_track = track;
  m_id = id;
}


void TrackLabel::on_realize() {
  DrawingArea::on_realize();
  RefPtr<Gdk::Window> win = get_window();
  m_gc = GC::create(win);
  m_gc->set_background(m_bg_color);
  m_gc->set_foreground(m_fg_color);
  win->clear();
  FontDescription fd("helvetica bold 12");
  get_pango_context()->set_font_description(fd);
  m_layout = Layout::create(get_pango_context());
  char tmp[10];
  sprintf(tmp, "%03d ", m_id);
  m_layout->set_text(string(tmp) + m_track->get_name());
}


bool TrackLabel::on_expose_event(GdkEventExpose* event) {
  RefPtr<Gdk::Window> win = get_window();
  //win->clear();
  m_gc->set_foreground(m_bg_color);
  win->draw_rectangle(m_gc, true, 0, 0, m_width, m_height);
  m_gc->set_foreground(m_fg_color);
  int lHeight = m_layout->get_pixel_logical_extents().get_height();
  win->draw_layout(m_gc, 2, (m_height - lHeight)/2, m_layout);
  return true;
}


bool TrackLabel::on_button_press_event(GdkEventButton* event) {
  return true;
}


bool TrackLabel::on_button_release_event(GdkEventButton* event) {
  return true;
}


bool TrackLabel::on_motion_notify_event(GdkEventMotion* event) {
  return true;
}


void TrackLabel::update() {
  RefPtr<Gdk::Window> win = get_window();
  win->invalidate_rect(Gdk::Rectangle(0, 0, m_width, m_height), false);
  win->process_updates(false);
}
