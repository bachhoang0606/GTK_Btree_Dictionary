
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "bt-5.0.1/inc/btree.h"

// main window

GtkWidget *window;

GtkWidget *team, *add, *edit, *delete, *search;
GtkWidget *notify;
GtkWidget *text_search;
GtkWidget *text_suggest;
GtkWidget *text_trans, *text_data;
GtkBuilder *builder;

// Window team data

GtkWidget  *aboutdialog;
GtkWidget *button6;

// Btree

BTA *tree;

// hien thong tin team

void on_button6_clicked(GtkWidget *button)
{
    gtk_widget_hide(aboutdialog);
}

//an cac nut

void hide_widget() 
{
    gtk_widget_hide(add);
    gtk_widget_hide(edit);
    gtk_widget_hide(delete);
}

// them va chinh sua du lieu trong file Btree

void add_edit_data(GtkWidget* button)
{
    hide_widget();
    char data[1000];
    long rsize;
    GtkTextIter start, end;
    gchar *textget, *btext;
    GtkTextBuffer *textbuffer_data, *textbuffer_trans;

    tree = btopn("data/tudienanhviet.dat", 0, 0);

    textget = gtk_entry_get_text(GTK_ENTRY(text_search));
    textbuffer_data = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_data));
    textbuffer_trans = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_trans));

    gtk_text_buffer_get_bounds (textbuffer_data, &start, &end);
    btext = gtk_text_buffer_get_text(textbuffer_data, &start, &end ,FALSE);
    
    if (btsel(tree, textget, data, 1000, &rsize))  
    {
        btins(tree, textget, btext, strlen(btext)+1);  
        gtk_label_set_text(notify,"Data added successfully !");
    }else
    {
        btupd(tree, textget, btext, strlen(btext)+1);
        gtk_label_set_text(notify,"Successful data editing !");
    }

    gtk_entry_set_text(text_search,"");
	gtk_text_buffer_set_text(textbuffer_data,"",-1);
    gtk_text_buffer_set_text(textbuffer_trans,"",-1);
    gtk_widget_hide(add);
    btcls(tree); 
}

// xoa du lieu trong file Btree

void do_delete(GtkWidget *button)
{
    gchar *textget;
    long rsize;
    GtkTextBuffer *textbuffer_data, *textbuffer_trans;

    tree = btopn("data/tudienanhviet.dat", 0, 0);

    hide_widget();

    textget = gtk_entry_get_text(GTK_ENTRY(text_search));
    textbuffer_data = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_data));
    textbuffer_trans = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_trans));
    
    if(!btdel(tree, textget))
	{
        gtk_entry_set_text(text_search,"");
	    gtk_text_buffer_set_text(textbuffer_data,"",-1);
        gtk_text_buffer_set_text(textbuffer_trans,"",-1);
        gtk_label_set_text(notify, "Deleted data successfully !");
    } else
    {
        gtk_label_set_text(notify,"ERROR !\nCAN NOT DELETE !");
        return;
    }
       
 	hide_widget();
    btcls(tree); 
}

// tim kiem khi an enter va an phim search trong cua so

void do_search(GtkWidget *button)
{
    long rsize;
    gchar *textget;
    char data[1000];

    hide_widget();
    tree = btopn("data/tudienanhviet.dat", 0, 0);

    textget = gtk_entry_get_text(GTK_ENTRY(text_search));
    
    GtkTextBuffer *textbuffer_data, *textbuffer_trans;
    textbuffer_data = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_data));
    textbuffer_trans = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_trans));

    if (!strcmp(textget,""))
    {
        gtk_text_buffer_set_text(textbuffer_data,"",-1);
        gtk_text_buffer_set_text(textbuffer_trans,"",-1);
        gtk_label_set_text(notify,"");
    }    
    else{
        if (btsel(tree, textget, data, 1000, &rsize))
        {  
            gtk_text_buffer_set_text(textbuffer_data,"=> Add data here",-1);
            gtk_text_buffer_set_text(textbuffer_trans,textget,-1); 
    		gtk_label_set_text(notify, "So bad i can't find it !");
            gtk_widget_show(add);  
        }
        else
        {   
    		gtk_label_set_text(notify, "Great, I found it !");
            gtk_text_buffer_set_text(textbuffer_trans,textget,-1); 
            gtk_text_buffer_set_text(textbuffer_data,data,-1);            
            gtk_widget_show(delete);
            gtk_widget_show(edit);
        }
    }
    btcls(tree); 
}

// an tab tu dong dien string dau tien trong suggest vao searchetry

gboolean autoComplete(GtkWidget *widget, GdkEventKey *key, gpointer user_data){
	
	gchar *suggests, *textget;
    char press_tab[100];
    int count = 0;

    suggests = gtk_entry_get_text(GTK_ENTRY(text_suggest));
	textget = gtk_entry_get_text(GTK_ENTRY(widget));
	
	if(key->keyval == GDK_KEY_Tab)
    {
		do
        {
            press_tab[count] = suggests[count];
        }while(suggests[++count] != '\t');
        press_tab[count] = '\0';
        gtk_entry_set_text(widget, press_tab);
		return 1;
	}
	return 0;
}

// in ra cac từ gợi ý lên thanh entry

void quickSuggest(GtkButton *button)
{
    gtk_label_set_text(notify,"");
    hide_widget();

    gchar *textget;
    GtkTextBuffer *textbuffer_data, *textbuffer_trans;

    textget = gtk_entry_get_text(GTK_ENTRY(text_search));
    textbuffer_data = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_data));
    textbuffer_trans = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_trans));

    if (!strcmp(textget,"")) //neu search entry == null
    {
        gtk_text_buffer_set_text(textbuffer_data,"",-1);
        gtk_text_buffer_set_text(textbuffer_trans,"",-1);
        gtk_entry_set_text(text_suggest,"");
    }
    else
    {
        BTA* tree = btopn("data/tudienanhviet.dat", 0, 0);
        long rsize;
        char tmp[100];
        char data[1000];
        char suggests[100]; //dung calloc bi loi con tro ??

        strcpy(tmp, textget);
        strcpy(suggests, ""); // khong strcpy co tro tro linh tinh ??
        btsel(tree, "", data, 1000, &rsize);
        while(!btseln(tree, tmp, data, 1000, &rsize))
        {
            if(!memcmp(textget, tmp, strlen(textget)))
            {
                strncat(suggests, tmp, strlen(tmp));
                strncat(suggests,"\t",2);
            }
            if(strlen(suggests) >= 100) break;
        }

        gtk_entry_set_text(text_suggest, suggests);
        btcls(tree);
    }
}

int main(int argc, char* argv[])
{
    gtk_init(&argc, &argv);

    builder = gtk_builder_new_from_file("glade_file.glade");

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_builder_connect_signals(builder, NULL); 

    team = GTK_WIDGET(gtk_builder_get_object(builder, "team"));
    search = GTK_WIDGET(gtk_builder_get_object(builder, "search"));
    add = GTK_WIDGET(gtk_builder_get_object(builder, "add"));
    edit = GTK_WIDGET(gtk_builder_get_object(builder, "edit"));
    delete = GTK_WIDGET(gtk_builder_get_object(builder, "delete"));
    notify = GTK_WIDGET(gtk_builder_get_object(builder, "notify"));
    text_search = GTK_WIDGET(gtk_builder_get_object(builder, "searchentry"));
    text_suggest = GTK_WIDGET(gtk_builder_get_object(builder, "suggest"));

    text_trans = GTK_WIDGET(gtk_builder_get_object(builder, "trans"));
    text_data = GTK_WIDGET(gtk_builder_get_object(builder, "datatras"));

    aboutdialog = GTK_WIDGET(gtk_builder_get_object(builder, "aboutdialog"));
    button6 = GTK_WIDGET(gtk_builder_get_object(builder, "button6"));

    gtk_widget_hide(add);
    gtk_widget_hide(edit);
    gtk_widget_hide(delete);
    gtk_widget_show(window);
    gtk_main();
    return EXIT_SUCCESS;
}

