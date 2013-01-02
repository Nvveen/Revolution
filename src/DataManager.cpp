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
#include <iostream>
#include <boost/tokenizer.hpp>
#include <vector>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <map>
#include "DataManager.hpp"
#include "World.hpp"

DataManager *DataManager::setHeight = NULL;

DataManager::DataManager ()
{
}

DataManager::~DataManager ()
{
}

DataManager * DataManager::readFile ( std::string const & dataset,
    World & world )
{
  std::ifstream file;
  DataManager * man = NULL;
  try {
    file.open(dataset);
    std::string line;
    getline(file, line);
    boost::tokenizer<boost::escaped_list_separator<char>> tok(line);
    boost::char_separator<char> sep("|");
    boost::tokenizer<boost::char_separator<char>> strTok(*tok.begin(), sep);
    auto it = strTok.begin();
    std::string name = *it; ++it;
    std::string type = *it;
    if (type.size() != 1) {
      throw (DataManagerException("wrong token defined"));
    } else {
      file.seekg(0);
      switch (type[0]) {
        case 'H':
          man = new HeightDataManager(file, world);
          man->type = DM_Height;
          break;
        case 'C':
          man = new ColorDataManager(file, world);
          man->type = DM_Color;
          break;
        default:
          throw (DataManagerException("wrong dataset type defined"));
      }
    }
    if (man != NULL)
      man->name = name;
    std::cout << "Loaded dataset " << man->name << std::endl;
  }
  catch (std::ifstream::failure & e) {
    std::cerr << "Error opening file " << e.what() << std::endl;
  }
  catch (DataManagerException & e ) {
    std::cerr << e.what() << std::endl;
  }
  return man;
}

void DataManager::init ( std::ifstream & dataset, World & world )
{
  typedef boost::tokenizer<boost::escaped_list_separator<char>> Tokenizer;
  std::string line;
  unsigned int row = 0;
  // TODO make proper error
  try {
    while (std::getline(dataset, line)) {
      Drawable * d = NULL;
      Tokenizer tokens(line);
      unsigned int col = 0;
      for (auto token : tokens) {
        std::stringstream buf(token);
        if (row == 0 && col > 0) {
          _dim.emplace_back();
          buf >> _dim.back();
        } else if (row > 0) {
          if (col == 0) {
            d = world.getCountry(token);
            if (d != NULL)
              _dataMembers[d] = std::vector<double>(_dim.size(), 0);
            else
              throw(false);
          } else {
            buf >> _dataMembers[d][col-1];
          }
        }
        col++;
      }
      row++;
    }
  }
  catch (bool found) {
    std::cerr << "Country not found, exiting..." << std::endl;
    throw;
  }
}

HeightDataManager::HeightDataManager ( std::ifstream & dataset,
                                       World & world )
{
  init(dataset, world);
  normalize();
}

HeightDataManager::~HeightDataManager ()
{
}

void HeightDataManager::activate ( unsigned int const & dimension )
{
  std::cout << "Activating " << name << " in dimension " << dimension;
  std::cout << std::endl;
  if (DataManager::setHeight != NULL && this->type == DM_Height) {
    std::cout << "Deactivating previous dataset " << setHeight->name;
    std::cout << std::endl;
    setHeight->deactivate();
    setHeight = this;
  }
  unsigned int index = std::find(_dim.begin(), _dim.end(), dimension) -
    _dim.begin();
  for (auto & keyVal : _dataMembers)
    keyVal.first->setHeightDistortion(keyVal.second[index]);
}

void HeightDataManager::deactivate ()
{
  for (auto & keyVal : _dataMembers)
    keyVal.first->setHeightDistortion(0);
}

void HeightDataManager::normalize ()
{
  // Find max element
  double max = 0;
  for (auto & keyVal : _dataMembers)
    max = std::max(max, *std::max_element(keyVal.second.begin(),
                                          keyVal.second.end()));
  // Divide all by max
  if (max > 0)
    for (auto & keyVal : _dataMembers)
      std::for_each(keyVal.second.begin(), keyVal.second.end(),
                    [&](double & d) {
                      d *= (Drawable::maxHeightDistortion/max);
                    });
}

ColorDataManager::ColorDataManager ( std::ifstream & dataset, World & world )
{
  init(dataset, world);
}

ColorDataManager::~ColorDataManager ()
{
}

void ColorDataManager::activate ( unsigned int const & dimension )
{
}

void ColorDataManager::deactivate ()
{
}

void ColorDataManager::normalize ()
{
  for (unsigned int i = 0; i < _dim.size(); i++) {
    double max = 0;
    for (auto keyVal : _dataMembers)
      if (max < keyVal.second[i]) max = keyVal.second[i];
    for (auto & keyVal : _dataMembers) {
      keyVal.second[i] /= max;
    }
  }
}
