#ifndef TRACKWIDGET_HPP
#define TRACKWIDGET_HPP

#include <gtkmm.h>

#include "song.hpp"
#include "track.hpp"


using namespace Gdk;
using namespace Glib;
using namespace Gtk;


class TrackWidget : public DrawingArea {
public:
  
  TrackWidget(const Song* song = NULL);
  
  void setTrack(Track* track);

  virtual void on_realize();
  virtual bool on_expose_event(GdkEventExpose* event);
  virtual bool on_button_press_event(GdkEventButton* event);
  virtual bool on_button_release_event(GdkEventButton* event);
  virtual bool on_motion_notify_event(GdkEventMotion* event);
  
  void slotInsertPattern(int pattern, int position);
  
  void update();
  
private:
  
  const Song* mSong;
  Track* mTrack;
  int mColWidth;

  RefPtr<GC> gc;
  RefPtr<Colormap> mColormap;
  Color bgColor, bgColor2, fgColor, gridColor, edgeColor, hlColor;
  
  int mDragBeat, mDragPattern;
  
  Menu mPatternMenu;
};


#endif
