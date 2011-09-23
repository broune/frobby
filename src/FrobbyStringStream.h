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
#ifndef FROBBY_STRING_STREAM_GUARD
#define FROBBY_STRING_STREAM_GUARD

#include <string>
#include <stdexcept>

/** A replacement for stringstream. See the .cpp file for a motivation
 to use this instead of the regular stringstream. It should not be
 used for operations that need to be efficient. */
class FrobbyStringStream {
 public:
  FrobbyStringStream& operator<<(unsigned long integer);
  FrobbyStringStream& operator<<(unsigned int integer);
  FrobbyStringStream& operator<<(const mpz_class& integer);
  FrobbyStringStream& operator<<(const string& text);
  FrobbyStringStream& operator<<(const char* text);

  /** This overload actually appends the character to the stream
   instead of appending a string representation of the number. This
   is different from stringstream. */
  FrobbyStringStream& operator<<(char character);

  string& str();
  const string& str() const;
  operator const string&() const;

  void clear() {_str.clear();}

  static void appendIntegerToString(string& str, unsigned long integer);
  static void appendIntegerToString(string& str, const mpz_class& integer);

  /** Throws NotAnIntegerException if str is not the string
   representation of an integer. */
  static void parseInteger(mpz_class& integer, const string& str);
  class NotAnIntegerException : public runtime_error {
  public:
    NotAnIntegerException(const string&);
  };

 private:
  string _str;
};

#endif
