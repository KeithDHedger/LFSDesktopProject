/*
 *
 * K.D.Hedger 2013 <kdhedger68713@gmail.com>
 *
*/

#include "kkedit-includes.h"

#define TABLECOLS 2


const char* 	shortcuttext[NUMSHORTCUTS]={"Delete Current Line","Delete To End Of Line","Delete To Beginning Of Line","Select Word Under Cursor","Delete Word Under Cursor","Duplicate Current Line","Select Current Line","Move Current Line Up","Move Current Line Down","Select From Cursor To End Of Line","Select From Beginning Of Line To Cursor","Move Selection Up","Move Selection Down","Show Completion"};

void findTool(toolStruct* data,char* toolname)
{
#ifndef _USEQT5_
	if(strcmp(toolname,data->menuName)==0)
		selectedToolFromDrop=data;
#endif
}

#ifndef _USEQT5_
void selectToolOptions(GtkWidget* widget,gpointer data)
#else
//TODO//
void selectToolOptions(void)
#endif
{
#ifndef _USEQT5_
	char*	text=gtk_combo_box_text_get_active_text((GtkComboBoxText*)widget);
	int		flags=0;

	selectedToolFromDrop=NULL;

	g_list_foreach(toolsList,(GFunc)findTool,text);
	if(selectedToolFromDrop!=NULL)
		{
			gtk_toggle_button_set_active((GtkToggleButton*)inPopupWidget,selectedToolFromDrop->inPopUp);
			gtk_toggle_button_set_active((GtkToggleButton*)alwaysPopupWidget,selectedToolFromDrop->alwaysPopup);
			gtk_toggle_button_set_active((GtkToggleButton*)clearViewWidget,selectedToolFromDrop->clearView);
			gtk_toggle_button_set_active((GtkToggleButton*)runAsRootWidget,selectedToolFromDrop->runAsRoot);
			gtk_toggle_button_set_active((GtkToggleButton*)runAsRootWidget,selectedToolFromDrop->runAsRoot);
			gtk_toggle_button_set_active((GtkToggleButton*)useBarWidget,selectedToolFromDrop->useBar);

			gtk_entry_set_text((GtkEntry*)toolNameWidget,selectedToolFromDrop->menuName);
			gtk_entry_set_text((GtkEntry*)commandLineWidget,selectedToolFromDrop->command);
			if(selectedToolFromDrop->comment!=NULL)
				gtk_entry_set_text((GtkEntry*)commentWidget,selectedToolFromDrop->comment);
			else
				gtk_entry_set_text((GtkEntry*)commentWidget,"");

			if(selectedToolFromDrop->keyCode!=0)
				gtk_entry_set_text((GtkEntry*)keyWidget,gdk_keyval_name(selectedToolFromDrop->keyCode));
			else
				gtk_entry_set_text((GtkEntry*)keyWidget,"");

			flags=selectedToolFromDrop->flags;
			if(flags & TOOL_ASYNC)
				gtk_toggle_button_set_active((GtkToggleButton*)syncWidget,false);
			else
				gtk_toggle_button_set_active((GtkToggleButton*)syncWidget,true);

			if((flags & TOOL_INSERT_MASK)==TOOL_IGNORE_OP)
				gtk_toggle_button_set_active((GtkToggleButton*)ignoreWidget,true);
			if((flags & TOOL_INSERT_MASK)==TOOL_VIEW_OP)
				gtk_toggle_button_set_active((GtkToggleButton*)outputWidget,true);
			if((flags & TOOL_INSERT_MASK)==TOOL_PASTE_OP)
				gtk_toggle_button_set_active((GtkToggleButton*)pasteWidget,true);
			if((flags & TOOL_INSERT_MASK)==TOOL_REPLACE_OP)
				gtk_toggle_button_set_active((GtkToggleButton*)replaceWidget,true);

			if((gtk_toggle_button_get_active((GtkToggleButton*)syncWidget)==false) || (gtk_toggle_button_get_active((GtkToggleButton*)inTermWidget)==true))
				{
					gtk_widget_set_sensitive(ignoreWidget,false);
					gtk_widget_set_sensitive(pasteWidget,false);
					gtk_widget_set_sensitive(replaceWidget,false);
					gtk_widget_set_sensitive(outputWidget,false);
				}
			else
				{
					gtk_widget_set_sensitive(ignoreWidget,true);
					gtk_widget_set_sensitive(pasteWidget,true);
					gtk_widget_set_sensitive(replaceWidget,true);
					gtk_widget_set_sensitive(outputWidget,true);
				}

			if(flags & TOOL_SHOW_DOC)
				gtk_toggle_button_set_active((GtkToggleButton*)showDocWidget,true);
			else
				gtk_toggle_button_set_active((GtkToggleButton*)showDocWidget,false);
		}
#endif
}

void addToolToDrop(gpointer data,gpointer user_data)
{
#ifndef _USEQT5_
	if(((toolStruct*)data)->global==false)
		gtk_combo_box_text_append_text((GtkComboBoxText*)toolSelect,((toolStruct*)data)->menuName);
#endif
}

#ifndef _USEQT5_
void fillCombo(GtkComboBoxText* combo)
#else
//TODO//
void fillCombo(void)
#endif
{
#ifndef _USEQT5_
	g_list_foreach(toolsList,addToolToDrop,NULL);
#endif
}

#ifndef _USEQT5_
gboolean getToolKey(GtkEntry* widget,GdkEventKey* event,gpointer data)
#else
//TODO//
gboolean getToolKey(void)
#endif
{
#ifndef _USEQT5_
	if((event->type==GDK_KEY_PRESS) && (event->keyval==GDK_KEY_Delete))
		{
			gtk_entry_set_text(widget,"");
			return(true);
		}

	if ((event->type==GDK_KEY_PRESS)&& (event->state & GDK_CONTROL_MASK))
		gtk_entry_set_text(widget,gdk_keyval_name(event->keyval));

#endif
	return(true);
}

void doMakeTool(void)
{
#ifndef _USEQT5_
	GtkWidget*	vbox;
	GtkWidget*	hbox;
	GtkWidget*	button;
	GtkWidget*	toolwin;
	GtkWidget*	infolabel;

	char*		placeholderinfo;

	asprintf(&placeholderinfo,"%s","PLACEHOLDERS:\n%t - Currently selected text. Passed to command as $KKEDIT_SELECTION\n%f - Filepath of the current document. Passed to command as $KKEDIT_CURRENTFILE\n%d - Directory of the current document or $HOME. Passed to command as $KKEDIT_CURRENTDIR\n%i - The location of the globally installed tools. Passed to command as $KKEDIT_DATADIR\n%h - Tempory file for displaying html in doc viewer. Passed to command as $KKEDIT_HTMLFILE\n%l - Highlihting language. Passed to command as $KKEDIT_SOURCE_LANG");

	toolwin=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title((GtkWindow*)toolwin,"Edit External Tools");
	vbox=gtk_vbox_new(false,8);

//select tool
	toolSelect=gtk_combo_box_text_new();
	gtk_box_pack_start(GTK_BOX(vbox),toolSelect,false,true,0);
	g_signal_connect(G_OBJECT(toolSelect),"changed",G_CALLBACK(selectToolOptions),NULL);
	fillCombo((GtkComboBoxText*) toolSelect);

//name
	toolNameWidget=gtk_entry_new();
	hbox=gtk_hbox_new(false,0);
	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new("Tool Name:\t"),false,true,0);
	gtk_box_pack_start(GTK_BOX(hbox),toolNameWidget,true,true,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,false,true,0);
	gtk_widget_show(toolNameWidget);
	gtk_entry_set_text((GtkEntry*)toolNameWidget,"New Tool");

//command
	commandLineWidget=gtk_entry_new();
	hbox=gtk_hbox_new(false,0);
	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new("Command: \t"),false,true,0);
	gtk_box_pack_start(GTK_BOX(hbox),commandLineWidget,true,true,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,false,true,0);
	gtk_widget_show(commandLineWidget);
	gtk_entry_set_text((GtkEntry*)commandLineWidget,"");
//key
	keyWidget=gtk_entry_new();
	gtk_widget_show(keyWidget);
	hbox=gtk_hbox_new(false,0);
	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new("Shortcut:\t\t"),false,true,0);
	gtk_box_pack_start(GTK_BOX(hbox),keyWidget,true,true,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,false,true,0);
	gtk_entry_set_text((GtkEntry*)keyWidget,"");
	g_signal_connect(G_OBJECT(keyWidget),"key-press-event",G_CALLBACK(getToolKey),NULL);

//comment
	commentWidget=gtk_entry_new();
	gtk_widget_show(commentWidget);
	hbox=gtk_hbox_new(false,0);
	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new("Comment:  \t"),false,true,0);
	gtk_box_pack_start(GTK_BOX(hbox),commentWidget,true,true,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,false,true,0);
	gtk_entry_set_text((GtkEntry*)commentWidget,"");

//info
	infolabel=gtk_label_new(placeholderinfo);
	gtk_label_set_selectable((GtkLabel*)infolabel,true);
	gtk_box_pack_start(GTK_BOX(vbox),infolabel,false,false,0);
	free(placeholderinfo);
//in terminal
	inTermWidget=gtk_check_button_new_with_label("Run Tool In Terminal");
	gtk_widget_set_name(inTermWidget,"interm");
	gtk_box_pack_start(GTK_BOX(vbox),inTermWidget,false,true,0);
	g_signal_connect(G_OBJECT(inTermWidget),"toggled",G_CALLBACK(setToolOptions),NULL);
//show in popup menu
	inPopupWidget=gtk_check_button_new_with_label("Show Tool In Pop-Up Menu");
	gtk_widget_set_name(inPopupWidget,"inpopup");
	gtk_box_pack_start(GTK_BOX(vbox),inPopupWidget,false,true,0);
	g_signal_connect(G_OBJECT(inPopupWidget),"toggled",G_CALLBACK(setToolOptions),NULL);
//always show in popup
	alwaysPopupWidget=gtk_check_button_new_with_label("Always Show Tool In Pop-Up Menu");
	gtk_widget_set_name(alwaysPopupWidget,"alwayspopup");
	gtk_box_pack_start(GTK_BOX(vbox),alwaysPopupWidget,false,true,0);
	g_signal_connect(G_OBJECT(alwaysPopupWidget),"toggled",G_CALLBACK(setToolOptions),NULL);

//flags
//snch/async
	syncWidget=gtk_check_button_new_with_label("Run Tool Synchronously");
	gtk_widget_set_name(syncWidget,"sync");
	gtk_box_pack_start(GTK_BOX(vbox),syncWidget,false,true,0);
	g_signal_connect(G_OBJECT(syncWidget),"toggled",G_CALLBACK(setToolOptions),NULL);

//flags - show html doc
	showDocWidget=gtk_check_button_new_with_label("Show HTML Doc");
	gtk_widget_set_name(showDocWidget,"showdoc");
	gtk_box_pack_start(GTK_BOX(vbox),showDocWidget,false,true,0);
	g_signal_connect(G_OBJECT(showDocWidget),"toggled",G_CALLBACK(setToolOptions),NULL);

//flags - clear view first
	clearViewWidget=gtk_check_button_new_with_label("Clear Tool Output First");
	gtk_widget_set_name(clearViewWidget,"clearview");
	gtk_box_pack_start(GTK_BOX(vbox),clearViewWidget,false,true,0);
	g_signal_connect(G_OBJECT(clearViewWidget),"toggled",G_CALLBACK(setToolOptions),NULL);

//flags - run as root
	runAsRootWidget=gtk_check_button_new_with_label("Run Tool As Root");
	gtk_widget_set_name(runAsRootWidget,"runasroot");
	gtk_box_pack_start(GTK_BOX(vbox),runAsRootWidget,false,true,0);
	g_signal_connect(G_OBJECT(runAsRootWidget),"toggled",G_CALLBACK(setToolOptions),NULL);

//flags - use progressbar
	useBarWidget=gtk_check_button_new_with_label("Use Progress Bar");
	gtk_widget_set_name(useBarWidget,"usebar");
	gtk_box_pack_start(GTK_BOX(vbox),useBarWidget,false,true,0);
	g_signal_connect(G_OBJECT(useBarWidget),"toggled",G_CALLBACK(setToolOptions),NULL);

//flags - ignore
	ignoreWidget=gtk_radio_button_new_with_label(NULL,"Ignore Output");
	gtk_widget_set_name(ignoreWidget,"ignore");
	gtk_toggle_button_set_active((GtkToggleButton*)ignoreWidget,true);
	gtk_box_pack_start(GTK_BOX(vbox),ignoreWidget,false,true,0);

//flags - paste
	pasteWidget=gtk_radio_button_new_with_label_from_widget((GtkRadioButton*)ignoreWidget,"Paste Output");
	gtk_widget_set_name(pasteWidget,"paste");
	gtk_box_pack_start(GTK_BOX(vbox),pasteWidget,false,true,0);

//flags - replace all
	replaceWidget=gtk_radio_button_new_with_label_from_widget((GtkRadioButton*)ignoreWidget,"Replace All Contents");
	gtk_widget_set_name(replaceWidget,"replace");
	gtk_box_pack_start(GTK_BOX(vbox),replaceWidget,false,true,0);

//flags - view
	outputWidget=gtk_radio_button_new_with_label_from_widget((GtkRadioButton*)ignoreWidget,"View Output");
	gtk_widget_set_name(outputWidget,"outtoview");
	gtk_box_pack_start(GTK_BOX(vbox),outputWidget,false,true,0);

//buttons
	gtk_box_pack_start(GTK_BOX(vbox),gtk_hseparator_new(),false,false,0);

	hbox=gtk_hbox_new(false,0);
	button=gtk_button_new_from_stock(GTK_STOCK_APPLY);
	gtk_box_pack_start(GTK_BOX(hbox),button,true,false,2);
	gtk_widget_set_name(button,"apply");
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(setToolOptions),(void*)toolwin);	

	button=gtk_button_new_from_stock(GTK_STOCK_DELETE);
	gtk_box_pack_start(GTK_BOX(hbox),button,true,false,2);
	gtk_widget_set_name(button,"delete");
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(setToolOptions),(void*)toolwin);	

	button=gtk_button_new_from_stock(GTK_STOCK_CANCEL);
	gtk_box_pack_start(GTK_BOX(hbox),button,true,false,2);
	gtk_widget_set_name(button,"cancel");
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(setToolOptions),(void*)toolwin);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,true,false,2);

//show it
	gtk_container_add(GTK_CONTAINER(toolwin),(GtkWidget*)vbox);
	gtk_widget_show_all(toolwin);
#endif
}
//
//void buildTools(void)
//{
////TODO//
//printf("buildTools\n");
//#ifndef _USEQT5_
//	GtkWidget*		menuitem;
//	GtkWidget*		menu;
//	GtkWidget*		image;
//	GList*			ptr;
//	bool			gotglobal=false;
//	int				keyflags=0;
//
//	buildToolsList();
//
//	GtkWidget* submenu=gtk_menu_item_get_submenu((GtkMenuItem*)toolsMenu);
//	if(submenu!=NULL)
//		gtk_menu_item_set_submenu((GtkMenuItem*)toolsMenu,NULL);
//
//	menu=gtk_menu_new();
//	gtk_menu_item_set_submenu(GTK_MENU_ITEM(toolsMenu),menu);
//
////addtool
//	menuitem=gtk_image_menu_item_new_with_label("Manage External Tools");
//	image=gtk_image_new_from_stock(GTK_STOCK_EDIT,GTK_ICON_SIZE_MENU);
//	gtk_image_menu_item_set_image((GtkImageMenuItem *)menuitem,image);
//	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
//	g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(doMakeTool),NULL);
//
//	menuitem=gtk_separator_menu_item_new();
//	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
//
//	ptr=toolsList;
//	while(ptr!=NULL)
//		{
//			if( ((toolStruct*)ptr->data)->global==true)
//				{
//					gotglobal=true;
//					menuitem=gtk_image_menu_item_new_with_label(((toolStruct*)ptr->data)->menuName);
//					gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
//					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(externalTool),(void*)ptr->data);
//
//					if(((toolStruct*)ptr->data)->keyCode>0)
//						{
//							keyflags=0;
//							if(gdk_keyval_is_upper(((toolStruct*)ptr->data)->keyCode))
//								keyflags=GDK_SHIFT_MASK;
//							gtk_widget_add_accelerator((GtkWidget *)menuitem,"activate",accgroup,((toolStruct*)ptr->data)->keyCode,(GdkModifierType)(GDK_CONTROL_MASK|keyflags),GTK_ACCEL_VISIBLE);
//						}
//
//					if(((toolStruct*)ptr->data)->comment!=NULL)
//						gtk_widget_set_tooltip_text((GtkWidget*)menuitem,((toolStruct*)ptr->data)->comment);
//				}
//			ptr=g_list_next(ptr);
//		}
//
//	if(gotglobal==true)
//		{
//			menuitem=gtk_separator_menu_item_new();
//			gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
//		}
//
//	ptr=toolsList;
//	while(ptr!=NULL)
//		{
//			if( ((toolStruct*)ptr->data)->global==false)
//				{
//					menuitem=gtk_image_menu_item_new_with_label(((toolStruct*)ptr->data)->menuName);
//					gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
//					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(externalTool),(void*)ptr->data);
//
//					if(((toolStruct*)ptr->data)->keyCode>0)
//						{
//							keyflags=0;
//							if(gdk_keyval_is_upper(((toolStruct*)ptr->data)->keyCode))
//								{
//									keyflags=GDK_SHIFT_MASK;
//									if(gdk_keyval_is_lower(((toolStruct*)ptr->data)->keyCode))
//									keyflags=0;
//								}
//							gtk_widget_add_accelerator((GtkWidget *)menuitem,"activate",accgroup,((toolStruct*)ptr->data)->keyCode,(GdkModifierType)(GDK_CONTROL_MASK|keyflags),GTK_ACCEL_VISIBLE);
//						}
//
//					if(((toolStruct*)ptr->data)->comment!=NULL)
//						gtk_widget_set_tooltip_text((GtkWidget*)menuitem,((toolStruct*)ptr->data)->comment);
//				}
//			ptr=g_list_next(ptr);
//		}
//#endif
//}

void doIconView(void)
{
#ifndef _USEQT5_
	GtkCellRenderer *renderer;

	listStore=gtk_list_store_new(3,GDK_TYPE_PIXBUF,G_TYPE_STRING,G_TYPE_INT);
	iconView=(GtkIconView*)gtk_icon_view_new();
	gtk_icon_view_set_pixbuf_column(GTK_ICON_VIEW(iconView),PIXBUF_COLUMN);
	gtk_icon_view_set_model(GTK_ICON_VIEW(iconView),GTK_TREE_MODEL(listStore));
	gtk_icon_view_set_item_padding(GTK_ICON_VIEW(iconView),0);
	
	populateStore();

	gtk_icon_view_set_selection_mode (GTK_ICON_VIEW (iconView),GTK_SELECTION_SINGLE);
	gtk_icon_view_set_orientation (GTK_ICON_VIEW (iconView),GTK_ORIENTATION_HORIZONTAL);
	gtk_icon_view_set_columns (GTK_ICON_VIEW (iconView),24);
	gtk_icon_view_set_reorderable(GTK_ICON_VIEW(iconView),TRUE);

	renderer=gtk_cell_renderer_pixbuf_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(iconView),renderer,false);

	renderer=gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start (GTK_CELL_LAYOUT(iconView),renderer,false);

	gtk_box_pack_start(GTK_BOX(iconViewBox),(GtkWidget*)iconView,false,false,2);
	g_signal_connect(G_OBJECT(iconView),"button-press-event",G_CALLBACK(clickIt),NULL);
#endif
}

#ifndef _USEQT5_
void setKeyCuts(GtkWidget* widget,gpointer data)
#else
//TODO//
void setKeyCuts(void)
#endif
{
#ifndef _USEQT5_
	char*	filename;
	const char*		text;

	if(strcasecmp(gtk_widget_get_name(widget),"cancel")==0)
		gtk_widget_hide(keysWindow);

	if(strcasecmp(gtk_widget_get_name(widget),"apply")==0)
		{
			for(int j=0;j<NUMSHORTCUTS;j++)
				{
					text=gtk_entry_get_text((GtkEntry*)entries[j]);
					shortCuts[j][0]=gdk_keyval_from_name(text);
					shortCuts[j][1]=j;
					if(shortCutStrings[j]!=NULL)
						debugFree(&shortCutStrings[j],"setKeyCuts shortCutStrings");
					asprintf(&shortCutStrings[j],"%i %i - ^%c %s",shortCuts[j][0],shortCuts[j][1],shortCuts[j][0],shortcuttext[j]);
				}
			asprintf(&filename,"%s/" KKEDITFOLDER "/keybindings.rc",getenv("HOME"));
			saveVarsToFile(filename,keybindings_rc);
			gtk_widget_hide(keysWindow);
		}
#endif
}

#ifndef _USEQT5_
gboolean setKeyInEntry(GtkEntry* widget,GdkEventKey* event,gpointer data)
#else
//TODO//
gboolean setKeyInEntry(void)
#endif
{
#ifndef _USEQT5_
	if((event->type==GDK_KEY_PRESS) && (event->keyval==GDK_KEY_Delete))
		{
			gtk_entry_set_text(widget,"");
			return(true);
		}

	if ((event->type==GDK_KEY_PRESS)&& (event->state & GDK_CONTROL_MASK))
		gtk_entry_set_text(widget,gdk_keyval_name(event->keyval));

#endif
	return(true);
}

void buildKeys()
{
#ifndef _USEQT5_
	GtkWidget*	vbox;
	GtkWidget*	item;
	GtkWidget*	hbox;
	char		keystring[32];
	int			loop;
	char*		keycutsinfo;

	if(keysWindow==NULL)
		{
			keysWindow=gtk_window_new(GTK_WINDOW_TOPLEVEL);
			gtk_window_set_title((GtkWindow*)keysWindow,"Define Keyboard Shortcuts");
			vbox=gtk_vbox_new(false,8);

			asprintf(&keycutsinfo,"%s","To set a custom shortcut:\nClick in the appropriate box and press CONTROL ( and optionally SHIFT ) plus your custom key.\nJust press 'Delete' to remove the shortcut\nClick 'Apply' to keep changes or 'Cancel' to discard any changes.");
			item=gtk_label_new(keycutsinfo);
			gtk_label_set_justify((GtkLabel*)item,GTK_JUSTIFY_CENTER);
			gtk_label_set_line_wrap((GtkLabel*)item,true);
			gtk_box_pack_start(GTK_BOX(vbox),item,false,false,0);
			free(keycutsinfo);
//functions
			for(loop=0;loop<NUMSHORTCUTS;loop++)
				{
					hbox=gtk_hbox_new(true,0);
					shortcuttext[loop]=gettext(shortcuttext[loop]);
					gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new(shortcuttext[loop]),true,true,0);
					entries[loop]=gtk_entry_new();
					g_signal_connect(G_OBJECT(entries[loop]),"key-press-event",G_CALLBACK(setKeyInEntry),NULL);
					gtk_box_pack_start(GTK_BOX(hbox),entries[loop],true,true,0);
					gtk_box_pack_start(GTK_BOX(vbox),hbox,true,true,0);
				}
//buttons
			gtk_box_pack_start(GTK_BOX(vbox),gtk_hseparator_new(),true,true,0);

			hbox=gtk_hbox_new(true,4);
			item=gtk_button_new_from_stock(GTK_STOCK_APPLY);
			gtk_box_pack_start(GTK_BOX(hbox),item,true,false,2);
			gtk_widget_set_name(item,"apply");
			g_signal_connect(G_OBJECT(item),"clicked",G_CALLBACK(setKeyCuts),(void*)item);	

			item=gtk_button_new_from_stock(GTK_STOCK_CANCEL);
			gtk_box_pack_start(GTK_BOX(hbox),item,true,false,2);
			gtk_widget_set_name(item,"cancel");
			g_signal_connect(G_OBJECT(item),"clicked",G_CALLBACK(setKeyCuts),(void*)item);
			gtk_box_pack_start(GTK_BOX(vbox),hbox,true,true,2);

//show it
			gtk_container_add(GTK_CONTAINER(keysWindow),(GtkWidget*)vbox);
		}

	for(int j=0;j<NUMSHORTCUTS;j++)
		{
			if(gdk_keyval_name(shortCuts[j][0])!=NULL)
				{
					sprintf(&keystring[0],"%s",gdk_keyval_name(shortCuts[j][0]));
					gtk_entry_set_text((GtkEntry*)entries[j],(char*)&keystring);
				}
		}
	gtk_widget_show_all(keysWindow);
#endif
}

void cancelPrefs(void)
{
	kkedit->prefsWindow->hide();
	//delete prefsWindow;
	kkedit->readConfigs();
	kkedit->resetAllFilePrefs();
	kkedit->switchPage(qobject_cast<QTabWidget*>(kkedit->mainNotebook)->currentIndex());
}



#ifndef _USEQT5_
void addRecentToMenu(GtkRecentChooser* chooser,GtkWidget* menu)
#else
//TODO//
void addRecentToMenu(void)
#endif
{
#ifndef _USEQT5_
	GList*		itemlist=NULL;
	GList*		l=NULL;
	GtkWidget*	menuitem;
	char*		uri=NULL;
	char*		filename=NULL;
	int			i=0;

	itemlist=gtk_recent_chooser_get_items(chooser);

	for (l=itemlist;l !=NULL;l=l->next)
		{
			const gchar *menuname;
			GtkRecentInfo *info = (GtkRecentInfo*)l->data;
			if (i >= MAXRECENT)
				break;
			i++;

			menuname=gtk_recent_info_get_display_name(info);
			uri=(char*)gtk_recent_info_get_uri(info);
			if (uri!=NULL)
				{
					filename=g_filename_from_uri((const gchar*)uri,NULL,NULL);
					menuitem=gtk_image_menu_item_new_with_label(menuname);
					gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(recentFileMenu),(void*)filename);
					g_free (uri);
				}
		}
#endif
}
#ifdef _USEQT5_
void keyp(QKeyEvent * event)
{
//	closingAll=false;
//	closeTab(NULL,tabnum);
printf("qqqqqqqqqqqqqqq\n");
}
#endif

void doneDialog(int state)
{
printf(">>%i<<\n",state);
//	spellCheckWord=NULL;
}


#ifdef _ASPELL_
void buildWordCheck(int documentCheck)
{
#ifndef _USEQT5_
	GtkWidget*	vbox;
	GtkWidget*	button;
	GtkWidget*	hbox;
	GtkWidget*	label;
	GtkWidget*	image;
	char*		labeltext[512];
	int			docflag=documentCheck;

	spellCheckWord=gtk_dialog_new();
	gtk_window_set_title((GtkWindow*)spellCheckWord,"Spell check word");
	vbox=gtk_vbox_new(true,8);

	hbox=gtk_hbox_new(true,8);

	sprintf((char*)&labeltext,"Change <i><b>%s</b></i> to: ",badWord);
	label=gtk_label_new((char*)&labeltext);
	gtk_label_set_use_markup((GtkLabel*)label,true);
	gtk_box_pack_start(GTK_BOX(hbox),label,true,true,0);

	wordListDropbox=gtk_combo_box_text_new();
	gtk_box_pack_start(GTK_BOX(hbox),wordListDropbox,true,true,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,true,true,0);

	hbox=gtk_hbox_new(true,8);
	button=gtk_button_new_from_stock(GTK_STOCK_APPLY);
	gtk_box_pack_start(GTK_BOX(hbox),button,true,true,0);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(doChangeWord),(gpointer)(long)docflag);

	button=gtk_button_new_with_label("Ignore");
	image=gtk_image_new_from_stock(GTK_STOCK_ADD,GTK_ICON_SIZE_MENU);
	gtk_button_set_image((GtkButton*)button,image);
	gtk_box_pack_start(GTK_BOX(hbox),button,true,true,0);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(doAddIgnoreWord),(gpointer)1);

	button=gtk_button_new_from_stock(GTK_STOCK_ADD);
	gtk_box_pack_start(GTK_BOX(hbox),button,true,true,0);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(doAddIgnoreWord),(gpointer)2);

	button=gtk_button_new_from_stock(GTK_STOCK_CANCEL);
	gtk_box_pack_start(GTK_BOX(hbox),button,true,true,0);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(doCancelCheck),NULL);

	gtk_box_pack_start(GTK_BOX(vbox),gtk_hseparator_new(),true,true,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,true,true,0);

	GtkWidget* content=gtk_dialog_get_content_area((GtkDialog *)spellCheckWord);
	gtk_container_add(GTK_CONTAINER(content),(GtkWidget*)vbox);

	gtk_signal_connect_object(GTK_OBJECT(spellCheckWord),"delete_event",GTK_SIGNAL_FUNC(gtk_widget_hide),GTK_OBJECT(spellCheckWord));
	g_signal_connect(G_OBJECT(spellCheckWord),"delete_event",GTK_SIGNAL_FUNC(gtk_true),NULL);
#endif
}
#endif

int showFunctionEntry(void)
{
	gint		result=false;
#ifndef _USEQT5_
	GtkWidget*	dialog;
	GtkWidget*	content_area;
	GtkWidget*	entrybox;

	dialog=gtk_message_dialog_new(GTK_WINDOW(mainWindow),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_OTHER,GTK_BUTTONS_NONE,"Enter Function Name");

	gtk_dialog_add_buttons((GtkDialog*)dialog,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,GTK_STOCK_OK,GTK_RESPONSE_YES,NULL);
	gtk_window_set_title(GTK_WINDOW(dialog),"Find Function");

	content_area=gtk_dialog_get_content_area(GTK_DIALOG(dialog));	
	entrybox=gtk_entry_new();
	gtk_entry_set_activates_default((GtkEntry*)entrybox,true);
	gtk_dialog_set_default_response((GtkDialog*)dialog,GTK_RESPONSE_YES);
	gtk_container_add(GTK_CONTAINER(content_area),entrybox);
	gtk_widget_show_all(content_area);
	result=gtk_dialog_run(GTK_DIALOG(dialog));
	if(functionSearchText!=NULL)
		debugFree(&functionSearchText,"showFunctionEntry functionSearchText");
	functionSearchText=strdup(gtk_entry_get_text((GtkEntry*)entrybox));
	gtk_widget_destroy(dialog);

#endif
	return(result);
}





