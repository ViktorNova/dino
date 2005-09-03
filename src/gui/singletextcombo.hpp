#ifndef SINGLETEXTCOMBO_HPP
#define SINGLETEXTCOMBO_HPP

#include <string>

#include <gtkmm.h>

using namespace Glib;
using namespace Gtk;
using namespace std;


/** This class is needed because @c ComboBoxText in gtkmm does not provide a
    way of removing entries. */
class SingleTextCombo : public ComboBox {
private:
  // noncopyable
  SingleTextCombo(const SingleTextCombo&);
  SingleTextCombo& operator=(const SingleTextCombo&);

public:
  SingleTextCombo();
  
  /** Add a string at the end of the combo box with the ID number @c id. */
  void append_text(const ustring& text, int id = -1);
  /** Add a string at the beginning of the combo box with the ID number 
      @c id. */
  void prepend_text(const ustring& text, int id = -1);
  
  /** Returns the ID number for the active entry. */
  int get_active_id() const;
  /** Set the active entry to the one with ID number @c id. If there is no
      such entry, return @c false. */
  bool set_active_id(int id);
  /** Returns the text for the active entry. */
  string get_active_text() const;
  
  /** Remove all entries. */
  void clear();

protected:

  /** These are the columns that are used in the model for this combo box.
      Only the @c mText column is displayed, the @c mID column is used to
      store an additional ID number. */
  class TextModelColumns : public TreeModel::ColumnRecord {
  public:
    TextModelColumns() { add(m_text); add(m_id); }
    TreeModelColumn<ustring> m_text;
    TreeModelColumn<int> m_id;
  };

  TextModelColumns m_text_columns;
  RefPtr<ListStore> m_store;
};


#endif 
