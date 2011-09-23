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
#include "display.h"

#include <cctype>

namespace {
  /** Automatically break lines at this width. */
  static const size_t ConsoleWidth = 79;

  /** Helper class for display(). */
  class Printer {
  public:
    Printer(const string& msg, const string& prepend):
      _pos(0), _lineSize(0), _msg(msg), _prefix(prepend) {

	  string wordSpacePrefix;

      while (_pos < _msg.size()) {
        // We are always at the start of a line at this point.
		ASSERT(_lineSize == 0);
        readIndentation();
		printRaw(_prefix);
		printRaw(_indentation);

        if (_pos == _msg.size())
          break;
        if (_msg[_pos] == '\n') {
          newLine();
          ++_pos;
          continue;
        }

		wordSpacePrefix.clear();
        while (_pos < _msg.size()) {
          if (_msg[_pos] == '\n') {
            ++_pos;
            break;
          }
          if (isspace(_msg[_pos])) {
			wordSpacePrefix += _msg[_pos];
			++_pos;
            continue;
          }
          ASSERT(!isspace(_msg[_pos]));
          ASSERT(_msg[_pos] != '\n');
          ASSERT(_pos < _msg.size());

          string word;
          while (_pos < _msg.size() &&
				 _msg[_pos] != '\n' &&
				 !isspace(_msg[_pos])) {
            word += _msg[_pos];
            ++_pos;
          }
          ASSERT(!word.empty());
          printWord(wordSpacePrefix, word);
		  wordSpacePrefix.clear();
        }

		newLine();
      }
    }

  private:
    void newLine() {
      printRaw('\n');
      _lineSize = 0;
    }

    void readIndentation() {
      // Read whitespace at beginning of line.
      _indentation.clear();
      while (_pos < _msg.size() && _msg[_pos] != '\n' && isspace(_msg[_pos])) {
        _indentation += _msg[_pos];
        ++_pos;
      }
    }

    void printWord(const string& wordSpacePrefix, const string& word) {
      ASSERT(!word.empty());

      // Note that this will print beyond the console width if word is
      // the first thing we are printing on this line. That is because
      // there then is no way to fit the word on one line.
	  size_t wordAndPrefixSize = word.size() + wordSpacePrefix.size();
	  if (_lineSize != 0 && _lineSize + wordAndPrefixSize  > ConsoleWidth) {
		// we skip space before word if inserting newline
		newLine();
		printRaw(_prefix);
		printRaw(_indentation);
	  } else
		printRaw(wordSpacePrefix);
      printRaw(word);
    }

    void printRaw(const string& word) {
      fputs(word.c_str(), stderr);
      _lineSize += word.size();
    }

    void printRaw(char c) {
      fputc(c, stderr);
      ++_lineSize;
    }

    size_t _pos;
    size_t _lineSize;
    const string& _msg;
    const string& _prefix;
    string _indentation;
  };
}

void display(const string& msg, const string& prepend) {
  Printer(msg, prepend);
}

void displayNote(const string& msg) {
  display("NOTE: " + msg + "\n");
}

void displayError(const string& msg) {
  display("ERROR: " + msg + "\n");
}

void displayInternalError(const string& msg) {
  display("INTERNAL ERROR: " + msg + "\n");
}

void displayException(const std::exception& exception) {
  try {
    display(exception.what());
  } catch (...) {
    fputs("\n\n*** Error while printing error! ***\n", stderr);
    fflush(stderr);
    fputs("*** Retrying display of error using simpler display method. ***\n",
          stderr);
    fflush(stderr);
    fputs(exception.what(), stderr);
    fflush(stderr);
    throw;
  }
}
