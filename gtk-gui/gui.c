/****************************************************************************/
/** Software: GUI prototype dedicated to master-mind                       **/
/** Author  : Ivan Auge (Email: auge@ensiie.fr)                            **/
/****************************************************************************/
/** Handle a dialog window dedicated to master-mind                        **/
/****************************************************************************/

#include <gtk/gtk.h>

#include "gui.h"
#include "canvas.h"

/****************************************************************************/
/* GUI data structure                                                       */

struct _Tgui {
    GdkDisplay* display;
    GtkWidget*  topwidget;
    GtkWidget*  topgrid;
    GtkWidget*  btn_undo;
    GtkWidget*  btn_help;
    GtkWidget*  btn_quit;
    Tcvs*       canvas;

    // run time data
    Taction     action;
    Tsegm       actionSegm;
};

/****************************************************************************/
/* main gui interface                                                       */

static gboolean gui_btn_undo_cb(GtkWidget* button, GdkEvent* event, gpointer user_data);
static gboolean gui_btn_help_cb(GtkWidget* button, GdkEvent* event, gpointer user_data);
static gboolean gui_btn_quit_cb(GtkWidget* button, GdkEvent* event, gpointer user_data);
#if 0
static gboolean gui_destroy_cb(GtkWidget* top, Tgui*gui)
{
    //printf("top=%p gui=%p cvs=%p DESTROT-CB\n",top,gui,gui->canvas);
    gui->action = GUI_ACT_Quit;
    gui->topwidget = 0;
    gtk_main_quit();
    return TRUE;
}
#endif

#if 1
static gboolean gui_delete_cb(GtkWidget* top, void* e, Tgui*gui)
{
    //printf("top=%p gui=%p cvs=%p DELETE-CB\n",top,gui,gui->canvas);
    gui->action = GUI_ACT_Quit;
    gtk_main_quit();
    return TRUE;
}
#endif

static GtkWidget* gui_btn_create(Tgui*gui, char*label, gboolean (*cb)(GtkWidget*, GdkEvent*, gpointer))
{
    GtkWidget* button = gtk_button_new_with_label(label);
    gtk_widget_set_events(button, GDK_BUTTON_PRESS_MASK /*| GDK_SCROLL_MASK*/);
    g_signal_connect(button, "button-press-event", G_CALLBACK(cb), gui);
    return button;
}

extern Tgui* gui_open (int cvspixsize, int cvspixstep)
{
    Tgui* gui = malloc( sizeof(*gui) );
    gui->action = GUI_ACT_Undef;

  /* initialize gtk & create the main window */
    gtk_init_check(NULL,NULL);
    gui->display =  gdk_display_get_default();
    gui->topwidget = gtk_window_new(GTK_WINDOW_TOPLEVEL);
//  g_signal_connect(gui->topwidget, "destroy"     , G_CALLBACK(gui_destroy_cb), gui);
    g_signal_connect(gui->topwidget, "delete_event", G_CALLBACK(gui_delete_cb), gui);

  /* creation of the sub-widgets */
    gui->btn_undo = gui_btn_create(gui,"Undo",gui_btn_undo_cb);
    gui->btn_help = gui_btn_create(gui,"Help",gui_btn_help_cb);
    gui->btn_quit = gui_btn_create(gui,"Quit",gui_btn_quit_cb);
    gui->canvas   = cvs_new(300,15,gui);

  /* place sub-widget in top window */ 
    gui->topgrid = gtk_grid_new(); // its a 2D matrice of widgets
    gtk_container_add(GTK_CONTAINER(gui->topwidget),gui->topgrid);
    gtk_grid_attach(GTK_GRID(gui->topgrid), gui->btn_undo           ,1,0,1,1);
    gtk_grid_attach(GTK_GRID(gui->topgrid), gui->btn_help           ,1,1,1,1);
    gtk_grid_attach(GTK_GRID(gui->topgrid), gui->btn_quit           ,1,2,1,1);
    gtk_grid_attach(GTK_GRID(gui->topgrid), cvs_widget(gui->canvas) ,0,0,1,3);

  /* build the dialog */
    gtk_widget_set_can_default(gui->topwidget, TRUE);
    gtk_widget_show_all(gui->topwidget);

    return gui;
}

extern void gui_close(Tgui*gui)
{
    cvs_delete(gui->canvas);
    gtk_widget_destroy(gui->topwidget);
    free(gui);
}

extern void    gui_redraw     (Tgui* gui)
{
    cvs_redraw( gui->canvas );
}

extern Taction gui_getAction  (Tgui* gui, Tsegm*segm)
{
    cvs_setModeStd(gui->canvas);
    gui->action        = GUI_ACT_Undef;
    gui->actionSegm.p1 = int2_init(0,0);
    gui->actionSegm.p2 = int2_init(0,0);
    gtk_main();
    *segm = gui->actionSegm;
    return gui->action;
}

extern Taction gui_getSegOfSet(Tgui* gui, Tsegm*lines, int lineNb, Tsegm*segm)
{
    cvs_setModeLAS(gui->canvas,lines,lineNb);
    gui->action        = GUI_ACT_Undef;
    gui->actionSegm.p1 = int2_init(0,0);
    gui->actionSegm.p2 = int2_init(0,0);
    gtk_widget_set_sensitive(gui->btn_undo,FALSE);
    gtk_widget_set_sensitive(gui->btn_help,FALSE);
    gtk_main();
    gtk_widget_set_sensitive(gui->btn_undo,TRUE);
    gtk_widget_set_sensitive(gui->btn_help,TRUE);
    *segm = gui->actionSegm;
    return gui->action;
}

/****************************************************************************/
/* points & lines interface                                                 */

extern void gui_supPointsLines (Tgui* gui)                       { cvs_clrPointsLines(gui->canvas); }
extern void gui_addPoints(Tgui* gui, Cint2* points, int pointNb) { cvs_addPoints(gui->canvas,points,pointNb); }
extern void gui_addLines (Tgui* gui, Csegm* lines , int lineNb)  { cvs_addLines (gui->canvas,lines ,lineNb); }
extern void gui_supLastLine(Tgui* gui) { cvs_supLastLine(gui->canvas); }

/****************************************************************************/
/* Button callbacks                                                         */

static gboolean gui_btn_undo_cb(GtkWidget* button, GdkEvent* event, gpointer user_data)
{ Tgui* gui = user_data; gui->action = GUI_ACT_Undo; gtk_main_quit(); return TRUE; }

static gboolean gui_btn_help_cb(GtkWidget* button, GdkEvent* event, gpointer user_data)
{ Tgui* gui = user_data; gui->action = GUI_ACT_Help; gtk_main_quit(); return TRUE; }

static gboolean gui_btn_quit_cb(GtkWidget* button, GdkEvent* event, gpointer user_data)
{ Tgui* gui = user_data; gui->action = GUI_ACT_Quit; gtk_main_quit(); return TRUE; }

/****************************************************************************/
/** Modal error dialog                                                     **/

extern void gui_error(Tgui*gui, const char* fmt,...)
{
    char buf[10000];

    va_list ap;
    va_start(ap,fmt);
    vsprintf(buf,fmt,ap);
    va_end(ap);
    GtkWidget* label = gtk_label_new (buf);

    GtkWidget* dialog;
    dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_MODAL,GTK_MESSAGE_WARNING,GTK_BUTTONS_OK,
            "%s",buf);
    gtk_dialog_run( GTK_DIALOG(dialog) );
    gtk_widget_destroy( dialog );
}

/****************************************************************************/
/* Interface to abstract gui structure                                      */

extern void gui_setRetValues(Tgui*gui, Taction action, Tsegm segm)
{
    gui->action     = action;
    gui->actionSegm = segm;
}

/****************************************************************************/
