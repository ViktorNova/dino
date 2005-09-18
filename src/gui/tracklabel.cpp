#include <cassert>
#include <iostream>
#include <map>

#include "tracklabel.hpp"
#include "track.hpp"


using namespace std;


TrackLabel::TrackLabel(const Song* song) 
  : m_song(song), m_width(100), m_height(20), m_is_active(false) {
  assert(song);
  m_colormap  = Colormap::get_system();
  m_bg_color.set_rgb(30000, 30000, 60000);
  m_fg_color.set_rgb(0, 0, 0);
  m_colormap->alloc_color(m_bg_color);
  m_colormap->alloc_color(m_fg_color);
  m_layout = Layout::create(get_pango_context());

  add_events(BUTTON_PRESS_MASK | BUTTON_RELEASE_MASK | BUTTON_MOTION_MASK);
  set_size_request(m_width, m_height + 4);
}
  

void TrackLabel::set_track(int id, Track* track) {
  assert(track);
  m_track = track;
  m_id = id;
  m_name_connection.disconnect();
  m_name_connection = m_track->signal_name_changed.
    connect(mem_fun(*this, &TrackLabel::slot_name_changed));
  slot_name_changed(track->get_name());
}


void TrackLabel::on_realize() {
  DrawingArea::on_realize();
  RefPtr<Gdk::Window> win = get_window();
  m_gc = GC::create(win);
  m_gc->set_background(m_bg_color);
  m_gc->set_foreground(m_fg_color);
  FontDescription fd("helvetica bold 9");
  get_pango_context()->set_font_description(fd);
  win->clear();
}


bool TrackLabel::on_expose_event(GdkEventExpose* event) {
  RefPtr<Gdk::Window> win = get_window();
  win->clear();
  if (m_is_active) {
    m_gc->set_foreground(m_bg_color);
    win->draw_rectangle(m_gc, true, 0, 4, m_width, m_height + 4);
  }
  m_gc->set_foreground(m_fg_color);
  int lHeight = m_layout->get_pixel_logical_extents().get_height();
  win->draw_layout(m_gc, 2, 4 + (m_height - lHeight)/2, m_layout);
  return true;
}


bool TrackLabel::on_button_press_event(GdkEventButton* event) {
  signal_clicked(event->button);
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
  if (win) {
    win->invalidate_rect(Gdk::Rectangle(0, 0, m_width, m_height + 4), false);
    win->process_updates(false);
  }
}


void TrackLabel::set_active_track(int id) {
  if (m_is_active != (id == m_id)) {
    m_is_active = (id == m_id);
    update();
  }
}


void TrackLabel::slot_name_changed(const string& name) {
  char tmp[10];
  sprintf(tmp, "%03d ", m_id);
  //m_layout->set_text(string(tmp));
  m_layout->set_text(string(tmp) + name);
  update();
}
