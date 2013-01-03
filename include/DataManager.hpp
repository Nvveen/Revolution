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
#include <vector>
#include <map>
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

    static DataManager * readFile ( std::string const & dataset);
    virtual void activate ( unsigned int const & dimension ) = 0;
    virtual void deactivate () = 0;

    std::vector<unsigned int> const & getDimensions ();
    unsigned int const & getActiveDimension ();

    std::string name;
    DataManagerType type;

    static DataManager *setHeight;
  protected:
    virtual void init ( std::ifstream & dataset );
    virtual void normalize () = 0;

    std::map<Drawable *, std::vector<double>> _dataMembers;
    std::vector<unsigned int> _dim;
    bool _active;
    unsigned int _activeDimension;
  private:
};

class HeightDataManager : public DataManager {
  public:
    HeightDataManager ( std::ifstream & dataset );
    virtual ~HeightDataManager ();

    virtual void activate ( unsigned int const & dimension );
    virtual void deactivate ();
  protected:
    virtual void normalize ();
  private:
};

class ColorDataManager : public DataManager {
  public:
    ColorDataManager ( std::ifstream & dataset );
    virtual ~ColorDataManager ();

    virtual void activate ( unsigned int const & dimension );
    virtual void deactivate ();
  protected:
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

inline std::vector<unsigned int> const & DataManager::getDimensions ()
{
  return _dim;
}

inline unsigned int const & DataManager::getActiveDimension ()
{
  if (!_active) throw DataManagerException("inactive datamanager");
  else return _activeDimension;
}

#endif
