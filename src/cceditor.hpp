#ifndef CCEDITOR_HPP
#define CCEDITOR_HPP

#include <gtkmm.h>

#include "pattern.hpp"


using namespace Gdk;
using namespace Glib;
using namespace Gtk;


class CCEditor : public DrawingArea {
public:
  CCEditor();
  
  void setPattern(Pattern* pat);
  void setCCNumber(int ccNumber);
  void setHeight(int height);  
  
protected:
  
  // event handlers
  virtual bool on_button_press_event(GdkEventButton* event);
  virtual bool on_button_release_event(GdkEventButton* event);
  virtual bool on_motion_notify_event(GdkEventMotion* event);
  virtual void on_realize();
  virtual bool on_expose_event(GdkEventExpose* event);
  
  void update(int step = -1);
  
private:
  RefPtr<GC> gc;
  RefPtr<Colormap> colormap;
  Color bgColor, bgColor2, fgColor, edgeColor;
  int colWidth;
  int width, height;
  double vScale;
  
  Pattern* pat;
  int ccNumber;
  
  int dragStep;
  int lineStep;
  int lineValue;
  
  Menu shapeMenu;
};


#endif

