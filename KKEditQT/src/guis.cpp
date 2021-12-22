/*
 *
 * K.D.Hedger 2013 <kdhedger68713@gmail.com>
 *
*/

#include "kkedit-includes.h"

#define TABLECOLS 2
//
//void findTool(toolStruct* data,char* toolname)
//{
//#ifndef _USEQT5_
//	if(strcmp(toolname,data->menuName)==0)
//		selectedToolFromDrop=data;
//#endif
//}
//
//#ifndef _USEQT5_
//void selectToolOptions(GtkWidget* widget,gpointer data)
//#else
////TODO//
//void selectToolOptions(void)
//#endif
//{
//#ifndef _USEQT5_
//	char*	text=gtk_combo_box_text_get_active_text((GtkComboBoxText*)widget);
//	int		flags=0;
//
//	selectedToolFromDrop=NULL;
//
//	g_list_foreach(toolsList,(GFunc)findTool,text);
//	if(selectedToolFromDrop!=NULL)
//		{
//			gtk_toggle_button_set_active((GtkToggleButton*)inPopupWidget,selectedToolFromDrop->inPopUp);
//			gtk_toggle_button_set_active((GtkToggleButton*)alwaysPopupWidget,selectedToolFromDrop->alwaysPopup);
//			gtk_toggle_button_set_active((GtkToggleButton*)clearViewWidget,selectedToolFromDrop->clearView);
//			gtk_toggle_button_set_active((GtkToggleButton*)runAsRootWidget,selectedToolFromDrop->runAsRoot);
//			gtk_toggle_button_set_active((GtkToggleButton*)runAsRootWidget,selectedToolFromDrop->runAsRoot);
//			gtk_toggle_button_set_active((GtkToggleButton*)useBarWidget,selectedToolFromDrop->useBar);
//
//			gtk_entry_set_text((GtkEntry*)toolNameWidget,selectedToolFromDrop->menuName);
//			gtk_entry_set_text((GtkEntry*)commandLineWidget,selectedToolFromDrop->command);
//			if(selectedToolFromDrop->comment!=NULL)
//				gtk_entry_set_text((GtkEntry*)commentWidget,selectedToolFromDrop->comment);
//			else
//				gtk_entry_set_text((GtkEntry*)commentWidget,"");
//
//			if(selectedToolFromDrop->keyCode!=0)
//				gtk_entry_set_text((GtkEntry*)keyWidget,gdk_keyval_name(selectedToolFromDrop->keyCode));
//			else
//				gtk_entry_set_text((GtkEntry*)keyWidget,"");
//
//			flags=selectedToolFromDrop->flags;
//			if(flags & TOOL_ASYNC)
//				gtk_toggle_button_set_active((GtkToggleButton*)syncWidget,false);
//			else
//				gtk_toggle_button_set_active((GtkToggleButton*)syncWidget,true);
//
//			if((flags & TOOL_INSERT_MASK)==TOOL_IGNORE_OP)
//				gtk_toggle_button_set_active((GtkToggleButton*)ignoreWidget,true);
//			if((flags & TOOL_INSERT_MASK)==TOOL_VIEW_OP)
//				gtk_toggle_button_set_active((GtkToggleButton*)outputWidget,true);
//			if((flags & TOOL_INSERT_MASK)==TOOL_PASTE_OP)
//				gtk_toggle_button_set_active((GtkToggleButton*)pasteWidget,true);
//			if((flags & TOOL_INSERT_MASK)==TOOL_REPLACE_OP)
//				gtk_toggle_button_set_active((GtkToggleButton*)replaceWidget,true);
//
//			if((gtk_toggle_button_get_active((GtkToggleButton*)syncWidget)==false) || (gtk_toggle_button_get_active((GtkToggleButton*)inTermWidget)==true))
//				{
//					gtk_widget_set_sensitive(ignoreWidget,false);
//					gtk_widget_set_sensitive(pasteWidget,false);
//					gtk_widget_set_sensitive(replaceWidget,false);
//					gtk_widget_set_sensitive(outputWidget,false);
//				}
//			else
//				{
//					gtk_widget_set_sensitive(ignoreWidget,true);
//					gtk_widget_set_sensitive(pasteWidget,true);
//					gtk_widget_set_sensitive(replaceWidget,true);
//					gtk_widget_set_sensitive(outputWidget,true);
//				}
//
//			if(flags & TOOL_SHOW_DOC)
//				gtk_toggle_button_set_active((GtkToggleButton*)showDocWidget,true);
//			else
//				gtk_toggle_button_set_active((GtkToggleButton*)showDocWidget,false);
//		}
//#endif
//}
//
//void addToolToDrop(gpointer data,gpointer user_data)
//{
//#ifndef _USEQT5_
//	if(((toolStruct*)data)->global==false)
//		gtk_combo_box_text_append_text((GtkComboBoxText*)toolSelect,((toolStruct*)data)->menuName);
//#endif
//}
//
//#ifndef _USEQT5_
//void fillCombo(GtkComboBoxText* combo)
//#else
////TODO//
//void fillCombo(void)
//#endif
//{
//#ifndef _USEQT5_
//	g_list_foreach(toolsList,addToolToDrop,NULL);
//#endif
//}
//
//#ifndef _USEQT5_
//gboolean getToolKey(GtkEntry* widget,GdkEventKey* event,gpointer data)
//#else
////TODO//
//gboolean getToolKey(void)
//#endif
//{
//#ifndef _USEQT5_
//	if((event->type==GDK_KEY_PRESS) && (event->keyval==GDK_KEY_Delete))
//		{
//			gtk_entry_set_text(widget,"");
//			return(true);
//		}
//
//	if ((event->type==GDK_KEY_PRESS)&& (event->state & GDK_CONTROL_MASK))
//		gtk_entry_set_text(widget,gdk_keyval_name(event->keyval));
//
//#endif
//	return(true);
//}
//
//void doMakeTool(void)
//{
//#ifndef _USEQT5_
//	GtkWidget*	vbox;
//	GtkWidget*	hbox;
//	GtkWidget*	button;
//	GtkWidget*	toolwin;
//	GtkWidget*	infolabel;
//
//	char*		placeholderinfo;
//
//	asprintf(&placeholderinfo,"%s","PLACEHOLDERS:\n%t - Currently selected text. Passed to command as $KKEDIT_SELECTION\n%f - Filepath of the current document. Passed to command as $KKEDIT_CURRENTFILE\n%d - Directory of the current document or $HOME. Passed to command as $KKEDIT_CURRENTDIR\n%i - The location of the globally installed tools. Passed to command as $KKEDIT_DATADIR\n%h - Tempory file for displaying html in doc viewer. Passed to command as $KKEDIT_HTMLFILE\n%l - Highlihting language. Passed to command as $KKEDIT_SOURCE_LANG");
//
//	toolwin=gtk_window_new(GTK_WINDOW_TOPLEVEL);
//	gtk_window_set_title((GtkWindow*)toolwin,"Edit External Tools");
//	vbox=gtk_vbox_new(false,8);
//
////select tool
//	toolSelect=gtk_combo_box_text_new();
//	gtk_box_pack_start(GTK_BOX(vbox),toolSelect,false,true,0);
//	g_signal_connect(G_OBJECT(toolSelect),"changed",G_CALLBACK(selectToolOptions),NULL);
//	fillCombo((GtkComboBoxText*) toolSelect);
//
////name
//	toolNameWidget=gtk_entry_new();
//	hbox=gtk_hbox_new(false,0);
//	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new("Tool Name:\t"),false,true,0);
//	gtk_box_pack_start(GTK_BOX(hbox),toolNameWidget,true,true,0);
//	gtk_box_pack_start(GTK_BOX(vbox),hbox,false,true,0);
//	gtk_widget_show(toolNameWidget);
//	gtk_entry_set_text((GtkEntry*)toolNameWidget,"New Tool");
//
////command
//	commandLineWidget=gtk_entry_new();
//	hbox=gtk_hbox_new(false,0);
//	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new("Command: \t"),false,true,0);
//	gtk_box_pack_start(GTK_BOX(hbox),commandLineWidget,true,true,0);
//	gtk_box_pack_start(GTK_BOX(vbox),hbox,false,true,0);
//	gtk_widget_show(commandLineWidget);
//	gtk_entry_set_text((GtkEntry*)commandLineWidget,"");
////key
//	keyWidget=gtk_entry_new();
//	gtk_widget_show(keyWidget);
//	hbox=gtk_hbox_new(false,0);
//	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new("Shortcut:\t\t"),false,true,0);
//	gtk_box_pack_start(GTK_BOX(hbox),keyWidget,true,true,0);
//	gtk_box_pack_start(GTK_BOX(vbox),hbox,false,true,0);
//	gtk_entry_set_text((GtkEntry*)keyWidget,"");
//	g_signal_connect(G_OBJECT(keyWidget),"key-press-event",G_CALLBACK(getToolKey),NULL);
//
////comment
//	commentWidget=gtk_entry_new();
//	gtk_widget_show(commentWidget);
//	hbox=gtk_hbox_new(false,0);
//	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new("Comment:  \t"),false,true,0);
//	gtk_box_pack_start(GTK_BOX(hbox),commentWidget,true,true,0);
//	gtk_box_pack_start(GTK_BOX(vbox),hbox,false,true,0);
//	gtk_entry_set_text((GtkEntry*)commentWidget,"");
//
////info
//	infolabel=gtk_label_new(placeholderinfo);
//	gtk_label_set_selectable((GtkLabel*)infolabel,true);
//	gtk_box_pack_start(GTK_BOX(vbox),infolabel,false,false,0);
//	free(placeholderinfo);
////in terminal
//	inTermWidget=gtk_check_button_new_with_label("Run Tool In Terminal");
//	gtk_widget_set_name(inTermWidget,"interm");
//	gtk_box_pack_start(GTK_BOX(vbox),inTermWidget,false,true,0);
//	g_signal_connect(G_OBJECT(inTermWidget),"toggled",G_CALLBACK(setToolOptions),NULL);
////show in popup menu
//	inPopupWidget=gtk_check_button_new_with_label("Show Tool In Pop-Up Menu");
//	gtk_widget_set_name(inPopupWidget,"inpopup");
//	gtk_box_pack_start(GTK_BOX(vbox),inPopupWidget,false,true,0);
//	g_signal_connect(G_OBJECT(inPopupWidget),"toggled",G_CALLBACK(setToolOptions),NULL);
////always show in popup
//	alwaysPopupWidget=gtk_check_button_new_with_label("Always Show Tool In Pop-Up Menu");
//	gtk_widget_set_name(alwaysPopupWidget,"alwayspopup");
//	gtk_box_pack_start(GTK_BOX(vbox),alwaysPopupWidget,false,true,0);
//	g_signal_connect(G_OBJECT(alwaysPopupWidget),"toggled",G_CALLBACK(setToolOptions),NULL);
//
////flags
////snch/async
//	syncWidget=gtk_check_button_new_with_label("Run Tool Synchronously");
//	gtk_widget_set_name(syncWidget,"sync");
//	gtk_box_pack_start(GTK_BOX(vbox),syncWidget,false,true,0);
//	g_signal_connect(G_OBJECT(syncWidget),"toggled",G_CALLBACK(setToolOptions),NULL);
//
////flags - show html doc
//	showDocWidget=gtk_check_button_new_with_label("Show HTML Doc");
//	gtk_widget_set_name(showDocWidget,"showdoc");
//	gtk_box_pack_start(GTK_BOX(vbox),showDocWidget,false,true,0);
//	g_signal_connect(G_OBJECT(showDocWidget),"toggled",G_CALLBACK(setToolOptions),NULL);
//
////flags - clear view first
//	clearViewWidget=gtk_check_button_new_with_label("Clear Tool Output First");
//	gtk_widget_set_name(clearViewWidget,"clearview");
//	gtk_box_pack_start(GTK_BOX(vbox),clearViewWidget,false,true,0);
//	g_signal_connect(G_OBJECT(clearViewWidget),"toggled",G_CALLBACK(setToolOptions),NULL);
//
////flags - run as root
//	runAsRootWidget=gtk_check_button_new_with_label("Run Tool As Root");
//	gtk_widget_set_name(runAsRootWidget,"runasroot");
//	gtk_box_pack_start(GTK_BOX(vbox),runAsRootWidget,false,true,0);
//	g_signal_connect(G_OBJECT(runAsRootWidget),"toggled",G_CALLBACK(setToolOptions),NULL);
//
////flags - use progressbar
//	useBarWidget=gtk_check_button_new_with_label("Use Progress Bar");
//	gtk_widget_set_name(useBarWidget,"usebar");
//	gtk_box_pack_start(GTK_BOX(vbox),useBarWidget,false,true,0);
//	g_signal_connect(G_OBJECT(useBarWidget),"toggled",G_CALLBACK(setToolOptions),NULL);
//
////flags - ignore
//	ignoreWidget=gtk_radio_button_new_with_label(NULL,"Ignore Output");
//	gtk_widget_set_name(ignoreWidget,"ignore");
//	gtk_toggle_button_set_active((GtkToggleButton*)ignoreWidget,true);
//	gtk_box_pack_start(GTK_BOX(vbox),ignoreWidget,false,true,0);
//
////flags - paste
//	pasteWidget=gtk_radio_button_new_with_label_from_widget((GtkRadioButton*)ignoreWidget,"Paste Output");
//	gtk_widget_set_name(pasteWidget,"paste");
//	gtk_box_pack_start(GTK_BOX(vbox),pasteWidget,false,true,0);
//
////flags - replace all
//	replaceWidget=gtk_radio_button_new_with_label_from_widget((GtkRadioButton*)ignoreWidget,"Replace All Contents");
//	gtk_widget_set_name(replaceWidget,"replace");
//	gtk_box_pack_start(GTK_BOX(vbox),replaceWidget,false,true,0);
//
////flags - view
//	outputWidget=gtk_radio_button_new_with_label_from_widget((GtkRadioButton*)ignoreWidget,"View Output");
//	gtk_widget_set_name(outputWidget,"outtoview");
//	gtk_box_pack_start(GTK_BOX(vbox),outputWidget,false,true,0);
//
////buttons
//	gtk_box_pack_start(GTK_BOX(vbox),gtk_hseparator_new(),false,false,0);
//
//	hbox=gtk_hbox_new(false,0);
//	button=gtk_button_new_from_stock(GTK_STOCK_APPLY);
//	gtk_box_pack_start(GTK_BOX(hbox),button,true,false,2);
//	gtk_widget_set_name(button,"apply");
//	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(setToolOptions),(void*)toolwin);	
//
//	button=gtk_button_new_from_stock(GTK_STOCK_DELETE);
//	gtk_box_pack_start(GTK_BOX(hbox),button,true,false,2);
//	gtk_widget_set_name(button,"delete");
//	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(setToolOptions),(void*)toolwin);	
//
//	button=gtk_button_new_from_stock(GTK_STOCK_CANCEL);
//	gtk_box_pack_start(GTK_BOX(hbox),button,true,false,2);
//	gtk_widget_set_name(button,"cancel");
//	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(setToolOptions),(void*)toolwin);
//	gtk_box_pack_start(GTK_BOX(vbox),hbox,true,false,2);
//
////show it
//	gtk_container_add(GTK_CONTAINER(toolwin),(GtkWidget*)vbox);
//	gtk_widget_show_all(toolwin);
//#endif
//}

void buildTools(void)
{
//TODO//
printf("buildTools\n");
#ifndef _USEQT5_
	GtkWidget*		menuitem;
	GtkWidget*		menu;
	GtkWidget*		image;
	GList*			ptr;
	bool			gotglobal=false;
	int				keyflags=0;

	buildToolsList();

	GtkWidget* submenu=gtk_menu_item_get_submenu((GtkMenuItem*)toolsMenu);
	if(submenu!=NULL)
		gtk_menu_item_set_submenu((GtkMenuItem*)toolsMenu,NULL);

	menu=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(toolsMenu),menu);

//addtool
	menuitem=gtk_image_menu_item_new_with_label("Manage External Tools");
	image=gtk_image_new_from_stock(GTK_STOCK_EDIT,GTK_ICON_SIZE_MENU);
	gtk_image_menu_item_set_image((GtkImageMenuItem *)menuitem,image);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
	g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(doMakeTool),NULL);

	menuitem=gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);

	ptr=toolsList;
	while(ptr!=NULL)
		{
			if( ((toolStruct*)ptr->data)->global==true)
				{
					gotglobal=true;
					menuitem=gtk_image_menu_item_new_with_label(((toolStruct*)ptr->data)->menuName);
					gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(externalTool),(void*)ptr->data);

					if(((toolStruct*)ptr->data)->keyCode>0)
						{
							keyflags=0;
							if(gdk_keyval_is_upper(((toolStruct*)ptr->data)->keyCode))
								keyflags=GDK_SHIFT_MASK;
							gtk_widget_add_accelerator((GtkWidget *)menuitem,"activate",accgroup,((toolStruct*)ptr->data)->keyCode,(GdkModifierType)(GDK_CONTROL_MASK|keyflags),GTK_ACCEL_VISIBLE);
						}

					if(((toolStruct*)ptr->data)->comment!=NULL)
						gtk_widget_set_tooltip_text((GtkWidget*)menuitem,((toolStruct*)ptr->data)->comment);
				}
			ptr=g_list_next(ptr);
		}

	if(gotglobal==true)
		{
			menuitem=gtk_separator_menu_item_new();
			gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
		}

	ptr=toolsList;
	while(ptr!=NULL)
		{
			if( ((toolStruct*)ptr->data)->global==false)
				{
					menuitem=gtk_image_menu_item_new_with_label(((toolStruct*)ptr->data)->menuName);
					gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
					g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(externalTool),(void*)ptr->data);

					if(((toolStruct*)ptr->data)->keyCode>0)
						{
							keyflags=0;
							if(gdk_keyval_is_upper(((toolStruct*)ptr->data)->keyCode))
								{
									keyflags=GDK_SHIFT_MASK;
									if(gdk_keyval_is_lower(((toolStruct*)ptr->data)->keyCode))
									keyflags=0;
								}
							gtk_widget_add_accelerator((GtkWidget *)menuitem,"activate",accgroup,((toolStruct*)ptr->data)->keyCode,(GdkModifierType)(GDK_CONTROL_MASK|keyflags),GTK_ACCEL_VISIBLE);
						}

					if(((toolStruct*)ptr->data)->comment!=NULL)
						gtk_widget_set_tooltip_text((GtkWidget*)menuitem,((toolStruct*)ptr->data)->comment);
				}
			ptr=g_list_next(ptr);
		}
#endif
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





