#include <gtkmm.h>

class URLItemBox:public Gtk::Box{
  public:
  URLItemBox();
  virtual ~URLItemBox();
  void enableButtons(bool enable);
  void clearResult();
  
  Gtk::Label m_statusLabel;
  Gtk::Entry m_urlText;
  Gtk::Button m_editBtn;
  Gtk::Button m_deleteBtn;
  Gtk::Button m_checkBtn;
  Gtk::Spinner m_spinner;
  Gtk::Image m_floppyImg;
  Gtk::Image m_deleteImg;
};
