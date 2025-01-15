/*
 *
 * ©K. D. Hedger. Mon 13 Jan 16:23:18 GMT 2025 keithdhedger@gmail.com

 * This file (settings.c) is part of LFSTray.

 * LFSTray is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation,either version 3 of the License,or
 * (at your option) any later version.

 * LFSTray is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSTray.  If not,see <http://www.gnu.org/licenses/>.
*/

/*
 Forked from stalonetray ( big thanks )
 Original Code here:
 https://github.com/kolbusa/stalonetray
 * settings.c
 * Sun,12 Sep 2004 18:55:53 +0700
 * -------------------------------
 * settings parser\container
 * -------------------------------*/

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <libgen.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "config.h"
#include "common.h"
#include "layout.h"
#include "list.h"
#include "settings.h"
#include "tray.h"
#include "wmh.h"
#include "xutils.h"

/* Here we keep our filthy settings */
struct Settings settings;
/* Initialize data */
void init_default_settings()
{
	settings.bg_color_str="gray";
	settings.tint_color_str="white";
	settings.display_str=NULL;
	settings.geometry_str=NULL;
	settings.max_geometry_str="0x0";
	settings.icon_size=FALLBACK_ICON_SIZE;
	settings.slot_size.x=-1;
	settings.slot_size.y=-1;
	settings.max_tray_dims.x=0;
	settings.max_tray_dims.y=0;
	settings.parent_bg=0;
	settings.shrink_back_mode=1;
	settings.sticky=1;
	settings.skip_taskbar=1;
	settings.transparent=0;
	settings.vertical=0;
	settings.grow_gravity=GRAV_N | GRAV_W;
	settings.icon_gravity=GRAV_N | GRAV_W;
	settings.wnd_layer=NULL;
	settings.wnd_name=PROGNAME;
	settings.xsync=0;
	settings.need_help=0;
	settings.config_fname=NULL;
	settings.full_pmt_search=1;
	settings.min_space_policy=0;
	settings.pixmap_bg=0;
	settings.bg_pmap_path=NULL;
	settings.tint_level=0;
	settings.wm_strut_mode=WM_STRUT_AUTO;
	settings.kludge_flags=0;
	settings.ignored_classes=NULL;
}

/* ******* general parsing utils ********* */

#define PARSING_ERROR(msg,str) fprintf(stderr,"Parsing error: %s \"%s\" found\n",msg,str);

/* Parse list of ignored window classes */
int parse_ignored_classes(int argc,const char **argv,void **references,int silent)
{
	struct WindowClass **classes=(struct WindowClass **) references[0];
	struct WindowClass *newclass=NULL;
	int i;

	for(i=0; i<argc; i++)
		{
			newclass=malloc(sizeof(struct WindowClass));
			newclass->name=strdup(argv[i]);
			LIST_ADD_ITEM(*classes,newclass);
		}
	return SUCCESS;
}

/* Parse gravity string ORing resulting value
 * with current value of tgt */
int parse_gravity(int argc,const char **argv,void **references,int silent)
{
	int *gravity=(int *) references[0];
	const char *gravity_s=argv[0];
	int parsed=0;

	if(strlen(gravity_s) > 2)
		goto fail;

	for(; *gravity_s; gravity_s++)
		{
			switch (tolower(*gravity_s))
				{
				case 'n':
					parsed |= GRAV_N;
					break;
				case 's':
					parsed |= GRAV_S;
					break;
				case 'w':
					parsed |= GRAV_W;
					break;
				case 'e':
					parsed |= GRAV_E;
					break;
				default:
					goto fail;
				}
		}

	if((parsed & GRAV_N && parsed & GRAV_S) || (parsed & GRAV_E && parsed & GRAV_W))
		goto fail;

	*gravity=parsed;

	return SUCCESS;

fail:
	PARSING_ERROR("gravity expected",gravity_s);
	return FAILURE;
}

/* Parse integer string storing resulting value in tgt */
int parse_int(int argc,const char **argv,void **references,int silent)
{
	int *parsed=(int *) references[0];
	char *invalid;

	*parsed=strtol(argv[0],&invalid,0);

	if(*invalid!='\0')
		{
			PARSING_ERROR("integer expected",argv[0]);
			return FAILURE;
		}

	return SUCCESS;
}

/* Parse kludges mode */
int parse_kludges(int argc,const char **argv,void **references,int silent)
{
	const char *token=strtok((char *) argv[0],",");
	int *kludges=(int *) references[0];

	for(; token!=NULL; token=strtok(NULL,","))
		{
			if(!strcasecmp(token,"fix_window_pos"))
				*kludges |= KLUDGE_FIX_WND_POS;
			else if(!strcasecmp(token,"force_icons_size"))
				*kludges |= KLUDGE_FORCE_ICONS_SIZE;
			else if(!strcasecmp(token,"use_icons_hints"))
				*kludges |= KLUDGE_USE_ICONS_HINTS;
			else
				{
					PARSING_ERROR("kludge flag expected",token);
					return FAILURE;
				}
		}
	return SUCCESS;
}

/* Parse strut mode */
int parse_strut_mode(int argc,const char **argv,void **references,int silent)
{
	int *strut_mode=(int *) references[0];

	if(!strcasecmp(argv[0],"auto"))
		*strut_mode=WM_STRUT_AUTO;
	else if(!strcasecmp(argv[0],"top"))
		*strut_mode=WM_STRUT_TOP;
	else if(!strcasecmp(argv[0],"bottom"))
		*strut_mode=WM_STRUT_BOT;
	else if(!strcasecmp(argv[0],"left"))
		*strut_mode=WM_STRUT_LFT;
	else if(!strcasecmp(argv[0],"right"))
		*strut_mode=WM_STRUT_RHT;
	else if(!strcasecmp(argv[0],"none"))
		*strut_mode=WM_STRUT_NONE;
	else
		{
			PARSING_ERROR(
			    "one of top,bottom,left,right,or auto expected",argv[0]);
			return FAILURE;
		}
	return SUCCESS;
}

/* Parse boolean string storing result in tgt*/
int parse_bool(int argc,const char **argv,void **references,int silent)
{
	const char *true_str[]= {"yes","on","true","1",NULL};
	const char *false_str[]= {"no","off","false","0",NULL};
	int *boolean=(int *) references[0];

	for(const char **s=true_str; *s; s++)
		{
			if(!strcasecmp(argv[0],*s))
				{
					*boolean=True;
					return SUCCESS;
				}
		}

	for(const char **s=false_str; *s; s++)
		{
			if(!strcasecmp(argv[0],*s))
				{
					*boolean=False;
					return SUCCESS;
				}
		}

	PARSING_ERROR("boolean expected",argv[0]);
	return FAILURE;
}

/* Backwards version of the boolean parser */
int parse_bool_rev(int argc,const char **argv,void **references,int silent)
{
	int *boolean=(int *) references[0];

	if(parse_bool(argc,argv,references,silent))
		{
			*boolean=! *boolean;
			return SUCCESS;
		}

	return FAILURE;
}

/* Parse window layer string storing result in tgt */
int parse_wnd_layer(int argc,const char **argv,void **references,int silent)
{
	char **window_layer=(char **) references[0];

	if(!strcasecmp(argv[0],"top"))
		*window_layer=_NET_WM_STATE_ABOVE;
	else if(!strcasecmp(argv[0],"bottom"))
		*window_layer=_NET_WM_STATE_BELOW;
	else if(!strcasecmp(argv[0],"normal"))
		*window_layer=NULL;
	else
		{
			PARSING_ERROR("window layer expected",argv[0]);
			return FAILURE;
		}

	return SUCCESS;
}

/* Just copy string from arg to *tgt */
int parse_copystr(int argc,const char **argv,void **references,int silent)
{
	const char **stringref=(const char **) references[0];
	/* Valgrind note: this memory will never be freed before lfstray's exit. */
	if((*stringref=strdup(argv[0]))==NULL)
		DIE_OOM(("Could not copy value from parameter\n"));

	return SUCCESS;
}

int parse_pos(int argc,const char **argv,void **references,int silent)
{
	struct Point *pos=(struct Point *) references[0];
	unsigned int dummy;
	XParseGeometry(argv[0],&pos->x,&pos->y,&dummy,&dummy);
	return SUCCESS;
}

int parse_size(int argc,const char **argv,void **references,int silent)
{
	struct Point *size=(struct Point *) references[0];
	unsigned int width,height;
	int bitmask,dummy;

	bitmask=XParseGeometry(argv[0],&dummy,&dummy,&width,&height);

	if(bitmask==0 || bitmask & ~(WidthValue | HeightValue))
		return FAILURE;
	if((bitmask & HeightValue)==0)
		height=width;

	val_range(width,0,INT_MAX);
	val_range(height,0,INT_MAX);

	size->x=width;
	size->y=height;

	return SUCCESS;
}

/************ CLI **************/

#define MAX_TARGETS 10
#define MAX_DEFAULT_ARGS 10

/* parameter parser function */
typedef int (*param_parser_t)(int argc,const char **argv,void **references,int silent);

struct Param
{
	const char *short_name;        /* short command line parameter name */
	const char *long_name;         /* long command line parameter name */
	const char *rc_name;           /* parameter name for config file */
	void *references[MAX_TARGETS]; /* array of references necessary when parsing */

	const int pass; /* 0th pass parameters are parsed before rc file,*/
	/* 1st pass parameters are parsed after it */

	const int min_argc; /* minimum number of expected arguments */
	const int max_argc; /* maximum number of expected arguments,0 for unlimited */

	const int default_argc;                     /* number of default arguments,if present */
	const char *default_argv[MAX_DEFAULT_ARGS]; /* default arguments if none are given */

	param_parser_t parser;  /* pointer to parsing function */
};

struct Param params[]=
{
#ifdef XPM_SUPPORTED
	{
		.short_name=NULL,
		.long_name="--pixmap-bg",
		.rc_name="pixmap_bg",
		.references={ (void *) &settings.bg_pmap_path },

		.pass=0,

		.min_argc=1,
		.max_argc=1,

		.default_argc=0,
		.default_argv=NULL,

		.parser=(param_parser_t) &parse_copystr
	},
#endif
	{
		.short_name="-display",
		.long_name=NULL,
		.rc_name="display",
		.references={ (void *) &settings.display_str },

		.pass=1,

		.min_argc=1,
		.max_argc=1,

		.default_argc=0,
		.default_argv=NULL,

		.parser=(param_parser_t) &parse_copystr
	},
	{
		.short_name="-bg",
		.long_name="--background",
		.rc_name="background",
		.references={ (void *) &settings.bg_color_str },

		.pass=1,

		.min_argc=1,
		.max_argc=1,

		.default_argc=0,
		.default_argv=NULL,

		.parser=(param_parser_t) &parse_copystr
	},
	{
		.short_name="-c",
		.long_name="--config",
		.rc_name=NULL,
		.references={ (void *) &settings.config_fname },

		.pass=0,

		.min_argc=1,
		.max_argc=1,

		.default_argc=0,
		.default_argv=NULL,

		.parser=(param_parser_t) &parse_copystr
	},
	{
		.short_name="-geometry",
		.long_name="--geometry",
		.rc_name="geometry",
		.references={ (void *) &settings.geometry_str },

		.pass=1,

		.min_argc=1,
		.max_argc=1,

		.default_argc=0,
		.default_argv=NULL,

		.parser=(param_parser_t) &parse_copystr
	},
	{
		.short_name=NULL,
		.long_name="--grow-gravity",
		.rc_name="grow_gravity",
		.references={ (void *) &settings.grow_gravity },

		.pass=1,

		.min_argc=1,
		.max_argc=1,

		.default_argc=0,
		.default_argv=NULL,

		.parser=(param_parser_t) &parse_gravity
	},
	{
		.short_name=NULL,
		.long_name="--icon-gravity",
		.rc_name="icon_gravity",
		.references={ (void *) &settings.icon_gravity },

		.pass=1,

		.min_argc=1,
		.max_argc=1,

		.default_argc=0,
		.default_argv=NULL,

		.parser=(param_parser_t) &parse_gravity
	},
	{
		.short_name="-i",
		.long_name="--icon-size",
		.rc_name="icon_size",
		.references={ (void *) &settings.icon_size },

		.pass=1,

		.min_argc=1,
		.max_argc=1,

		.default_argc=0,
		.default_argv=NULL,

		.parser=(param_parser_t) &parse_int
	},
	{
		.short_name="-h",
		.long_name="--help",
		.rc_name=NULL,
		.references={ (void *) &settings.need_help },

		.pass=0,

		.min_argc=0,
		.max_argc=0,

		.default_argc=1,
		.default_argv={"true"},

		.parser=(param_parser_t) &parse_bool
	},
	{
		.short_name=NULL,
		.long_name="--kludges",
		.rc_name="kludges",
		.references={ (void *) &settings.kludge_flags },

		.pass=1,

		.min_argc=1,
		.max_argc=1,

		.default_argc=0,
		.default_argv=NULL,

		.parser=(param_parser_t) &parse_kludges
	},
	{
		.short_name=NULL,
		.long_name="--max-geometry",
		.rc_name="max_geometry",
		.references={ (void *) &settings.max_geometry_str },

		.pass=1,

		.min_argc=1,
		.max_argc=1,

		.default_argc=0,
		.default_argv=NULL,

		.parser=(param_parser_t) &parse_copystr
	},
	{
		.short_name=NULL,
		.long_name="--no-shrink",
		.rc_name="no_shrink",
		.references={ (void *) &settings.shrink_back_mode },

		.pass=1,

		.min_argc=0,
		.max_argc=1,

		.default_argc=1,
		.default_argv={"true"},

		.parser=(param_parser_t) &parse_bool_rev
	},
	{
		.short_name="-p",
		.long_name="--parent-bg",
		.rc_name="parent_bg",
		.references={ (void *) &settings.parent_bg },

		.pass=1,

		.min_argc=0,
		.max_argc=1,

		.default_argc=1,
		.default_argv={"true"},

		.parser=(param_parser_t) &parse_bool
	},
	{
		.short_name=NULL,
		.long_name="--skip-taskbar",
		.rc_name="skip_taskbar",
		.references={ (void *) &settings.skip_taskbar },

		.pass=1,

		.min_argc=0,
		.max_argc=1,

		.default_argc=1,
		.default_argv={"true"},

		.parser=(param_parser_t) &parse_bool
	},
	{
		.short_name="-s",
		.long_name="--slot-size",
		.rc_name="slot_size",
		.references={ (void *) &settings.slot_size },

		.pass=1,

		.min_argc=1,
		.max_argc=1,

		.default_argc=0,
		.default_argv=NULL,

		.parser=(param_parser_t) &parse_size
	},
	{
		.short_name=NULL,
		.long_name="--sticky",
		.rc_name="sticky",
		.references={ (void *) &settings.sticky },

		.pass=1,

		.min_argc=0,
		.max_argc=1,

		.default_argc=1,
		.default_argv={"true"},

		.parser=(param_parser_t) &parse_bool
	},
	{
		.short_name=NULL,
		.long_name="--tint-color",
		.rc_name="tint_color",
		.references={ (void *) &settings.tint_color_str },

		.pass=1,

		.min_argc=1,
		.max_argc=1,

		.default_argc=0,
		.default_argv=NULL,

		.parser=(param_parser_t) &parse_copystr
	},
	{
		.short_name=NULL,
		.long_name="--tint-level",
		.rc_name="tint_level",
		.references={ (void *) &settings.tint_level },

		.pass=1,

		.min_argc=1,
		.max_argc=1,

		.default_argc=0,
		.default_argv=NULL,

		.parser=(param_parser_t) &parse_int
	},
	{
		.short_name="-t",
		.long_name="--transparent",
		.rc_name="transparent",
		.references={ (void *) &settings.transparent },

		.pass=1,

		.min_argc=0,
		.max_argc=1,

		.default_argc=1,
		.default_argv={"true"},

		.parser=(param_parser_t) &parse_bool
	},
	{
		.short_name="-v",
		.long_name="--vertical",
		.rc_name="vertical",
		.references={ (void *) &settings.vertical },

		.pass=1,

		.min_argc=0,
		.max_argc=1,

		.default_argc=1,
		.default_argv={"true"},

		.parser=(param_parser_t) &parse_bool
	},
	{
		.short_name=NULL,
		.long_name="--window-layer",
		.rc_name="window_layer",
		.references={ (void *) &settings.wnd_layer },

		.pass=1,

		.min_argc=1,
		.max_argc=1,

		.default_argc=0,
		.default_argv=NULL,

		.parser=(param_parser_t) &parse_wnd_layer
	},
	{
		.short_name=NULL,
		.long_name="--window-name",
		.rc_name="window_name",
		.references={ (void *) &settings.wnd_name },

		.pass=1,

		.min_argc=1,
		.max_argc=1,

		.default_argc=0,
		.default_argv=NULL,

		.parser=(param_parser_t) &parse_copystr
	},
	{
		.short_name=NULL,
		.long_name="--window-strut",
		.rc_name="window_strut",
		.references={ (void *) &settings.wm_strut_mode },

		.pass=1,

		.min_argc=1,
		.max_argc=1,

		.default_argc=0,
		.default_argv=NULL,

		.parser=(param_parser_t) &parse_strut_mode
	},
	{
		.short_name=NULL,
		.long_name="--xsync",
		.rc_name="xsync",
		.references={ (void *) &settings.xsync },

		.pass=1,

		.min_argc=0,
		.max_argc=1,

		.default_argc=1,
		.default_argv={"true"},

		.parser=(param_parser_t) &parse_bool
	},
	{
		.short_name=NULL,
		.long_name=NULL,
		.rc_name="ignore_classes",
		.references={ (void *) &settings.ignored_classes },

		.pass=1,

		.min_argc=1,
		.max_argc=0,

		.default_argc=0,
		.default_argv=NULL,

		.parser=(param_parser_t) &parse_ignored_classes
	},
};

void usage(char *progname)
{
	printf("\nlfstray " VERSION "\n");
	printf("\nUsage: %s [options...]\n",progname);
	printf(
	    "\n"
	    "For short options argument can be specified as -o value or -ovalue.\n"
	    "For long options argument can be specified as --option value or --option=value.\nAll flag-options have expilicit optional boolean argument,which can be true (1,yes) or false (0,no).\n"
	    "\n"
	    "Possible options are:\n"
	    "    -display <display>          use X display <display>\n"
	    "    -bg,--background <color>   select background color (default: #777777)\n"
	    "    -c,--config <filename>     read configuration from <file> (instead of default $HOME/.config/LFS/lfstrayrc)\n"
		"    --dockapp-mode [<mode>]     enable dockapp mode; mode can be none (default),\n simple (default if no mode specified),or wmaker\n"
	    "    [-]-geometry <geometry>     set initial tray`s geometry (width "
	    "and height\n"
	    "                                are defined in icon slots; offsets "
	    "are defined\n"
	    "                                in pixels)\n"
	    "    --grow-gravity <gravity>    set tray`s grow gravity,\n"
	    "                                either to N,S,W,E,NW,NE,SW,or "
	    "SE\n"
	    "    --icon-gravity <gravity>    icon positioning gravity (NW,NE,"
	    "SW,SE)\n"
	    "    -i,--icon-size <n>         set basic icon size to <n>; default "
	    "is 24\n"
	    "   --ignore-classes <classes>   ignore tray icons in xembed if the "
	    "tray window has one of the given classes,separated by commas\n"
	    "    -h,--help                  show this message\n"

	    "    --kludges <list>            enable specific kludges to work "
	    "around\n"
	    "                                non-conforming WMs and/or "
	    "lfstray bugs;\n"
	    "                                argument is a comma-separated list "
	    "of:\n"
	    "                                 - fix_window_pos (fix window "
	    "position),\n"
	    "                                 - force_icons_size (ignore icon "
	    "resizes),\n"
	    "                                 - use_icons_hints (use icon size "
	    "hints)\n"
	    "    --max-geometry <geometry>   set tray maximal width and height; 0 "
	    "indicates\n"
	    "                                no limit in respective direction\n"
	    "    --no-shrink                 do not shrink window back after icon removal\n"
	    "    -p,--parent-bg             use parent for background\n"
	    "    --pixmap-bg <pixmap>        use pixmap for tray`s window background\n"
	    "    --slot-size <w>[x<h>]       set icon slot size in pixels\n"
	    "                                if omitted,hight is set equal to width\n"
	    "    --skip-taskbar              hide tray`s window from the taskbar\n"
	    "    --sticky                    make tray`s window sticky across multiple desktops/pages\n"
	    "    -t,--transparent           enable root transparency\n"
	    "    --tint-color <color>        tint tray background with color (not used  with plain color background)\n"
	    "    --tint-level <level>        set tinting level from 0 to 255\n"
	    "    -v,--vertical              use vertical layout of icons (horizontal layout is used by default)\n"
	    "    --window-layer <layer>      set tray`s window EWMH layer either to bottom,normal,or top\n"
	    "    --window-strut <mode>       set window strut mode to either "
	    "auto,\n"
	    "                                left,right,top,or bottom\n"
	    "desktop\n"
	    "    --xsync                     operate on X server synchronously "
	    "(SLOW)\n"
	    "\n");
}

/* Parse command line parameters */
int parse_cmdline(int argc,char **argv,int pass)
{
	struct Param *p,*match;
	char *arg,*progname=argv[0];
	const char **p_argv=NULL,*argbuf[MAX_DEFAULT_ARGS];
	int p_argc;

	while(--argc > 0)
		{
			argv++;
			match=NULL;
			for(p=params; p->parser!=NULL; p++)
				{
					if(p->max_argc)
						{
							if(p->short_name!=NULL && strstr(*argv,p->short_name)==*argv)
								{
									if((*argv)[strlen(p->short_name)]!='\0')   /* accept arguments in the form -a5 */
										{
											argbuf[0]=*argv + strlen(p->short_name);
											p_argc=1;
											p_argv=argbuf;
										}
									else if(argc > 1 && argv[1][0]!='-')     /* accept arguments in the form -a 5,do not accept values starting with '-' */
										{
											argbuf[0]=*(++argv);
											p_argc=1;
											p_argv=argbuf;
											argc--;
										}
									else if(p->min_argc > 0)     /* argument is missing */
										{
											//  LOG_ERROR(("%s expects an argument\n",p->short_name));
											break;
										}
									else     /* argument is optional,use default value */
										{
											p_argc=p->default_argc;
											p_argv=p->default_argv;
										}
								}
							else if(p->long_name!=NULL && strstr(*argv,p->long_name)==*argv)
								{
									if((*argv)[strlen(p->long_name)]=='=')  /* accept arguments in the form --abcd=5 */
										{
											argbuf[0]=*argv + strlen(p->long_name) + 1;
											p_argc=1;
											p_argv=argbuf;
										}
									else if((*argv)[strlen(p->long_name)]=='\0')     /* accept arguments in the from --abcd 5 */
										{
											if(argc > 1 && argv[1][0]!='-')   /* arguments cannot start with the dash */
												{
													argbuf[0]=*(++argv);
													p_argc=1;
													p_argv=argbuf;
													argc--;
												}
											else if(p->min_argc > 0)     /* argument is missing */
												{
													break;
												}
											else /* argument is optional,use default value */
												{
													p_argc=p->default_argc;
													p_argv=p->default_argv;
												}
										}
									else
										continue; /* just in case when there can be both --abc and --abcd */
								}
							else
								continue;
							match=p;
							break;
						}
					else if(strcmp(*argv,p->short_name)==0 || strcmp(*argv,p->long_name)==0)
						{
							match=p;
							p_argc=p->default_argc;
							p_argv=p->default_argv;
							break;
						}
				}

#define USAGE_AND_DIE() \
    do { usage(progname);fprintf(stderr,"Could not parse command line\n");exit(-1); } while(0)

			if(match==NULL) USAGE_AND_DIE();
			if(match->pass!=pass) continue;
			if(p_argv==NULL)
				{
					fprintf(stderr,"Argument cannot be NULL!\n");
					exit(-1);
				}

			if(!match->parser(p_argc,p_argv,match->references,match->min_argc==0))
				{
					if(match->min_argc==0)
						{
							assert(p_argv!=match->default_argv);
							match->parser(match->default_argc,match->default_argv,match->references,False);
							argc++;
							argv--;
						}
					else
						USAGE_AND_DIE();
				}
		}

	if(settings.need_help)
		{
			usage(progname);
			exit(0);
		}

	return SUCCESS;
}

/************ .lfstrayrc ************/

/**************************************************************************************
 * <line> ::= [<whitespaces>] [(<arg> <whitespaces>)* <arg>] [<whitespaces>]
 *<comment> <arg> ::= "<arbitrary-text>"|<text-without-spaces-and-#> <comment>
 *::= # <arbitrary-text>
 **************************************************************************************/

/* Does exactly what its name says */
#define SKIP_SPACES(p) \
    { \
        for(; *p!=0 && isspace((int)*p); p++) \
            ; \
    }
/* Break the line in argc,argv pair */
int get_args(char *line,int *argc,char ***argv)
{
	int q_flag=0;
	char *arg_start,*q_pos;
	*argc=0;
	*argv=NULL;
	/* 1. Strip leading spaces */
	SKIP_SPACES(line);
	if(0==*line)   /* meaningless line */
		{
			return SUCCESS;
		}
	arg_start=line;
	/* 2. Strip comments */
	for(; 0!=*line; line++)
		{
			q_flag=('"'==*line) ? !q_flag : q_flag;
			if('#'==*line && !q_flag)
				{
					*line=0;
					break;
				}
		}
	if(q_flag)   /* disbalance of quotes */
		{
			//   LOG_ERROR(("Disbalance of quotes\n"));
			return FAILURE;
		}
	if(arg_start==line)   /* meaningless line */
		{
			return SUCCESS;
		}
	line--;
	/* 3. Strip trailing spaces */
	for(; line!=arg_start && isspace((int)*line); line--)
		;
	if(arg_start==line)   /* meaningless line */
		{
			return FAILURE;
		}
	*(line + 1)=0; /* this _is_ really ok since isspace(0)!=0 */
	line=arg_start;
	/* 4. Extract arguments */
	do
		{
			(*argc)++;
			/* Add space to store one more argument */
			if(NULL==(*argv=realloc(*argv,*argc * sizeof(char *))))
				DIE_OOM(("Could not allocate memory to parse parameters\n"));
			if(*arg_start=='"')   /* 4.1. argument is quoted: find matching quote */
				{
					arg_start++;
					(*argv)[*argc - 1]=arg_start;
					if(NULL==(q_pos=strchr(arg_start,'"')))
						{
							free(*argv);
							fprintf(stderr,"Quotes balance calculation failed\n");
							exit(-1);
							return FAILURE;
						}
					arg_start=q_pos;
				}
			else     /* 4.2. whitespace-separated argument: find fist whitespace */
				{
					(*argv)[*argc - 1]=arg_start;
					for(; 0!=*arg_start && !isspace((int)*arg_start); arg_start++)
						;
				}
			if(*arg_start!=0)
				{
					*arg_start=0;
					arg_start++;
					SKIP_SPACES(arg_start);
				}
		}
	while(*arg_start!=0);
	return SUCCESS;
}

char *find_rc(const char *path_part1,const char *path_part2,const char *rc)
{
	static char full_path[PATH_MAX];
	int len;
	struct stat statbuf;

	if(path_part1==NULL) return NULL;
	if(path_part2==NULL)
		{
			len=snprintf(full_path,sizeof(full_path),"%s/%s",path_part1,rc);
		}
	else
		{
			len=snprintf(full_path,sizeof(full_path),"%s/%s/%s",path_part1,path_part2,rc);
		}

	if(len<0 || len >= sizeof(full_path)) return NULL;
	if(stat(full_path,&statbuf)!=0) return NULL;

	return full_path;
}

#define READ_BUF_SZ 512
/* Parses rc file (path is either taken from settings.config_fname
 * or ~/.lfstrayrc is used) */
void parse_rc()
{
	char *home_dir;
	FILE *cfg;

	char buf[READ_BUF_SZ + 1];
	int lnum=0;

	int argc,p_argc;
	char **argv;
	const char **p_argv;

	struct Param *p,*match;

	/* 1. Setup file name */
	if(settings.config_fname==NULL)
		settings.config_fname=find_rc(getenv("HOME"),NULL,LFSTRAY_RC);//TODO//
		
	if(settings.config_fname==NULL)
		return;

	/* 2. Open file */
	cfg=fopen(settings.config_fname,"r");
	if(cfg==NULL)
		return;

	/* 3. Read the file line by line */
	buf[READ_BUF_SZ]=0;
	while(!feof(cfg))
		{
			lnum++;
			if(fgets(buf,READ_BUF_SZ,cfg)==NULL)
				break;

			if(!get_args(buf,&argc,&argv))
				{
					fprintf(stderr,"Configuration file parse error at %s:%d: could not parse line\n",settings.config_fname,lnum);
					exit(-1);
				}
			if(!argc) continue; /* This is empty/comment-only line */

			match=NULL;
			p_argc=argc - 1;
			p_argv=(const char **) argv + 1;

			for(p=params; p->parser!=NULL; p++)
				{
					if(p->rc_name!=NULL && strcmp(argv[0],p->rc_name)==0)
						{
							if(argc - 1<p->min_argc || (p->max_argc && argc - 1 > p->max_argc))
								{
									fprintf(stderr,"Configuration file parse error at %s:%d: invalid number of args for \"%s\" (%d-%d required)\n",settings.config_fname,lnum,p->rc_name,p->min_argc,p->max_argc);
									exit(-1);
								}

							match=p;

							if(p_argc==0)
								{
									p_argc=match->default_argc;
									p_argv=match->default_argv;
								}

							break;
						}
				}

			if(!match)
				{
					fprintf(stderr,"Configuration file parse error at %s:%d: unrecognized rc file keyword \"%s\".\n",settings.config_fname,lnum,argv[0]);
					exit(-1);
				}

			assert(p_argv!=NULL);

			if(!match->parser(p_argc,p_argv,match->references,False))
				{
					fprintf(stderr,"Configuration file parse error at %s:%d: could not parse argument for \"%s\".\n",settings.config_fname,lnum,argv[0]);
					exit(-1);
				}

			free(argv);
		}

	fclose(cfg);
}

/* Interpret all settings that need an open display or other settings */
void interpret_settings()
{
	static int gravity_matrix[11]= {ForgetGravity,EastGravity,WestGravity,ForgetGravity,SouthGravity,SouthEastGravity,SouthWestGravity,ForgetGravity,NorthGravity,NorthEastGravity,NorthWestGravity};
	int geom_flags;
	int rc;
	int dummy;
	XWindowAttributes root_wa;
	/* Sanitize icon size */
	val_range(settings.icon_size,MIN_ICON_SIZE,INT_MAX);
	if(settings.slot_size.x<settings.icon_size)
		settings.slot_size.x=settings.icon_size;
	if(settings.slot_size.y<settings.icon_size)
		settings.slot_size.y=settings.icon_size;

	/* Sanitize all gravity strings */
	settings.icon_gravity |= ((settings.icon_gravity & GRAV_V) ? 0 : GRAV_N);
	settings.icon_gravity |= ((settings.icon_gravity & GRAV_H) ? 0 : GRAV_W);
	settings.win_gravity=gravity_matrix[settings.grow_gravity];
	settings.bit_gravity=gravity_matrix[settings.icon_gravity];
	/* Parse all background-related settings */
#ifdef XPM_SUPPORTED
	settings.pixmap_bg=(settings.bg_pmap_path!=NULL);
#endif
	if(settings.pixmap_bg)
		{
			settings.parent_bg=False;
			settings.transparent=False;
		}
	if(settings.transparent) settings.parent_bg=False;
	/* Parse color-related settings */
	if(!x11_parse_color(tray_data.dpy,settings.bg_color_str,&settings.bg_color))
		{
			fprintf(stderr,"Could not parse background color \"%s\"\n",settings.bg_color_str);
			exit(-1);
		}

	/* Sanitize tint level value */
	val_range(settings.tint_level,0,255);
	if(settings.tint_level)
		{
			/* Parse tint color */
			if(!x11_parse_color(tray_data.dpy,settings.tint_color_str,&settings.tint_color))
				{
					fprintf(stderr,"Could not parse tint color \"%s\"\n",settings.tint_color_str);
					exit(-1);
				}
		}
	/* Get dimensions of root window */
	if(!XGetWindowAttributes(tray_data.dpy,DefaultRootWindow(tray_data.dpy),&root_wa))
		{
			fprintf(stderr,"Could not get root window dimensions.\n");
			exit(-1);
		}
	tray_data.root_wnd.x=0;
	tray_data.root_wnd.y=0;
	tray_data.root_wnd.width=root_wa.width;
	tray_data.root_wnd.height=root_wa.height;
	/* Parse geometry */
#define DEFAULT_GEOMETRY "1x1+0+0"
	tray_data.xsh.flags=PResizeInc | PBaseSize;
	tray_data.xsh.x=0;
	tray_data.xsh.y=0;
	tray_data.xsh.width_inc=settings.slot_size.x;
	tray_data.xsh.height_inc=settings.slot_size.y;
	tray_data.xsh.base_width=0;
	tray_data.xsh.base_height=0;
	tray_calc_window_size(
	    0,0,&tray_data.xsh.base_width,&tray_data.xsh.base_height);
	geom_flags=XWMGeometry(tray_data.dpy,DefaultScreen(tray_data.dpy),settings.geometry_str,DEFAULT_GEOMETRY,0,&tray_data.xsh,&tray_data.xsh.x,&tray_data.xsh.y,&tray_data.xsh.width,&tray_data.xsh.height,&tray_data.xsh.win_gravity);
	tray_data.xsh.win_gravity=settings.win_gravity;
	tray_data.xsh.min_width=tray_data.xsh.width;
	tray_data.xsh.min_height=tray_data.xsh.height;
	tray_data.xsh.max_width=tray_data.xsh.width;
	tray_data.xsh.min_height=tray_data.xsh.height;
	tray_data.xsh.flags=PResizeInc | PBaseSize | PMinSize | PMaxSize | PWinGravity;
	tray_calc_tray_area_size(tray_data.xsh.width,tray_data.xsh.height,&settings.orig_tray_dims.x,&settings.orig_tray_dims.y);
	if(geom_flags & (XValue | YValue))
		tray_data.xsh.flags |= USPosition;
	else
		tray_data.xsh.flags |= PPosition;

	if(geom_flags & (WidthValue | HeightValue))
		tray_data.xsh.flags |= USSize;
	else
		tray_data.xsh.flags |= PSize;

	if((geom_flags & XNegative) && (geom_flags & YNegative))
		settings.geom_gravity=SouthEastGravity;
	else if(geom_flags & YNegative)
		settings.geom_gravity=SouthWestGravity;
	else if(geom_flags & XNegative)
		settings.geom_gravity=NorthEastGravity;
	else
		settings.geom_gravity=NorthWestGravity;
	/* Set tray maximal width/height */
	geom_flags=XParseGeometry(settings.max_geometry_str,&dummy,&dummy,(unsigned int *)&settings.max_tray_dims.x,(unsigned int *)&settings.max_tray_dims.y);
	if(!settings.max_tray_dims.x)
		settings.max_tray_dims.x=root_wa.width;
	else
		{
			settings.max_tray_dims.x *= settings.slot_size.x;
			val_range(settings.max_tray_dims.x,settings.orig_tray_dims.x,INT_MAX);
		}
	if(!settings.max_tray_dims.y)
		settings.max_tray_dims.y=root_wa.height;
	else
		{
			settings.max_tray_dims.y *= settings.slot_size.y;
			val_range(settings.max_tray_dims.y,settings.orig_tray_dims.y,INT_MAX);
		}
}

/************** "main" ***********/
int read_settings(int argc,char **argv)
{
	init_default_settings();
	/* Parse 0th pass command line args */
	parse_cmdline(argc,argv,0);
	/* Parse configuration files */
	parse_rc();
	/* Parse 1st pass command line args */
	parse_cmdline(argc,argv,1);
	return SUCCESS;
}
