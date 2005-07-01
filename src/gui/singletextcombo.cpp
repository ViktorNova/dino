#include <gtkmm/liststore.h>
#include <gtkmm/cellrenderertext.h>
#include <gtk/gtkcombobox.h>
#include <gtk/gtkcelllayout.h>

#include "singletextcombo.hpp"


SingleTextCombo::SingleTextCombo() {
  m_store = ListStore::create(m_text_columns);
  set_model(m_store);
  pack_start(m_text_columns.m_text);
}


void SingleTextCombo::append_text(const ustring& text, int id) {
  TreeModel::iterator iter = m_store->append();
  TreeModel::Row row = *iter;
  row[m_text_columns.m_text] = text;
  row[m_text_columns.m_id] = id;
}


void SingleTextCombo::prepend_text(const ustring& text, int id) {
  TreeModel::iterator iter = m_store->prepend();
  TreeModel::Row row = *iter;
  row[m_text_columns.m_text] = text;
}


int SingleTextCombo::get_active_id() const {
  int result = -1;
  TreeModel::iterator activeRow = get_active();
  if(activeRow) {
    TreeModel::Row row = *activeRow;
    result = row[m_text_columns.m_id];
  }
  return result;
}


void SingleTextCombo::clear() {
  m_store->clear();
}


bool SingleTextCombo::set_active_id(int ID) {
  RefPtr<TreeModel> model = get_model();
  if(model) {
    for(TreeModel::iterator iter = model->children().begin(); 
	iter != model->children().end(); ++iter) {
      int thisID = (*iter)[m_text_columns.m_id];
      if(thisID == ID) {
        set_active(iter);
        return true;
      }
    }
  }

  //Not found, so mark it as blank:
  unset_active();
  return false;
}
