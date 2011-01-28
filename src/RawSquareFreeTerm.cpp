/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2010 University of Aarhus
   Contact Bjarke Hammersholt Roune for license information (www.broune.com)

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
#include "stdinc.h"
#include "RawSquareFreeTerm.h"

#include <sstream>
#include <vector>

namespace SquareFreeTermOps {
  Word* newTermParse(const char* strParam) {
	string str(strParam);
	Word* term = newTerm(str.size());
	for (size_t var = 0; var < str.size(); ++var) {
	  ASSERT(str[var] == '0' || str[var] == '1');
	  setExponent(term, var, str[var] == '1' ? 1 : 0);
	}
	return term;
  }

  void print(FILE* file, const Word* term, size_t varCount) {
	ostringstream out;
	print(out, term, varCount);
	fputs(out.str().c_str(), file);
  }

  void print(ostream& out, const Word* term, size_t varCount) {
	ASSERT(term != 0 || varCount == 0);

	out << '(';
	for (size_t var = 0; var < varCount; ++var) {
	  if (var != 0)
		out << ", ";
	  out << getExponent(term, var);
	}
	out << ')';
  }
}
