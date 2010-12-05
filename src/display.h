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
#ifndef DISPLAY_GUARD
#define DISPLAY_GUARD

/** @file display.h

 This file contains functions for printing strings to standard
 error. They all perform automatic line breaking suitable for printing
 to a console.
*/

/** Display msg to standard error with automatic line breaking. If a
 automatically broken line begins with whitespace, that whitespace is
 repeated in front of every line that is generated from breaking
 it.

 @param prepend Print this in front of every line that is printed.
*/
void display(const string& msg, const string& prepend = "");

/** Display msg to standard error in a way that indicates that this is
 something that the user should take note of but that is not an
 error. */
void displayNote(const string& msg);

/** Display msg to standard error in a way that indicates that it is
 an error. */
void displayError(const string& msg);

/** Display msg to standard in a way that indicates that it is an
 internal error. */
void displayInternalError(const string& errorMsg);

/** Display the message of exception. */
void displayException(const std::exception& exception);

#endif
