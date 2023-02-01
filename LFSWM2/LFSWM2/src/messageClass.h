/*
 *
 * ©K. D. Hedger. Tue 31 Jan 12:13:53 GMT 2023 keithdhedger@gmail.com

 * This file (messageClass.h) is part of LFSWM2.

 * LFSWM2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * LFSWM2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSWM2.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _MESSAGECLASS_
#define _MESSAGECLASS_

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <signal.h>

#include "lfswm2Class.h"

class LFSWM2_messageClass
{

	public:

	LFSWM2_messageClass(LFSWM2_Class *mainClass);
	~LFSWM2_messageClass(void);

//functions
	bool					LFSWM2_readMsg(void);

//vars
		LFSWM2_Class		*mainClass;

		int				queueID=-1;
		int				key=777;
		int				delay=2;
//messages
		int				whatMsg=-1;

	private:

};

#endif
