#ifndef PATTERNEDITOR_HPP
#define PATTERNEDITOR_HPP

#include <gtkmm.h>

#include "pattern.hpp"


using namespace Gdk;
using namespace Glib;
using namespace Gtk;


class PatternEditor : public DrawingArea {
public:
  PatternEditor();
  
  void setPattern(Pattern* pat);
  
protected:
  
  // event handlers
  virtual bool on_button_press_event(GdkEventButton* event);
  virtual bool on_button_release_event(GdkEventButton* event);
  virtual bool on_motion_notify_event(GdkEventMotion* event);
  virtual void on_realize();
  virtual bool on_expose_event(GdkEventExpose* event);
  
  void update();
  
private:
  RefPtr<GC> gc;
  RefPtr<Colormap> colormap;
  Color bgColor, bgColor2, fgColor, gridColor, edgeColor, hlColor;
  int rowHeight;
  int colWidth;
  int maxNote;
  
  pair<int, int> addedNote;
  int dragStep;
  int dragNote;
  
  RefPtr<Pixmap> pix;
  int dMinStep, dMaxStep, dMinNote, dMaxNote;
  Pattern* pat;
};


#endif

