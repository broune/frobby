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
#ifndef IO_HANDLER_GUARD
#define IO_HANDLER_GUARD

#include "TermConsumer.h"
#include "VarNames.h"

#include <vector>

class Term;
class Scanner;
class BigIdeal;
class TermTranslator;
class IdealWriter;
class Ideal;

class IOHandler {
 public:
  virtual ~IOHandler();

  virtual void readIdeal(Scanner& in, BigIdeal& ideal) = 0;
  virtual void readIrreducibleDecomposition(Scanner& in, BigIdeal& decom) = 0;
  virtual void readTerm(Scanner& in, const VarNames& names,
						vector<mpz_class>& term);

  virtual void writeIdeal(FILE* out, const BigIdeal& ideal);
  virtual void writeIdeal(FILE* out, const Ideal& ideal,
						  const TermTranslator* translator);

  virtual IdealWriter* createWriter
    (FILE* file, const VarNames& names) const = 0;
  virtual IdealWriter* createWriter
    (FILE* file, const TermTranslator* translator) const = 0;

  virtual bool hasMoreInput(Scanner& scanner) const;

  virtual const char* getFormatName() const = 0;

  // Returns null if name is unknown.
  static IOHandler* getIOHandler(const string& name);

 protected:
  void readTerm(BigIdeal& ideal, Scanner& scanner);
  void readVarPower(vector<mpz_class>& term,
					const VarNames& names, Scanner& scanner);
};

#include "BigTermConsumer.h"

class IdealWriter : public TermConsumer, public BigTermConsumer {
 public:
  IdealWriter();
  IdealWriter(FILE* file, const VarNames& names);
  IdealWriter(FILE* file, const TermTranslator* translator, bool includeVar);
  virtual ~IdealWriter();

  virtual void consume(const vector<mpz_class>& term) = 0;
  virtual void consume(const Term& term) = 0;

  virtual void writeJustATerm(const Term& term);

  // TODO: implement everywhere and make pure virtual
  virtual void consume(const Term& term, TermTranslator* translator) {
	if (_translator == 0)
	  _translator = translator;
	ASSERT(_translator == translator); // TODO: this is bad
	consume(term);
  }

  // TODO: implement everywhere and make pure virtual
  virtual void consume(mpz_ptr* term) {
	vector<mpz_class> v(_names.getVarCount());
	for (size_t var = 0; var < _names.getVarCount(); ++var) {
	  v[var] = mpz_class(term[var]);
	}
  }

 protected:
  static void writeTerm(const vector<const char*>& term, FILE* file);

  static void writeTerm(const Term& term,
			const TermTranslator* translator,
			FILE* file);

  static void writeTerm(const vector<mpz_class>& term,
			const VarNames& names,
			FILE* file);

  FILE* _file;
  VarNames _names;
  const TermTranslator* _translator;
};

void readFrobeniusInstance(Scanner& scanner, vector<mpz_class>& numbers);

#endif
