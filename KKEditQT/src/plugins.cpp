/******************************************************
*
*     ©keithhedger Wed 23 Jul 15:34:30 BST 2014
*     kdhedger68713@gmail.com
*
*     plugins.cpp
* 
******************************************************/

#include "kkedit-includes.h"

#define COLUMN_ENABLE 0
#define COLUMN_PLUGIN 1
#define NUM_COLUMNS 2

#ifndef _USEQT5_
GtkWidget*	plugAboutButton;
GtkWidget*	plugPrefsButton;
GtkWidget*	plugwindow;
GtkWidget*	treeview;
#else
//TODO//
#endif

#ifndef _USEQT5_
void enableToggled(GtkCellRendererToggle *cell,gchar *path_str,gpointer data)
#else
//TODO//
void enableToggled(void)
#endif
{
#ifndef _USEQT5_
	GtkTreeModel *model = (GtkTreeModel *)data;
	GtkTreeIter  iter;
	GtkTreePath *path = gtk_tree_path_new_from_string (path_str);
	gboolean fixed;

	/* get toggled iter */
	gtk_tree_model_get_iter (model,&iter, path);
	gtk_tree_model_get (model, &iter, COLUMN_ENABLE, &fixed, -1);

	/* do something with the value */
	fixed ^= 1;

	/* set new value */
	gtk_list_store_set (GTK_LIST_STORE (model), &iter, COLUMN_ENABLE, fixed, -1);

	/* clean up */
	gtk_tree_path_free (path);
#endif
}

void getPlugName(gpointer data,gpointer store)
{
#ifndef _USEQT5_
	GtkTreeIter		iter;
	moduleData*		plugdata;

	plugdata=(moduleData*)data;
	gtk_list_store_append((GtkListStore*)store,&iter);
	gtk_list_store_set((GtkListStore*)store,&iter,COLUMN_ENABLE,plugdata->enabled,COLUMN_PLUGIN,plugdata->name,-1);
#endif
}

#ifndef _USEQT5_
gboolean doSetPlugData(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer data)
#else
//TODO//
gboolean doSetPlugData(void)
#endif
{
#ifndef _USEQT5_
	int			enabled;
	char*		name=NULL;
	moduleData*	pd;
	char*		filepath;
	GtkWidget*	dialog;

	gtk_tree_model_get(model,iter,COLUMN_ENABLE,&enabled,COLUMN_PLUGIN,&name,-1);
	if(!enabled)
		globalPlugins->appendToBlackList(name);

	pd=globalPlugins->getPluginByName(name);

	if((pd->loaded==true) && (!enabled))
			{
				if(globalPlugins->enablePlugin(name,true)==0)
					{
						pd->loaded=!g_module_close(pd->module);
						pd->enabled=pd->loaded;
						pd->module=NULL;
						pd->loaded=false;
						pd->enabled=false;
						return(false);
					}
				else
					{
						dialog=gtk_message_dialog_new((GtkWindow*)mainWindow,GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_ERROR,GTK_BUTTONS_CLOSE,gettext("Plugin '%s' cannot be unloaded yet.\nRestart KKEdit to unload."),name);
						gtk_dialog_run(GTK_DIALOG(dialog));
						gtk_widget_destroy(dialog);
						pd->enabled=false;
						return(false);
					}
			}

	if((pd->loaded==false) && (enabled))
		{
			filepath=globalPlugins->getPluginPathByName(name);
			if(filepath==NULL)
				{
					printf("Can't open: %s\n",name);
					return(false);
				}

			pd->module=g_module_open(filepath,G_MODULE_BIND_LAZY);
			debugFree(&filepath,"doSetPlugData filepath");
				
			if(pd->module!= NULL)
				{
					pd->loaded=true;
					pd->enabled=true;
					globalPlugins->enablePlugin(name,false);
				}
			else
				{
					pd->loaded=false;
					pd->enabled=false;
					printf("Can't open: %s\n",filepath);
				}
		}

#endif
	return(false);
}

void setPlugsEnabled(void)
{
#ifndef _USEQT5_
	GtkTreeModel*	model;

	globalPlugins->deleteBlackList();
	model=gtk_tree_view_get_model((GtkTreeView*)treeview);
    gtk_tree_model_foreach(model,doSetPlugData,NULL);
#endif
}

#ifndef _USEQT5_
void setPlugPrefs(GtkWidget* widget,gpointer data)
#else
//TODO//
void setPlugPrefs(void)
#endif
{
#ifndef _USEQT5_
	GtkTreeModel*		model;
	GtkTreeSelection*	selection=NULL;
	GtkTreeIter			iter;
	char*				plugname;

	switch(long(data))
		{
			case 1:
				selection=gtk_tree_view_get_selection((GtkTreeView*)treeview);
				if((selection!=NULL) && (gtk_tree_selection_get_selected(selection,&model,&iter)))
					{
						gtk_tree_model_get(model,&iter,COLUMN_PLUGIN,&plugname,-1);
						globalPlugins->runPlugFunction(globalPlugins->getPluginByName(plugname),"plugPrefs");
						debugFree(&plugname,"setPlugPrefs plugname");
					}
				break;
			case 2:
				selection=gtk_tree_view_get_selection((GtkTreeView*)treeview);
				if((selection!=NULL) && (gtk_tree_selection_get_selected(selection,&model,&iter)))
					{
						gtk_tree_model_get(model,&iter,COLUMN_PLUGIN,&plugname,-1);
						globalPlugins->runPlugFunction(globalPlugins->getPluginByName(plugname),"doAbout");
						debugFree(&plugname,"setPlugPrefs plugname");
					}
				break;
//apply
			case 3:
				setPlugsEnabled();
				break;
//close
			case 4:
				gtk_widget_hide(plugwindow);
				gtk_widget_destroy(plugwindow);
				break;
		}
#endif
}

#ifndef _USEQT5_
void onRowSelected(GtkTreeView* treeview,gpointer userdata)
#else
//TODO//
void onRowSelected(void)
#endif
{
#ifndef _USEQT5_
	GtkTreeModel*		model;
	GtkTreeSelection*	selection=NULL;
	GtkTreeIter			iter;
	char*				plugname;

	selection=gtk_tree_view_get_selection((GtkTreeView*)treeview);
	if((selection!=NULL) && (gtk_tree_selection_get_selected(selection,&model,&iter)))
		{
			gtk_tree_model_get(model,&iter,COLUMN_PLUGIN,&plugname,-1);
			gtk_widget_set_sensitive(plugAboutButton,globalPlugins->checkForFunction(plugname,"doAbout"));
			gtk_widget_set_sensitive(plugPrefsButton,globalPlugins->checkForFunction(plugname,"plugPrefs"));
			debugFree(&plugname,"onRowSelected plugname");
		}
#endif
}

VISIBLE void doPlugPrefs(Widget* widget,uPtr data)
{
printf("doPlugPrefs %i\n",(int)(long)data);

#ifndef _USEQT5_
	GtkWidget*		vbox;
	GtkListStore*	store;
	GtkTreeModel*	model=NULL;
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GtkWidget*		button;
	GtkWidget*		hbox;

	plugwindow=gtk_window_new(GTK_WINDOW_TOPLEVEL);

	vbox=gtk_vbox_new(false,4);
	store=gtk_list_store_new(NUM_COLUMNS,G_TYPE_BOOLEAN,G_TYPE_STRING);

	g_list_foreach(globalPlugins->plugins,getPlugName,store);

	model=GTK_TREE_MODEL(store);
	treeview=gtk_tree_view_new_with_model(model);
	gtk_container_add (GTK_CONTAINER (vbox),treeview);

//enable
	renderer=gtk_cell_renderer_toggle_new();
	g_signal_connect(renderer,"toggled",G_CALLBACK(enableToggled),model);

	column=gtk_tree_view_column_new_with_attributes(gettext("Enable"),renderer,"active",COLUMN_ENABLE,NULL);
	gtk_tree_view_column_set_sizing(GTK_TREE_VIEW_COLUMN(column),GTK_TREE_VIEW_COLUMN_FIXED);
	gtk_tree_view_column_set_fixed_width(GTK_TREE_VIEW_COLUMN(column),50);
	gtk_tree_view_append_column((GtkTreeView*)treeview,column);

//plug
	renderer=gtk_cell_renderer_text_new();
	column=gtk_tree_view_column_new_with_attributes(gettext("Plug In"),renderer,"text",COLUMN_PLUGIN,NULL);
	gtk_tree_view_column_set_sort_column_id(column,COLUMN_PLUGIN);
	gtk_tree_view_append_column((GtkTreeView*)treeview,column);

	hbox=gtk_hbox_new(false,4);
//plugin prefs
	plugPrefsButton=gtk_button_new_from_stock(GTK_STOCK_PREFERENCES);
	gtk_box_pack_start((GtkBox*)hbox,plugPrefsButton,false,false,4);
	g_signal_connect(G_OBJECT(plugPrefsButton),"clicked",G_CALLBACK(setPlugPrefs),(void*)1);
//plugin about
	plugAboutButton=gtk_button_new_from_stock(GTK_STOCK_ABOUT);
	gtk_box_pack_start((GtkBox*)hbox,plugAboutButton,false,false,4);
	g_signal_connect(G_OBJECT(plugAboutButton),"clicked",G_CALLBACK(setPlugPrefs),(void*)2);
//apply
	button=gtk_button_new_from_stock(GTK_STOCK_APPLY);
	gtk_box_pack_start((GtkBox*)hbox,button,false,false,4);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(setPlugPrefs),(void*)3);
//close
	button=gtk_button_new_from_stock(GTK_STOCK_CLOSE);
	gtk_box_pack_start((GtkBox*)hbox,button,false,false,4);
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(setPlugPrefs),(void*)4);

	g_signal_connect(treeview,"cursor-changed",G_CALLBACK(onRowSelected),NULL);

	gtk_container_add (GTK_CONTAINER(vbox),hbox);
	gtk_container_add (GTK_CONTAINER(plugwindow),vbox);
	gtk_widget_show_all(plugwindow);
#endif
}

//left/right
VISIBLE void showSide(bool left)
{
#ifndef _USEQT5_
	if(left==true)
		{
			gtk_widget_show(globalPlugins->globalPlugData->leftUserBox);
			globalPlugins->globalPlugData->leftShow++;
		}
	else
		{
			gtk_widget_show(globalPlugins->globalPlugData->rightUserBox);
			globalPlugins->globalPlugData->rightShow++;
		}
#endif
}

VISIBLE void hideSide(bool left)
{
#ifndef _USEQT5_
	if(left==true)
		{
			globalPlugins->globalPlugData->leftShow--;
			if(globalPlugins->globalPlugData->leftShow<=0)
				{
					gtk_widget_hide(globalPlugins->globalPlugData->leftUserBox);
					globalPlugins->globalPlugData->leftShow=0;
				}
		}
	else
		{
			globalPlugins->globalPlugData->rightShow--;
			if(globalPlugins->globalPlugData->rightShow<=0)
				{
					gtk_widget_hide(globalPlugins->globalPlugData->rightUserBox);
					globalPlugins->globalPlugData->rightShow=0;
				}
		}
	
#endif
}

//top/bottom
VISIBLE void showTop(bool top)
{
#ifndef _USEQT5_
	if(top==true)
		{
			gtk_widget_show(globalPlugins->globalPlugData->topUserBox);
			globalPlugins->globalPlugData->topShow++;
		}
	else
		{
			gtk_widget_show(globalPlugins->globalPlugData->bottomUserBox);
			globalPlugins->globalPlugData->bottomShow++;
		}
#endif
}

VISIBLE void hideTop(bool top)
{
#ifndef _USEQT5_
	if(top==true)
		{
			globalPlugins->globalPlugData->topShow--;
			if(globalPlugins->globalPlugData->topShow<=0)
				{
					gtk_widget_hide(globalPlugins->globalPlugData->topUserBox);
					globalPlugins->globalPlugData->topShow=0;
				}
		}
	else
		{
			globalPlugins->globalPlugData->bottomShow--;
			if(globalPlugins->globalPlugData->bottomShow<=0)
				{
					gtk_widget_hide(globalPlugins->globalPlugData->bottomUserBox);
					globalPlugins->globalPlugData->bottomShow=0;
				}
		}
#endif
}




