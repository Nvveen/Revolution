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
#include <fstream>
#include <iostream>
#include <boost/tokenizer.hpp>
#include <vector>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <map>
#include "DataManager.hpp"

DataManager::DataManager ()
{
}

DataManager::~DataManager ()
{
}

DataManager * DataManager::readFile ( std::string const & dataset )
{
  std::ifstream file;
  std::string type;
  std::vector<unsigned int> years;
  std::map<std::string, std::vector<double>> gdpMap;
  try {
    file.open(dataset);
    std::string line;
    unsigned int row = 0, col = 0;
    while (std::getline(file, line)) {
      typedef boost::tokenizer<boost::escaped_list_separator<char>> Tokenizer;
      Tokenizer tok(line);
      std::string name;
      for (auto item : tok) {
        std::stringstream buf(item);
        if (row == 0) {
          if (col == 0) {
            type = item;
          } else {
            years.emplace_back();
            buf >> years.back();
          }
        } else {
          if (col == 0) {
            buf >> name;
            gdpMap[name] = std::vector<double>(years.size(), -1);
          } else {
            buf >> gdpMap[name][col-1];
          }
        }
        col++;
      }
      col = 0;
      row++;
    }
  }
  catch (std::ifstream::failure & e) {
    std::cerr << "exception opening file " << e.what() << std::endl;
  }
  return NULL;
}

HeightDataManager::HeightDataManager ( std::string const & dataset )
{
}

HeightDataManager::~HeightDataManager ()
{
}
