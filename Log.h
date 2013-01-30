/*
 * author Ph.Bonnet
 *
 * Copyright 2008 Ph.Bonnet and M.Chang
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


#ifndef Log_class
#define Log_class

#include <iostream>


/*
 * There is one file path per Log directory - named log.txt
 * Each log method opens - write - close the file.
 * Best effort - written once approach: try first primary, then secondary, then nop.
 */
class Log
{
	public:
		Log ();
		Log(std::string);
		Log(const Log&);
		~Log();

		Log& operator= (const Log&);

		std::string getPrimaryPath(void) const;

		void mesg(std::string);



	private:
		std::string primary_path;
};


#endif
