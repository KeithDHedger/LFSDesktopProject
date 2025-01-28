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
Copy the sanple prefs file "sample_prefs.rc to "~/.config/LFS/tray.rc"

```console
lfstray
Or for options:
lfstray -h
Or for instance:
lfstray --below -i 96 --gravity=4 --monitor 1
```
