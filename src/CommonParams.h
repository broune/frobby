/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2009 University of Aarhus
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
#ifndef COMMON_PARAMS_GUARD
#define COMMON_PARAMS_GUARD

#include <string>

class CliParams;

class CommonParams {
 public:
  CommonParams();

  /** Returns whether the input ideal is known to be minimally
      generated. This option is off by default.

      If the ideal is minimally generated, setting this option can
      improve performance. However, if this option is set and the ideal
      is not actually minimally generated, then Frobby will exhibit
      undefined behavior. Possibilities for what might happen include
      crashing, going into an infinite loop and producing incorrect
      results. */
  bool getIdealIsMinimal() const {return _idealIsMinimal;}
  void idealIsMinimal(bool value) {_idealIsMinimal = value;}

  /** Returns whether to print and time the large-scale actions
      that Frobby performs. */
  bool getPrintActions() const {return _printActions;}
  void printActions(bool value) {_printActions = value;}

  /** Returns the format used for parsing input. */
  const string& getInputFormat() const {return _inputFormat;}
  void setInputFormat(const string& value) {_inputFormat = value;}

  /** Returns the format in which output is produced. */
  const string& getOutputFormat() const {return _outputFormat;}
  void setOutputFormat(const string& value) {_outputFormat = value;}

  /** Returns whether to produce output in a canonical
      representation. */
  bool getProduceCanonicalOutput() const {return _produceCanonicalOutput;}
  void produceCanonicalOutput(bool value) {_produceCanonicalOutput = value;}

  /** Returns whether to print information about what the algorithm
      is doing to standard error as it runs. */
  bool getPrintDebug() const {return _printDebug;}
  void printDebug(bool value) {_printDebug = value;}

  /** Returns whether to print statistics on what the algorithm did
      to standard error after it has run. */
  bool getPrintStatistics() const {return _printStatistics;}
  void printStatistics(bool value) {_printStatistics = value;}

 private:
  bool _idealIsMinimal;
  bool _printActions;
  bool _produceCanonicalOutput;
  bool _printDebug;
  bool _printStatistics;

  string _inputFormat;
  string _outputFormat;
};

void addCommonParams(CliParams& params);
void addDebugParam(CliParams& params);

void extractCliValues(CommonParams& common, const CliParams& cli);

#endif
