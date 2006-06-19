#ifndef LV2WIDGET_HPP
#define LV2WIDGET_HPP

#include <map>
#include <string>

#include <gtkmm.h>
#include <lo/lo.h>


class LV2Widget : public Gtk::Table {
public:
  
  LV2Widget(const std::string& uri);
  
protected:
  
  void send_control(int port, float value);
  
  lo_address m_address;
  
};


#endif
