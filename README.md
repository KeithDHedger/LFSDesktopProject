**KKEdit Source Code Editor.**<br>
![KKEdit](screenshots/main.jpg?raw=true "KKEdit")<br>
<br>
[Full details and screenshots:](http://kkedit.freeddns.org)<br>
<br>
Why another text editor?<br>
<br>
Simple, I didn't like any of the others, anyway having used gedit for years it was getting increasingly difficult to keep it compiling without loads of gnome dependencies, looking around linux text editors seem to be VERY simple (leafpad) stupidly complex (GVim,Bluefish), trying to be all things to all people.<br>
One of the best text editors I have used is BBEdit but that is only available on the mac, so I bit the bullet and decided to write my own with the best bits from BBEdit ( the name KKEdit is a tip of the hat to BBEdit ), gedit and leafpad. A simple uncluttered interface, no major dependencies, with just the features I use all the time, and TA DAH! KKEdit.<br>
<br>
Some of the features:<br>
Jump to function declaration.<br>
Find Function declaration.<br>
Find and open include file.<br>
Multiple bookmarks.<br>
Run external tools.<br>
Save/Restore session.<br>
Run external tool synchronously or asynchronously.<br>
Pass selected text to external tools.<br>
<br>
Find API declarations in installed Gtk-Doc's.<br>
<br>
Full source code highlighted printing<br>
<br>
+ The usual source view options, line wrap, line numbers etc.<br>
<br>
Ported to gtk3.<br>
<br>
Add --enable-gtk3 to your configure to build for gtk3.<br>
<br>
Default is to build for gtk2<br>
You may need to install gtk3 dev files if your distro splits them off from the main package ( debian etc ).<br>
<br>
*bsd* compile abandoned for the moment as webkit is broken on freeports, can be used with --disable-docviewer.<br>
<br>
+ When building from a fesh clone for the first time, please use ./autogen.sh instead of ./configure, args passed to autogen.sh are the same for configure, this only needs to be done on a fresh clone<br>
