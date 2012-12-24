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
    unsigned int linenr = 0;
    while (std::getline(file, line)) {
      typedef boost::tokenizer<boost::escaped_list_separator<char>> Tokenizer;
      Tokenizer tok(line);
      if (line[1] == 'H' && linenr == 0) {
        man = new HeightDataManager(file, world);
      }
      linenr++;
    }
  }
  catch (std::ifstream::failure & e) {
    std::cerr << "exception opening file " << e.what() << std::endl;
  }
  return man;
}

void DataManager::activate ( unsigned int const & dimension )
{
  unsigned int index = *std::find(_dim.begin(), _dim.end(), dimension);
  for (auto & keyVal : _dataMembers)
    keyVal.first->setHeightDistortion(keyVal.second[index]);
}

void DataManager::deactivate ()
{
  for (auto & keyVal : _dataMembers)
    keyVal.first->setHeightDistortion(0);
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

void HeightDataManager::init ( std::ifstream & dataset, World & world )
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
        if (row == 0) {
          _dim.emplace_back();
          buf >> _dim.back();
        } else {
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

void HeightDataManager::normalize ()
{
  // Find max element
  double max = 0;
  for (auto & keyVal : _dataMembers)
    max = std::max(max, *std::max_element(keyVal.second.begin(),
                                          keyVal.second.end()));
  // Divide all by max
  for (auto & keyVal : _dataMembers)
    std::for_each(keyVal.second.begin(), keyVal.second.end(),
                  [&](double & d) {
                    d *= (Drawable::maxHeightDistortion/max);
                  });
}
