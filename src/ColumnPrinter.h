/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2009 Bjarke Hammersholt Roune (www.broune.com)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see http://www.gnu.org/licenses/.
*/
#ifndef COLUMN_PRINTER_GUARD
#define COLUMN_PRINTER_GUARD

#include "ElementDeleter.h"

#include <sstream>
#include <vector>

class FrobbyStringStream;

class ColumnPrinter {
 public:
  ColumnPrinter(size_t columnCount = 0);

  void setPrefix(const string& prefix);
  void addColumn(bool flushLeft = true,
				 const string& prefix = "  ", const string& suffix = "");
  size_t getColumnCount() const;

  ostream& operator[](size_t col);

  void print(ostream& out) const;
  void print(FrobbyStringStream& out) const;

 private:
  struct Col {
	string prefix;
	stringstream text;
	string suffix;
	bool flushLeft;
  };
  vector<Col*> _cols;
  ElementDeleter<vector<Col*> > _colsDeleter;
  string _prefix;
};

ostream& operator<<(ostream& out, const ColumnPrinter& printer);
void print(FILE* out, const ColumnPrinter& pr);

#endif
