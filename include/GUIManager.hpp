// This file is part of Revolution.
// 
// Revolution is free software: you can redistribute it and/or modify it under 
// the terms of the GNU General Public License as published by the 
// Free Software Foundation, either version 3 of the License, or 
// (at your option) any later version.
// 
// Revolution is distributed in the hope that it will be useful, but WITHOUT 
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for 
// more details.
// 
// You should have received a copy of the GNU General Public License along with 
// Revolution. If not, see <http://www.gnu.org/licenses/>.
#ifndef GUIMANAGER_HPP
#define GUIMANAGER_HPP

#include <string>
#include <stdexcept>
#include <CEGUI/CEGUI.h>
#include <vector>
#include "DataManager.hpp"

class GUIManager {
  public:
    GUIManager ();
    virtual ~GUIManager ();

    void render ();
    void populateDataList ( std::vector<DataManager *> const & list );
  protected:
  private:
    void init ();
    void setCEGUIPaths ();
    void createGUI ();
    void setEventHandles ();

    std::string CEGUIInstallBasePath;
};

class GUIManagerException : public std::runtime_error {
  public:
    GUIManagerException ( std::string const & msg ) :
      std::runtime_error("GUIException: "+msg) {};
    virtual ~GUIManagerException () throw() {};
};

class ListItem : public CEGUI::ListboxTextItem {
  friend class GUIManager;
  private:
    ListItem ( std::string const & text, int const & id = 0 ) :
      CEGUI::ListboxTextItem(text, id) {
      this->setSelectionBrushImage("TaharezLook", "MultiListSelectionBrush");
    };
};

#endif
