/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2007 Bjarke Hammersholt Roune (www.broune.com)

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
#ifndef FOURTI2_IO_HANDLER_GUARD
#define FOURTI2_IO_HANDLER_GUARD

#include "IOHandler.h"

class Scanner;
class VarNames;
class BigIdeal;

class Fourti2IOHandler : public IOHandler {
public:
  virtual void readIdeal(Scanner& scanner, BigIdeal& ideal);
  virtual void readIrreducibleDecomposition(Scanner& scanner, BigIdeal& decom);
  virtual void readTerm(Scanner& in, const VarNames& names,
						vector<mpz_class>& term);

  virtual void writeIdeal(FILE* out, const BigIdeal& ideal);

  // These are not supported. Use writeIdeal instead.
  virtual IdealWriter* createWriter
    (FILE* file, const VarNames& names) const;
  virtual IdealWriter* createWriter
    (FILE* file, const TermTranslator* translator) const;

  virtual const char* getFormatName() const;

 private:
  void displayWarning() const;
};

#endif
