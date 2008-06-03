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
#include "stdinc.h"
#include "CoCoA4IOHandler.h"

#include "Scanner.h"
#include "BigIdeal.h"
#include "VarNames.h"
#include "CoefTermConsumer.h"
#include "Term.h"
#include "TermTranslator.h"

CoCoA4IOHandler::CoCoA4IOHandler():
  IOHandler("cocoa4", "Format understandable by the program CoCoA 4.", false) {
  registerInput(MonomialIdeal);
  registerInput(MonomialIdealList);
  registerOutput(MonomialIdeal);
  registerOutput(Polynomial);
}

void CoCoA4IOHandler::writeIdealHeader(const VarNames& names, FILE* out) {
  if (names.getVarCount() ==  0) {
	fputs("Use R ::= Q[x];\nNames := [];\nI := Ideal(", out);
	return;
  }

  fprintf(out, "Use R ::= Q[x[1..%lu]];\n",
		  (unsigned long)names.getVarCount());
  
  fputs("Names := [", out);

  const char* pre = "\"";
  for (unsigned int i = 0; i < names.getVarCount(); ++i) {
	fputs(pre, out);
	fputs(names.getName(i).c_str(), out);
	pre = "\", \"";
  }
  fputs("\"];\nI := Ideal(", out);
}

void CoCoA4IOHandler::writeTermOfIdeal(const Term& term,
									   const TermTranslator* translator,
									   bool isFirst,
									   FILE* out) {
  fputs(isFirst ? "\n " : ",\n ", out);

  bool seenNonZero = false;
  size_t varCount = term.getVarCount();
  for (size_t var = 0; var < varCount; ++var) {
	const char* exp = translator->getExponentString(var, term[var]);
	if (exp == 0)
	  continue;
	seenNonZero = true;

	fprintf(out, "x[%lu]", (unsigned long)var);
	if (exp[0] != '1' || exp[1] != '\0') {
	  fputc('^', out);
	  fputs(exp, out);
	}
  }

  if (!seenNonZero)
	fputc('1', out);
}

void CoCoA4IOHandler::writeTermOfIdeal(const vector<mpz_class> term,
									   const VarNames& names,
									   bool isFirst,
									   FILE* out) {
  fputs(isFirst ? "\n " : ",\n ", out);

  bool seenNonZero = false;
  size_t varCount = term.size();
  for (size_t var = 0; var < varCount; ++var) {
	if (term[var] == 0)
	  continue;
	seenNonZero = true;

	fprintf(out, "x[%lu]", (unsigned long)(var + 1));
	if (term[var] != 1) {
	  fputc('^', out);
	  mpz_out_str(out, 10, term[var].get_mpz_t());
	}
  }

  if (!seenNonZero)
	fputc('1', out);
}

void CoCoA4IOHandler::writeIdealFooter(const VarNames& names,
									   FILE* out) {
  fputs("\n);\n", out);  
}

void CoCoA4IOHandler::readIdeal(Scanner& scanner, BigIdeal& ideal) {
  readVarsAndClearIdeal(ideal, scanner);

  size_t varCount = ideal.getVarCount();

  scanner.expect('I');
  scanner.expect(":=");
  scanner.expect("Ideal");
  scanner.expect('(');

  if (!scanner.match(')')) {
	do {
	  ideal.newLastTerm();

	  if (scanner.match('1'))
		continue;

	  vector<mpz_class>& term = ideal.getLastTermRef();

	  scanner.expect('x');
	  do {
		scanner.expect('[');

		size_t var;
		scanner.readSizeT(var);
		if (var > varCount) {
		  fprintf(stderr, "ERROR: No variable x[%lu].\n", (unsigned long)var);
		  exit(1);
		}
		--var;

		scanner.expect(']');

		if (term[var] != 0) {
		  scanner.printError();
		  fputs("A variable appears twice.\n", stderr);
		  exit(1);
		}

		if (scanner.match('^')) {
		  scanner.readInteger(term[var]);
		  if (term[var] <= 0) {
			scanner.printError();
			gmp_fprintf
			  (stderr, "Expected positive integer as exponent but got %Zd.\n",
			   term[var].get_mpz_t());
			exit(1);
		  }
		} else
		  term[var] = 1;
	  } while (scanner.match('x'));
	} while (scanner.match(','));
	scanner.expect(')');
  }
  scanner.match(';');
  scanner.expectEOF();
}

void CoCoA4IOHandler::readVarsAndClearIdeal(BigIdeal& ideal,
											Scanner& scanner) {
  scanner.expect("Use");
  scanner.expect('R');
  scanner.expect("::=");
  scanner.expect('Q');
  scanner.expect('[');
  scanner.expect('x');

  size_t varCount = 0;
  if (scanner.match('[')) {
	scanner.expect('1');
	scanner.expect("..");
	scanner.readSizeT(varCount);
	scanner.expect(']');
  }
  scanner.expect(']');
  scanner.expect(';');

  scanner.expect("Names");
  scanner.expect(":=");
  scanner.expect('[');

  VarNames names;
  for (size_t var = 0; var < varCount; ++var) {
	scanner.expect('\"');
	if (scanner.peekWhite()) {
	  fputs("ERROR: Variable name contains white-space.", stderr);
	  exit(1);
	}

	const char* varName = scanner.readIdentifier();
	if (names.contains(varName)) {
	  scanner.printError();
		fprintf(stderr, "The variable %s is declared twice.\n", varName);
		exit(1);
	}
	names.addVar(varName);

	if (scanner.peekWhite()) {
	  fputs("ERROR: Variable name contains white-space.", stderr);
	  exit(1);
	}
	scanner.expect('\"');
	if (var < varCount - 1)
	  scanner.expect(',');
  }

  scanner.expect(']');
  scanner.expect(';');

  ideal.clearAndSetNames(names);
}

// TODO: integrate this better and move some of it elsewhere
#include "TermTranslator.h"
#include "Term.h"
class CoCoA4CoefTermWriter : public CoefTermConsumer {
 public:
  CoCoA4CoefTermWriter(FILE* file, const TermTranslator* translator):
	_file(file),
	_translator(translator),
	_justStartedWriting(true) {
	const VarNames& names = _translator->getNames();

	fprintf(_file, "Use R ::= Q[x[1..%lu]];",
			(unsigned long)names.getVarCount());

	fputs("Names = [", _file);
    const char* pre = "";
    for (unsigned int i = 0; i < names.getVarCount(); ++i) {
      fputs(pre, _file);
      fputs(names.getName(i).c_str(), _file);
      pre = ", ";
    }
    fputs("];\n", _file);
    fputs("p :=\n", _file);
  }

  virtual ~CoCoA4CoefTermWriter() {
	if (_justStartedWriting)
	  fputc('0', _file);
    fputs(";\n", _file);
  };

  virtual void consume(const mpz_class& coef, const Term& term) {
	fputc(' ', _file);
    if (_justStartedWriting)
      _justStartedWriting = false;
	else if (coef >= 0)
	  fputc('+', _file);

	bool needsSeperator = true;
	if (coef == 1) {
	  if (term.isIdentity()) {
		fputc('1', _file);
		return;
	  }
	  needsSeperator = false;
	}
	if (coef != 1) {
	  if (coef == -1) {
		fputc('-', _file);
		if (term.isIdentity()) {
		  fputc('1', _file);
		  return;
		}
		needsSeperator = false;
	  }
	  else
		gmp_fprintf(_file, "%Zd", coef.get_mpz_t());
	}

	size_t varCount = term.getVarCount();
	for (size_t j = 0; j < varCount; ++j) {
	  const char* exp = _translator->getVarExponentString(j, term[j]);
	  if (exp == 0)
		continue;

	  if (needsSeperator)
		putc('*', _file);
	  else
		needsSeperator = true;

	  fputs(exp, _file);
	}
	fputc('\n', _file);
  }

private:
  FILE* _file;
  const TermTranslator* _translator;
  bool _justStartedWriting;
};

CoefTermConsumer* CoCoA4IOHandler::createCoefTermWriter
(FILE* file, const TermTranslator* translator) {
  return new CoCoA4CoefTermWriter(file, translator);
}
