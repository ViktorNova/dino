#ifndef RULER_HPP
#define RULER_HPP

#include <gtkmm.h>


using namespace Gtk;
using namespace Gdk;
using namespace Glib;


class Ruler : public DrawingArea {
public:
  Ruler(int length, int subs, int interval, int size, int height);
  
  void setLength(int length);
  void setSubdivisions(int subs);
  void setInterval(int interval);
  void setDivisionSize(int size);

  virtual void on_realize();
  virtual bool on_expose_event(GdkEventExpose* event);
  
private:
  
  int mLength;
  int mSubs;
  int mInterval;
  int mDivSize;
  int mHeight;
  
  RefPtr<GC> gc;
  RefPtr<Colormap> mColormap;
  
};

#endif
