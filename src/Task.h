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
#ifndef TASK_GUARD
#define TASK_GUARD

class TaskEngine;

/** A Task object represents a unit of work that is performed when the
 method run() is called. Tasks are intended to be used with a
 TaskEngine.
*/
class Task {
 public:
  virtual ~Task();

  /** Does whatever work this task represents. The parameter can be
   used to schedule additional tasks.
  */
  virtual void run(TaskEngine& engine) = 0;

  /** Called when the task is no longer used but run has not and will
   not be called. This can happen from a destructor being called due
   to an exception, so dispose must not throw an exception under any
   circumstances.
  */
  virtual void dispose() = 0;
};

#endif
