#include <cassert>
#include <map>

#include "trackwidget.hpp"


using namespace std;


TrackWidget::TrackWidget(const Song* song) 
  : mSong(song), mColWidth(20), mDragBeat(-1), mDragPattern(-1) {
  assert(song);
  mColormap  = Colormap::get_system();
  bgColor.set_rgb(65535, 65535, 65535);
  bgColor2.set_rgb(65535, 60000, 50000);
  fgColor.set_rgb(40000, 40000, 65535);
  gridColor.set_rgb(40000, 40000, 40000);
  edgeColor.set_rgb(0, 0, 0);
  hlColor.set_rgb(65535, 0, 0);
  mColormap->alloc_color(bgColor);
  mColormap->alloc_color(bgColor2);
  mColormap->alloc_color(fgColor);
  mColormap->alloc_color(gridColor);
  mColormap->alloc_color(edgeColor);
  mColormap->alloc_color(hlColor);
  
  add_events(BUTTON_PRESS_MASK | BUTTON_RELEASE_MASK | BUTTON_MOTION_MASK);
  set_size_request(mColWidth * mSong->getLength(), mColWidth);
}
  

void TrackWidget::setTrack(Track* track) {
  assert(track);
  mTrack = track;
}


void TrackWidget::on_realize() {
  DrawingArea::on_realize();
  RefPtr<Gdk::Window> win = get_window();
  gc = GC::create(win);
  gc->set_background(bgColor);
  gc->set_foreground(fgColor);
  win->clear();
}


bool TrackWidget::on_expose_event(GdkEventExpose* event) {

  RefPtr<Gdk::Window> win = get_window();
  win->clear();
  
  int width = mColWidth * mSong->getLength();
  int height = mColWidth;
  Rectangle bounds(0, 0, width + 1, height);
  gc->set_clip_rectangle(bounds);
  
  // draw background
  int bpb = 4;
  for (int b = 0; b < mSong->getLength(); ++b) {
    if (b % (2*bpb) < bpb)
      gc->set_foreground(bgColor);
    else
      gc->set_foreground(bgColor2);
    win->draw_rectangle(gc, true, b * mColWidth, 0, mColWidth, height);
  }
  gc->set_foreground(gridColor);
  win->draw_line(gc, 0, 0, width, 0);
  win->draw_line(gc, 0, height-1, width, height-1);
  for (int c = 0; c < mSong->getLength() + 1; ++c) {
    win->draw_line(gc, c * mColWidth, 0, c * mColWidth, height);
  }
  
  // draw patterns
  Pango::FontDescription fd("helvetica bold 12");
  get_pango_context()->set_font_description(fd);
  Track::Sequence::const_iterator iter = mTrack->getSequence().begin();
  char tmp[10];
  for ( ; iter != mTrack->getSequence().end(); ++iter) {
    gc->set_clip_rectangle(bounds);
    gc->set_foreground(fgColor);
    int length = iter->second->length;
    win->draw_rectangle(gc, true, iter->first * mColWidth, 0,
			length * mColWidth, height-1);
    gc->set_foreground(edgeColor);
    win->draw_rectangle(gc, false, iter->first * mColWidth, 0,
			length * mColWidth, height-1);
    Glib::RefPtr<Pango::Layout> l = Pango::Layout::create(get_pango_context());
    sprintf(tmp, "%03d", iter->second->patternID);
    l->set_text(tmp);
    int lHeight = l->get_pixel_logical_extents().get_height();
    Rectangle textBounds(iter->first * mColWidth, 0, 
			 length * mColWidth, height - 1);
    gc->set_clip_rectangle(textBounds);
    win->draw_layout(gc, iter->first * mColWidth + 2, (height - lHeight)/2, l);
  }
  
  return true;
}


bool TrackWidget::on_button_press_event(GdkEventButton* event) {
  int beat = int(event->x) / mColWidth;

  switch (event->button) {
  case 1: {
    char tmp[10];
    std::map<int, Pattern>::const_iterator iter= mTrack->getPatterns().begin();
    mPatternMenu.items().clear();
    for ( ; iter != mTrack->getPatterns().end(); ++iter) {
      sprintf(tmp, "%03d", iter->first);
      Menu_Helpers::MenuElem
	elem(tmp, bind(bind(mem_fun(*this, &TrackWidget::slotInsertPattern), 
			    beat), iter->first));
      mPatternMenu.items().push_back(elem);
    }
    if (mPatternMenu.items().size() == 0) {
      Menu_Helpers::MenuElem elem("No patterns");
      mPatternMenu.items().push_back(elem);
    }
    mPatternMenu.popup(event->button, event->time);
    return true;
  }
    
  case 2: {
    int bBeat, pattern, length;
    if (mTrack->getSequenceEntry(beat, bBeat, pattern, length)) {
      mDragBeat = bBeat;
      mDragPattern = pattern;
      mTrack->setSequenceEntry(bBeat, pattern, beat - bBeat + 1);
      update();
    }
    return true;
  }
    
  case 3:
    mTrack->removeSequenceEntry(beat);
    update();
    return true;
  } 
  
  return false;
}


bool TrackWidget::on_button_release_event(GdkEventButton* event) {
  if (event->button == 2)
    mDragBeat = -1;
}


bool TrackWidget::on_motion_notify_event(GdkEventMotion* event) {
  int beat = int(event->x) / mColWidth;

  if ((event->state & GDK_BUTTON2_MASK) && mDragBeat != -1 &&
      beat >= mDragBeat && 
      beat - mDragBeat +1 <= mTrack->getPatterns()[mDragPattern].getLength()) {
    mTrack->setSequenceEntry(mDragBeat, mDragPattern, beat - mDragBeat + 1);
    update();
    return true;
  }
  
  return false;
}


void TrackWidget::slotInsertPattern(int pattern, int position) {
  int length = mTrack->getPatterns()[pattern].getLength();
  mTrack->setSequenceEntry(position, pattern, length);
  update();
}


void TrackWidget::update() {
  RefPtr<Gdk::Window> win = get_window();
  win->invalidate_rect(Rectangle(0, 0, get_width(), get_height()), false);
  win->process_updates(false);
}
