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
#include "error.h"

#include "Scanner.h"
#include "FrobbyStringStream.h"

void reportError(const string& errorMsg) {
  FrobbyStringStream err;
  err << "ERROR: " << errorMsg << '\n';
  throw FrobbyException(err);
}

void reportInternalError(const string& errorMsg) {
  FrobbyStringStream err;
  err << "INTERNAL ERROR: " << errorMsg << '\n';
  throw InternalFrobbyException(err);
}

void reportSyntaxError(const Scanner& scanner, const string& errorMsg) {
  FrobbyStringStream err;
  err << "SYNTAX ERROR (";

  if (scanner.getFormat() != "")
	err << "format " << scanner.getFormat() << ", ";

  err << "line "
	  << scanner.getLineNumber()
	  << "):\n  "
	  << errorMsg
	  << '\n';

  throw FrobbyException(err);
}

void displayNote(const string& msg) {
  fprintf(stderr, "NOTE: %s\n", msg.c_str());
}

void displayDebugNote(const string& msg) {
  fprintf(stderr, "DEBUG: %s\n", msg.c_str());
}

void reportErrorNoThrow(const string& errorMsg) {
  reportErrorNoThrow(errorMsg.c_str());
}

void reportErrorNoThrow(const char* errorMsg) {
  fprintf(stderr, "ERROR: %s\n", errorMsg);
}

void reportErrorNoThrow(const FrobbyException& e) {
  fputs(e.what(), stderr);
}

void reportErrorNoThrow(const InternalFrobbyException& e) {
  fputs(e.what(), stderr);
}
