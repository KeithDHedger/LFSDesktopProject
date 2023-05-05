/*
 *
 * ©K. D. Hedger. Tue 31 Jul 13:21:56 BST 2018 keithdhedger@gmail.com

 * This file (lfshelper.cpp) is part of LFSDesktop.

 * LFSDesktop is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSDesktop is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSDesktop.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


enum {MOUNTDISK=1,UNMOUNTDISK,EJECTDISK};
int checkForNTFS(const char *arg)
{
	char	*command;
	int		ret;

	asprintf(&command,"lsblk -n --fs -l|sed -n '/%s/p'|grep ntfs &>/dev/null",arg);
	ret=system(command);
	free(command);
	return(ret);
}

int main(int argc, char **argv)
{
	char	*command;
	uid_t	user=getuid(); 
	int		isNTFS=1;
	int		what;
	int		retval=0;
	int		exitstatus=-666;

	if(argc<4)
		{
			printf("Usage:\nlfshelper UUID MOUNTPOINT [1=Mount|2=Unmount|3=Eject]\n");
			return(1);
		}
	if(argc==4)
		{
			isNTFS=checkForNTFS(argv[1]);
			setresuid(0,user,user);
			asprintf(&command,"mkdir -p %s &>/dev/null",argv[2]);
			system(command);
			free(command);
			what=atoi(argv[3]);
			switch(what)
				{
					case MOUNTDISK:
						if(isNTFS==0)
							asprintf(&command,"mount -o user,uid=%i,gid=%i UUID=\"%s\" \"%s\"",user,user,argv[1],argv[2]);
						else
							asprintf(&command,"mount UUID=\"%s\" \"%s\"",argv[1],argv[2]);
						break;
					case UNMOUNTDISK:
						asprintf(&command,"umount UUID=\"%s\"",argv[1]);
						exitstatus=system(command);
						free(command);			
						asprintf(&command,"rmdir \"%s\"",argv[2]);
						break;
					case EJECTDISK:
						asprintf(&command,"eject UUID=\"%s\" &",argv[1]);
						exitstatus=system(command);
						free(command);			
						asprintf(&command,"rmdir \"%s\"",argv[2]);
						break;
				}
			retval=system(command);
			free(command);			
		}
	if(exitstatus!=-666)
		return(WEXITSTATUS(exitstatus));
	else
		return(WEXITSTATUS(retval));
}