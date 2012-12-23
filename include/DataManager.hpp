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
#ifndef  DATAMANAGER_HPP
#define  DATAMANAGER_HPP

#include <string>

class DataManager {
  public:
    DataManager ();
    virtual ~DataManager ();
  protected:
  private:
    static DataManager * readFile ( std::string const & dataset );
};

class HeightDataManager : public DataManager {
  public:
    HeightDataManager ( std::string const & dataset );
    virtual ~HeightDataManager ();
  protected:
  private:
};

#endif