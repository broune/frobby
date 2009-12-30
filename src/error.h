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
#ifndef ERROR_GUARD
#define ERROR_GUARD

#include <stdexcept>
#include <string>

class Scanner;

/** This is the base of the Frobby exception hierarchy for exceptions
 that can occur due to expected error conditions. */
class FrobbyException : public std::runtime_error {
 public:
  FrobbyException(const string& str): runtime_error(str) {}
};

/** This exception signals that a bug in Frobby has been detected. */
class InternalFrobbyException : public std::logic_error {
 public:
  InternalFrobbyException(const string& str): logic_error(str) {}
};

#define INTERNAL_ERROR(msg) \
  reportInternalError(msg, __FILE__, __LINE__)

// These methods throw exceptions.
void reportError(const string& errorMsg);
void reportInternalError(const string& errorMsg);
void reportInternalError
(const string& errorMsg, const char* file, unsigned int lineNumber);
void reportSyntaxError(const Scanner& scanner, const string& errorMsg);

template<class Exception>
void throwError(const string& errorMsg) {
  throw Exception("ERROR: " + errorMsg + '\n');
}

// These methods return normally.
void displayNote(const string& msg);
void displayDebugNote(const string& msg);
void reportErrorNoThrow(const char* errorMsg);
void reportErrorNoThrow(const string& errorMsg);
void reportInternalErrorNoThrow(const string& errorMsg);
void reportErrorNoThrow(const FrobbyException& e);
void reportErrorNoThrow(const InternalFrobbyException& e);

class UnknownFormatException : public FrobbyException {
 public:
  UnknownFormatException(const string& str): FrobbyException(str) {}
};

#endif
