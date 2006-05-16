#include "debug.hpp"
#include "plugindialog.hpp"
#include "pluginlibrary.hpp"


using namespace Glib;
using namespace Gtk;
using namespace Gnome::Glade;
using namespace Dino;


PluginDialog::PluginDialog(BaseObjectType* cobject, const RefPtr<Xml>& xml)
  : Dialog(cobject) {
  m_view = dynamic_cast<TreeView*>(xml->get_widget("trv_plugins"));
  assert(m_view);
}


void PluginDialog::set_library(PluginLibrary& plib) {
  m_plib = &plib;
  
  m_store = ListStore::create(m_columns);
  PluginLibrary::iterator iter;
  for (iter = plib.begin(); iter != plib.end(); ++iter) {
    ListStore::iterator riter = m_store->append();
    (*riter)[m_columns.loaded] = plib.is_loaded(iter);
    (*riter)[m_columns.name] = iter->second.name;
    (*riter)[m_columns.filename] = iter->second.filename;
  }
  
  m_store->signal_row_changed().
    connect(sigc::hide<0>(mem_fun(*this, &PluginDialog::row_changed)));
  plib.signal_plugin_loaded.connect(mem_fun(*this, &PluginDialog::loaded));
  plib.signal_plugin_unloaded.connect(mem_fun(*this, &PluginDialog::unloaded));
  
  m_view->set_model(m_store);
  m_view->append_column_editable("Loaded", m_columns.loaded);
  m_view->append_column("Name", m_columns.name);
  m_view->append_column("Filename", m_columns.filename);
}


void PluginDialog::row_changed(const TreeModel::iterator& iter) {
  PluginLibrary::iterator piter = m_plib->find((*iter)[m_columns.name]);
  if (piter == m_plib->end()) {
    dbg0<<"Trying to load or unload \""<<string((*iter)[m_columns.name])<<"\", "
	<<"which doesn't exist"<<endl;
    return;
  }
  if ((*iter)[m_columns.loaded])
    m_plib->load_plugin(piter);
  else
    m_plib->unload_plugin(piter);
}


void PluginDialog::loaded(PluginLibrary::iterator iter) {
  TreeModel::iterator titer;
  const TreeModel::Children c = m_store->children();
  for (titer = c.begin(); titer != c.end(); ++titer) {
    if (string((*titer)[m_columns.name]) == iter->second.name) {
      (*titer)[m_columns.loaded] = true;
       break;
    }
  }
}


void PluginDialog::unloaded(PluginLibrary::iterator iter) {
  TreeModel::iterator titer;
  const TreeModel::Children c = m_store->children();
  for (titer = c.begin(); titer != c.end(); ++titer) {
    if (string((*titer)[m_columns.name]) == iter->second.name) {
      (*titer)[m_columns.loaded] = false;
       break;
    }
  }
}

