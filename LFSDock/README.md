This will be a simple dock.<br>
Far from finished yet, but usable.<br>
Much more to come!.<br>

Getting functionality working first, fancy graphics later.<br>

Gadgets:<br>
In the gadets list in ~/.config/LFS/lfsdock.rc and the prefs dialog, you can use:<br>
L - Any .desktop files in ~/.config/LFS/launchers-DOCK.<br>
C - Clock.<br>
c - Calendar.<br>
S - Alsa volume slider.<br>
D - Desktop switcher.<br>
s - Space.<br>
T - Task list.<br>
<br>
N.B.<br>
The tasklist needs to be included for the task switcher but the tasks are always on the right side of the dock.<br>
Right click on a launcher for a simple context menu.<br>
Clicking a launcher will either launch the app or if already launched bring it to front.<br>
Selecting 'Launch' from the right click menu will attemt to launch an new instance, ( some single instance apps will override this ).<br>
To add a launcher either copy a .desktop file to  ~/.config/LFS/launchers-DOCK or drag and drop one on the dock itself.<br>
Right clicking a task window from the pop up list will move that window to the current desktop.<br>
<br>
You can use 's' for spaces as many times as you want.<br>
<br>
You can add date reminders to the calendar gadget via a simple text file in '~/.config/LFS/calendardates', a default file is created on 1st run if it doesn't exist.<br>
The format is:<br>
month date colour<br>
where month is between 0 - 11 ( 0=January, 11=December ), date is 1- however many days in the month, colour is 0-7 and is a hard coded colour ( this may change later ), 0 is used for todays date.<br>
<br>
The calendar and slider gadgets are toggles and so need to be clicked on to make the appropriate window appear/disappear.<br>
Gadgets can be in any order you wish.<br>
<br>
If you set a backround image, it will be tiled over the window<br>
<PNG's should be used if you want to use end caps<br>
End caps can be optionally set and fshould be in the same folder and be the same format as the main tile image with the filenames being for instance:<br>
/home/keithhedger/Backgrounds/rage.png << main tile, only this path needs be set in the config file ( plaease use the lfsdockprefs app ).<br>
/home/keithhedger/Backgrounds/rage-ls.png << left end cap.<br>
/home/keithhedger/Backgrounds/rage-rs.png <<right end cap.<br>
Images can be (semi-)transparent.<br>
The main tile can be any size but the end caps height shoud be at least 120, the width can be any size>=1.<br>
A couple of examples are included in /usr/share/LFSDock/pixmaps<br>
<br>
More gadgets to come and improvements to the graphics.<br>
The dock looks best with a transparent background at the bottom.<br>
<br>
