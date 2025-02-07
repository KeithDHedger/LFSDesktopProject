# LFSTray
This ia a simple standalone systray.  
###
**To build/install:**
```console
./autogen.sh --prefix=/usr
make
make install
```

You dont need to install to try it out just run lfstray from the app folder.
###
**QUICK USE:**  
Copy the sanple prefs file sample_prefs.rc to ~/.config/LFS/lfstray.rc

```console
lfstray
Or for options:
lfstray -h
Usage:
LFSTray [ARG 0] ... [ARG N]
-m, --monitor INTEGER ARG        
        Place on monitor ARG
-i, --iconsize INTEGER ARG       
        Iconsize ARG
-g, --gravity INTEGER ARG        
        Gravity NW ARG=1,NE ARG=2,SE ARG=3,SW ARG=4,N ARG=5,E ARG=6,S ARG=7,W ARG=8
-V, --vertical   
        Vertical systray ( default horizontal )
-b, --below      
        Below all windows ( default above )
-f, --filepath STRING ARG        
        Use external file

Or for instance:
lfstray --below -i 96 --gravity=4 --monitor 1
```
