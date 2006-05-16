#ifndef PLUGINDIALOG_HPP
#define PLUGINDIALOG_HPP

#include <gtkmm.h>
#include <libglademm.h>

#include "pluginlibrary.hpp"


class PluginDialog : public Gtk::Dialog {
public:
  
  PluginDialog(BaseObjectType* cobject, 
	       const Glib::RefPtr<Gnome::Glade::Xml>& refGlade);
  
  void set_library(PluginLibrary& plib);
  
protected:
  
  class ModelColumns : public Gtk::TreeModel::ColumnRecord {
  public:
    Gtk::TreeModelColumn<bool> loaded;
    Gtk::TreeModelColumn<std::string> name;
    Gtk::TreeModelColumn<std::string> filename;
    
    ModelColumns() { 
      add(loaded);
      add(name); 
      add(filename); 
    }
  } m_columns;

  
  void row_changed(const Gtk::TreeModel::iterator& iter);
  void loaded(PluginLibrary::iterator iter);
  void unloaded(PluginLibrary::iterator iter);
  
  
  Glib::RefPtr<Gtk::ListStore> m_store;
  
  PluginLibrary* m_plib;
  
  Gtk::TreeView* m_view;
};


#endif
