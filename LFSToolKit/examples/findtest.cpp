#if 0

#©keithhedger Thu 20 Sep 13:34:49 BST 2018 kdhedger68713@gmail.com

g++ -Wall "$0"||exit 1
./a.out "$@"
retval=$?
rm ./a.out
exit $retval

#endif

#define _XOPEN_SOURCE 500
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

const char *name;

int display_info(const char *fpath, const struct stat *sb,int tflag, struct FTW *ftwbuf)
{
	if(tflag == FTW_F)
		{
			if(strcasecmp(fpath+ftwbuf->base,name)==0)
				//printf("%s -- %s -- %i -- %i\n",fpath,fpath + ftwbuf->base,ftwbuf->base,ftwbuf->level);
				printf("%s -- %s\n",fpath,fpath + ftwbuf->base);
		
		}
 //   printf("%-3s %2d %7jd   %-40s %d %s\n",
//        (tflag == FTW_D) ?   "d"   : (tflag == FTW_DNR) ? "dnr" :
//        (tflag == FTW_DP) ?  "dp"  : (tflag == FTW_F) ?   "f" :
//        (tflag == FTW_NS) ?  "ns"  : (tflag == FTW_SL) ?  "sl" :
//        (tflag == FTW_SLN) ? "sln" : "???",
//        ftwbuf->level, (intmax_t) sb->st_size,
//        fpath, ftwbuf->base, fpath + ftwbuf->base);
    return 0;           /* To tell nftw() to continue */
}

int main(int argc, char *argv[])
{
    int flags = 0;

//	char	*data="int main(int argc, char *argv[])";
//	printf(">>%s<<\n",data+4);
//	return 0;
 //   if (argc > 2 && strchr(argv[2], 'd') != NULL)
//        flags |= FTW_DEPTH;
//    if (argc > 2 && strchr(argv[2], 'p') != NULL)
//        flags |= FTW_PHYS;

	name="string";
	flags=FTW_PHYS;//|FTW_D;
	nftw(argv[1],display_info,20,flags);
		
//    if (nftw((argc < 2) ? "." : argv[1], display_info, 20, flags)
//            == -1) {
//        perror("nftw");
//        exit(EXIT_FAILURE);
//    }
    exit(EXIT_SUCCESS);
}