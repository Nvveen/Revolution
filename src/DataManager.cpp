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
  file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    file.open(dataset);
    while (!file.eof()) {
      std::string line;
      getline(file, line);
      static bool test = true;
      if (test) {
        std::cout << line << std::endl;
        test = false;
      }
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
