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
#ifndef MAIN_GUARD
#define MAIN_GUARD

// This function plays the role that main() usually would.
int frobbyMain(int argc, const char** argv);

// This wraps around frobbyMain and adds debug capabilities when DEBUG
// is defined.
int main(int argc, const char** argv);

// The exit codes below are returned from frobbyMain and main. The
// only part of these codes that can be counted on in future versions
// of Frobby is that a zero exit code means success, while a non-zero
// exit code indicates some kind of error.

// Everything went well
static const int ExitCodeSuccess = 0;

// A normal error occured, such as a syntax error.
static const int ExitCodeError = 1;

// An error that should be impossible to trigger occured. This
// indicates a bug in Frobby.
static const int ExitCodeInternalError = 2;

// Frobby ran out of memory.
static const int ExitCodeOutOfMemory = 3;

// Frobby encountered an exception that it does not know about.
static const int ExitCodeUnknownError = 4;

#endif
