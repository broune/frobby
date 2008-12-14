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

class Scanner;

class FrobbyException {
};

// These methods throw exceptions.
void reportError(const string& errorMsg);
void reportInternalError(const string& errorMsg);
void reportSyntaxError(const Scanner& scanner, const string& errorMsg);

// These methods return normally.
void displayNote(const string& msg);
void reportErrorNoThrow(const string& errorMsg);


#endif
