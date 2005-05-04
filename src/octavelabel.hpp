#ifndef OCTAVELABEL_HPP
#define OCTAVELABEL_HPP

#include <gtkmm.h>


using namespace Gtk;
using namespace Gdk;
using namespace Glib;


class OctaveLabel : public DrawingArea {
public:
  OctaveLabel(int width, int note_height);
  
  virtual void on_realize();
  virtual bool on_expose_event(GdkEventExpose* event);

private:
  
  int m_width;
  int m_note_height;
  
  RefPtr<GC> m_gc;
  RefPtr<Colormap> m_colormap;
  Color m_bg_color, m_fg_color;  
};


#endif
