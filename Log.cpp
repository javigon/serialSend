/*
 * authors Philippe Bonnet & Javier Gonzalez
 *
 * Copyright 2011 Philippe Bonnet & Javier Gonzalez
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */

#include "Log.h"
#include <cstring>
#include <iostream>
#include <ctime>
#include <fstream>
#include <sstream>


/*************************** CONSTRUCTOR ***************************/
Log::Log()
{
}

// actual constructor
Log::Log(std::string primary_logdir_path)
{
	// generate file names
	std::stringstream primary_path_stream(std::ios::out);
	primary_path_stream << primary_logdir_path << "/log.txt";
	primary_path = primary_path_stream.str();
}

// copy constructor
Log::Log(const Log& log)
{
	primary_path = log.getPrimaryPath();
}

/* assignment operator */
Log& Log::operator= (const Log& log)
{
  if (this == &log) return *this;   // Gracefully handle self assignment

  // Put the normal assignment duties here...
  primary_path = log.getPrimaryPath();

  return *this;
}

Log::~Log()
{
}

/************************************************************************/

std::string Log::getPrimaryPath() const
{
	return primary_path;
}


/************************************************************************/


// Best effort - Written once (either primary or nop)
void Log::mesg(std::string mesg)
{
	// get timestamp string from current time
	char ts[100];
	time_t cnow;

	if ( time(&cnow) == -1)
	{
	  // problem getting current time stamp
	  // proceed with storage using a dummy timestamp: Oscar's birthdate
	  strcpy(ts, "2007October16-14_00");
	} else {
	  // generate time stamp from current time in a format YearMonthnameDayHourMinute
	  struct tm* now = gmtime (&cnow);
	  if (strftime(ts, 30, "%Y%B%d-%H:%M:%S", now) == 0)
	  	return;
    }

	// open primary file name (append mode)
	std::fstream file_primary(primary_path.c_str(), std::ios::out|std::ios::app|std::ios::ate);
	if (!file_primary.is_open())
	{
		return; //nop
	}
	else
	{
		file_primary << ts << mesg << std::endl;
		file_primary.flush();
		file_primary.close();
	}

	return;
}

