This will be a simple dock.  
Far from finished yet, but usable.  
Much more to come!.  

Getting functionality working first, fancy graphics later.  

Gadgets:  
In the gadets list in ~/.config/LFS/lfsdock.rc and the prefs dialog, you can use:  
L - Any .desktop files in ~/.config/LFS/launchers-DOCK.  
C - Clock.  
c - Calendar.  
D - Desktop switcher.  
s - Space.  
T - Task list.  
  
N.B.  
The tasklist needs to be included for the task switcher but the tasks are always on the right side of the dock.  
Right click on a launcher for a simple context menu.  
Clicking a launcher will either launch the app or if already launched bring it to front.  
Selecting 'Launch' from the right click menu will attemt to launch an new instance, ( some single instance apps will override this ).  
To add a launcher either copy a .desktop file to  ~/.config/LFS/launchers-DOCK or drag and drop one on the dock itself.  
Right clicking a task window from the pop up list will move that window to the current desktop.  
  
You can use 's' for spaces as many times as you want, you can also ( optionally ) add a multiplier between 1-9 after the 's' ( eg s5 ) for multiple spaces.  
  
You can add date reminders to the calendar gadget via a simple text file in '~/.config/LFS/calendardates', a default file is created on 1st run if it doesn't exist.  
The format is:  
month date colour  
where month is between 0 - 11 ( 0=January, 11=December ), date is 1- however many days in the month, colour is 0-7 and is a hard coded colour ( this may change later ), 0 is used for todays date.  
You can open the config file with the your default editor by clicking the "Set Dates" buton, dates will be refreshed after selecting "<<<" or ">>>" or toggling calandar.  
  
The calendar gadget is a toggle and so need to be clicked on to make the appropriate window appear/disappear.  
  
Gadgets can be in any order you wish.  
  
If you set a backround image, it will be tiled over the window  
PNG's should be used if you want to use end caps  
End caps can be optionally set and should be in the same folder and be the same format as the main tile image with the filenames being for instance:  
/home/keithhedger/Backgrounds/rage.png << main tile, only this path needs be set in the config file ( please use the lfsdockprefs app ).  
/home/keithhedger/Backgrounds/rage-ls.png << end cap.  
End caps are flipped according left/right and up/down to suit the dock gravity, only one end cap is needed ( should be for the left of a SOUTH gravity dock ).
Images can be (semi-)transparent.  
Some examples are included in /usr/share/LFSDock/pixmaps  
  
More gadgets to come and improvements to the graphics.  
  
