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
#include <fstream>
#include <stdexcept>
#include "Drawable.hpp"

class World;

enum DataManagerType {
  DM_Height,
  DM_Pattern,
  DM_Color,
};

class DataManager {
  public:
    DataManager ();
    virtual ~DataManager ();

    static DataManager * readFile ( std::string const & dataset,
                                    World & world );
    virtual void activate ( unsigned int const & dimension );
    virtual void deactivate ();

    std::string name;
    DataManagerType type;
  protected:
    virtual void init ( std::ifstream & dataset, World & world ) = 0;

    std::map<Drawable *, std::vector<double>> _dataMembers;
    std::vector<unsigned int> _dim;
  private:
};

class HeightDataManager : public DataManager {
  public:
    HeightDataManager ( std::ifstream & dataset, World & world );
    virtual ~HeightDataManager ();
  protected:
    virtual void init ( std::ifstream & dataset, World & world );
    virtual void normalize ();
  private:
};

class DataManagerException : public std::runtime_error {
  public:
    DataManagerException ( std::string const & text ) :
      std::runtime_error("Data Manager error: "+text) {
    };
    virtual ~DataManagerException () throw() {};
};

#endif
