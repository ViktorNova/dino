#include <gtkmm/liststore.h>
#include <gtkmm/cellrenderertext.h>
#include <gtk/gtkcombobox.h>
#include <gtk/gtkcelllayout.h>

#include "singletextcombo.hpp"


SingleTextCombo::SingleTextCombo() {
  mStore = ListStore::create(mTextColumns);
  set_model(mStore);
  pack_start(mTextColumns.mText);
}


void SingleTextCombo::appendText(const ustring& text, int id) {
  TreeModel::iterator iter = mStore->append();
  TreeModel::Row row = *iter;
  row[mTextColumns.mText] = text;
  row[mTextColumns.mID] = id;
}


void SingleTextCombo::prependText(const ustring& text, int id) {
  TreeModel::iterator iter = mStore->prepend();
  TreeModel::Row row = *iter;
  row[mTextColumns.mText] = text;
}


int SingleTextCombo::getActiveID() const {
  int result = -1;
  TreeModel::iterator activeRow = get_active();
  if(activeRow) {
    TreeModel::Row row = *activeRow;
    result = row[mTextColumns.mID];
  }
  return result;
}


void SingleTextCombo::clear() {
  mStore->clear();
}


bool SingleTextCombo::setActiveID(int ID) {
  RefPtr<TreeModel> model = get_model();
  if(model) {
    for(TreeModel::iterator iter = model->children().begin(); 
	iter != model->children().end(); ++iter) {
      int thisID = (*iter)[mTextColumns.mID];
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
