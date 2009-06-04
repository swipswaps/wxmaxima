///
///  Copyright (C) 2009 Ziga Lenarcic <zigalenarcic@users.sourceforge.net>
///
///  This program is free software; you can redistribute it and/or modify
///  it under the terms of the GNU General Public License as published by
///  the Free Software Foundation; either version 2 of the License, or
///  (at your option) any later version.
///
///  This program is distributed in the hope that it will be useful,
///  but WITHOUT ANY WARRANTY; without even the implied warranty of
///  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///  GNU General Public License for more details.
///
///
///  You should have received a copy of the GNU General Public License
///  along with this program; if not, write to the Free Software
///  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
///

#ifndef INSPECTOR_H
#define INSPECTOR_H

#include <wx/wx.h>
#include <wx/splitter.h>

#include "MiniMathCtrl.h"

enum {
  inspector_listbox_id
};

class Inspector : public wxPanel
{
public:
  Inspector(wxWindow* parent, int id);
  ~Inspector();
  void SetValues(wxArrayString arrstr);
private:
  //wxSplitterWindow *m_splitter;
  wxListBox *m_listbox;
  MiniMathCtrl *m_minimathctrl;
  //DECLARE_EVENT_TABLE()
};

#endif // INSPECTOR_H
