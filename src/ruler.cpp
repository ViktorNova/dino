#include <iostream>
using namespace std;
#include "ruler.hpp"


::Ruler::Ruler(int length, int subs, int interval, int size, int height)
  : mLength(length), mSubs(subs), mInterval(interval), 
    mDivSize(size), mHeight(height) {
  set_size_request(mLength * mDivSize + 1, mHeight);
  add_events(BUTTON_PRESS_MASK | BUTTON_RELEASE_MASK | BUTTON_MOTION_MASK);
}

  
void ::Ruler::setLength(int length) {
  mLength = length;
  set_size_request(mLength * mDivSize + 1, mHeight);
}


void ::Ruler::setSubdivisions(int subs) {
  mSubs = subs;
}


void ::Ruler::setInterval(int interval) {
  mInterval = interval;
}


void ::Ruler::setDivisionSize(int size) {
  mDivSize = size;
  set_size_request(mLength * mDivSize + 1, mHeight);
}


void ::Ruler::on_realize() {
  Gtk::DrawingArea::on_realize();
  Glib::RefPtr<Gdk::Window> win = get_window();
  gc = GC::create(win);
  win->clear();
}


bool ::Ruler::on_expose_event(GdkEventExpose* event) {

  Glib::RefPtr<Gdk::Window> win = get_window();
  win->clear();
  if (mLength <= 0)
    return true;
  
  int width = mLength * mDivSize + 1, height = mHeight;

  Pango::FontDescription fd("helvetica bold 12");
  get_pango_context()->set_font_description(fd);
  char tmp[10];
  
  for (int i = 0; i <= mLength; ++i) {
    win->draw_line(gc, i * mDivSize, height - 4, i * mDivSize, height);
    if (i % mInterval == 0 && i != 0) {
      Glib::RefPtr<Pango::Layout> l = 
	Pango::Layout::create(get_pango_context());
      sprintf(tmp, "%d", i);
      l->set_text(tmp);
      Pango::Rectangle rect = l->get_pixel_logical_extents();
      win->draw_layout(gc, i * mDivSize - rect.get_width() / 2, 
		       (height - 4 - rect.get_height()) / 2, l);
    }
    if (i == mLength)
      break;
    for (int j = 1; j < mSubs; ++j) {
      win->draw_line(gc, i * mDivSize + j * mDivSize / mSubs, height - 2,
		     i * mDivSize + j * mDivSize / mSubs, height);
    }
  }
  return true;
}


bool ::Ruler::on_button_press_event(GdkEventButton* event) {
  double pos = event->x / mDivSize;
  signal_clicked(pos, event->button);
}
