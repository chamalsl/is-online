#include <gtkmm.h>
#include "url_item_box.h"

URLItemBox::URLItemBox():
m_checkBtn("Check")
{

  m_floppyImg.set_from_resource("/images/floppy.svg");
  m_deleteImg.set_from_resource("/images/delete.svg");

  m_editBtn.set_image(m_floppyImg);
  m_editBtn.set_tooltip_text("save");
  m_deleteBtn.set_image(m_deleteImg);
  m_deleteBtn.set_tooltip_text("delete");
  m_statusLabel.set_width_chars(5);
  m_statusLabel.set_name("status_label");
  m_urlText.set_width_chars(35);
  set_margin_top(5);
  pack_start(m_statusLabel, false, false, 5);
  pack_start(m_urlText, false, false, 5);
  pack_start(m_editBtn, false, false, 5);
  pack_start(m_deleteBtn, false, false, 5);
  pack_start(m_checkBtn, false, false, 5);
  pack_start(m_spinner, false, false, 5);
}

URLItemBox::~URLItemBox(){

}

void URLItemBox::enableButtons(bool enable)
{
  m_checkBtn.set_sensitive(enable);
  m_deleteBtn.set_sensitive(enable);
  m_editBtn.set_sensitive(enable);
}

void URLItemBox::clearResult()
{
  m_statusLabel.set_name("status_label");
  m_statusLabel.set_text("");
}
