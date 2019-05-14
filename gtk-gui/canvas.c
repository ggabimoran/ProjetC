/****************************************************************************/
/** Software: GUI prototype dedicated to master-mind                       **/
/** Author  : Ivan Auge (Email: auge@ensiie.fr)                            **/
/****************************************************************************/

#include <math.h>
#include <gtk/gtk.h>

#include "gui.h"
#include "canvas.h"

/****************************************************************************/
/* data structure                                                           */

struct _Tcvs {
    GtkWidget* wid;
    int        pixsize;    // widget dx and dy in pixel
    int        pixstep;    // a line every pixstep pixel.
    Tint2Set*  points;     // the points to draw
    TsegmSet*  lines;      // the lines and points to draw
              
    int        mode;       // 1: select a line among ltos lines    
                           // 0: select a line in canvas using 2 points

    // return data for mode=0
    int       nbPoints;   // number of set points in line (bellow)
    Tsegm     line;

    // return data for mode=1
    TsegmSet* ltos;       // line to select: select a line among those
    int       ltosIdx;    // >=0: ltos[ltosIdx] is the line that is
                          //      currently selected.
                          // -1:  no line is currently selected.
}; 

static Tcvs* cvs_dt_new(int pixsize, int gridpixstep)
{
    Tcvs* cvs = gui_malloc(sizeof(*cvs));
    cvs->pixsize = pixsize;
    cvs->pixstep = gridpixstep;
    cvs->points  = int2set_new(10);
    cvs->lines   = segmset_new(10);
    cvs->mode    = 0;
    cvs->ltos    = segmset_new(10);
    cvs->ltosIdx = -1;
    return cvs;
}
static void cvs_dt_delete(Tcvs*cvs)
{
    int2set_delete(cvs->points);
    segmset_delete(cvs->lines);
    segmset_delete(cvs->ltos);
    gui_free(cvs);
}

Inline int cvs_ConvPixToGrid(Tcvs*cvs, double x) { return round( (x-cvs->pixstep/2)/((double)cvs->pixstep) ); }
Inline void widget_setGuiUsrData(GtkWidget *w, void *ud) { g_object_set_data(G_OBJECT(w),"cvsud",ud);}
Inline void*widget_getGuiUsrData(GtkWidget *w)           { return g_object_get_data(G_OBJECT(w),"cvsud"); }

extern void cvs_setModeStd    (Tcvs*cvs)
{
    cvs->mode = 0;
    cvs->nbPoints = 0;
}

extern void cvs_setModeLAS (Tcvs*cvs, Tsegm*lines, int lineNb)
{
    cvs->mode = 1;
    if ( cvs->ltos==0 ) cvs->ltos = segmset_new(10);
    segmset_clear(cvs->ltos);
    segmset_appendT(cvs->ltos,lines,lineNb);
    cvs->ltosIdx = -1;
    cvs_redraw( cvs );
}

extern void cvs_clrPointsLines(Tcvs*cvs)
{
    int2set_clear(cvs->points);
    segmset_clear(cvs->lines);
}
extern void cvs_addPoints   (Tcvs*cvs, Cint2*pts  , int nb) { int2set_appendT(cvs->points,(Tint2*)pts,nb); }
extern void cvs_addLines    (Tcvs*cvs, Csegm*lines, int nb) { segmset_appendT(cvs->lines,(Tsegm*)lines,nb); }
extern void cvs_supLastLine (Tcvs*cvs)                      { segmset_supLast(cvs->lines); }

/****************************************************************************/
/** Drawing callback                                                       **/

/**
 * Returns the point set that is the union
 *   - of cvs->points
 *   - and points belonging to the lines of cvs->lines set and being
 *     on the grid. (integer coordinates).
 *
 * Caution the returned set must be freed by caller after use.
**/
static Tint2Set* cvs_dt_getPointsToDraw(Tcvs*cvs)
{
    int i,x0,y0,x,y,dx,dy;
    Tint2Set*ret = int2set_new(cvs->lines->eleNb*5);
//fprintf(stderr,"%s:%d:%s: ptsNb=%d retNb=%d\n",__FILE__,__LINE__,__func__,cvs->points->eleNb,ret->eleNb);
 
    // add line points to ret
    for ( i=0 ; i<cvs->lines->eleNb ; i+=1) {
        Tint2 points[1000];
        int   pi,pointNb = gui_getPointsOfSegment(
                points,TABLE_NB(points),cvs->lines->eles[i]);
        for (pi=0 ; pi<pointNb ; pi+=1)
                int2set_addUniqXY(ret,points[pi].x,points[pi].y);
    }

    // add canvas points to ret
    for (i=0 ; i<cvs->points->eleNb ; i+=1) {
//fprintf(stderr,"%s:%d:%s: ptsNb=%d retNb=%d\n",__FILE__,__LINE__,__func__,cvs->points->eleNb,ret->eleNb);
        int2set_addUniqPT(ret,cvs->points->eles[i]);
    }
    return ret;
}

static gboolean cvs_draw_cb(GtkWidget *widget, cairo_t *cr, gpointer   data)
{
    Tcvs*  cvs = widget_getGuiUsrData(widget);
    int    dx = gtk_widget_get_allocated_width (widget);
    int    dy = gtk_widget_get_allocated_height(widget);
    int  step = cvs->pixstep; 
    int i,xy,x=50,y=50;
    double g,r;
//fprintf(stderr,"%s:%d:%s: cvs=%p\n",__FILE__,__LINE__,__func__,cvs);

    // clear the canvas
    cairo_set_source_rgb (cr, 1, 1, 1);
    cairo_paint (cr);

    // draw grid
    g=0.5; cairo_set_source_rgb (cr, g, g, g);
    cairo_set_line_width(cr,0.5);
    for (xy=step/2 ; xy<dx ; xy+=step) {
//fprintf(stderr,"%3d %3d --> %3d %3d\n",xy,0,xy,dy);
        cairo_move_to(cr,xy,0); cairo_line_to(cr,xy,dy); // vertical line
    }
    for (xy=cvs->pixstep/2 ; xy<dy ; xy+=cvs->pixstep) {
        cairo_move_to(cr,0,xy); cairo_line_to(cr,dx,xy); // horizontal line
    }
    cairo_stroke(cr);

    // draw points
    g=0.0; cairo_set_source_rgb (cr, g, g, g);
    r = 2,5;
    Tint2Set*points = cvs_dt_getPointsToDraw(cvs);
//fprintf(stderr,"%s:%d:%s: cvs=%p nbpts=%d\n",__FILE__,__LINE__,__func__,cvs,points->eleNb);
    for (i=0 ; i<points->eleNb ; i+=1) {
        Tint2* p = points->eles+i;
        cairo_arc(cr,step/2+p->x*step,step/2+p->y*step,r,0, 2*G_PI);
        cairo_fill(cr);
    }
    int2set_delete(points);

    // draw lines
    g=0.0; cairo_set_source_rgb (cr, g, g, g);
    cairo_set_line_width(cr,1.5);
    for (i=0 ; i<cvs->lines->eleNb ; i+=1) {
        Tsegm* p = cvs->lines->eles+i;
        cairo_move_to(cr,p->p1.x*step+step/2,p->p1.y*step+step/2);
        cairo_line_to(cr,p->p2.x*step+step/2,p->p2.y*step+step/2);
    }
    cairo_stroke(cr);

    // draw ltos lines if needed
    if ( cvs->mode==1 ) {
        g=0.0; cairo_set_source_rgb (cr, g, g, 1);
        cairo_set_line_width(cr,1.5);
        for (i=0 ; i<cvs->ltos->eleNb ; i+=1) {
            Tsegm* p = cvs->ltos->eles+i;
            cairo_move_to(cr,p->p1.x*step+step/2,p->p1.y*step+step/2);
            cairo_line_to(cr,p->p2.x*step+step/2,p->p2.y*step+step/2);
        }
        cairo_stroke(cr);
        if ( cvs->ltosIdx>=0 ) {
            g=0.0; cairo_set_source_rgb (cr, g, 1, g);
            Tsegm* p = cvs->ltos->eles+cvs->ltosIdx;
            cairo_move_to(cr,p->p1.x*step+step/2,p->p1.y*step+step/2);
            cairo_line_to(cr,p->p2.x*step+step/2,p->p2.y*step+step/2);
            cairo_stroke(cr);
        }
    }

    // draw line  if needed
    if ( cvs->mode==0 && cvs->nbPoints==1 ) {
        g=0.7; cairo_set_source_rgb (cr, g, 1, g);
        cairo_set_line_width(cr,1.2);
        Tsegm* p = &cvs->line;
        cairo_move_to(cr,p->p1.x*step+step/2,p->p1.y*step+step/2);
        cairo_line_to(cr,p->p2.x*step+step/2,p->p2.y*step+step/2);
        cairo_stroke(cr);
    }

    //cairo_paint (cr);

    return GDK_EVENT_STOP;
}

static gboolean cvs_configure_cb (GtkWidget *widget, GdkEventConfigure *event, gpointer   data)
{
    //fprintf(stderr,"%s:%d:%s:\n",__FILE__,__LINE__,__func__);
}

/****************************************************************************/
/** pointer callbacks (button press, pointer motion and pointer            **/
/**                    leave/enter the canvas window)                      **/

// called when a pointer moved in canvas.
static gboolean cvs_motion_cb (GtkWidget*wid, GdkEventButton *event, Tgui* gui)
{
    Tcvs* cvs = widget_getGuiUsrData(wid);
//printf("%s:%d: mode=%d x=%d y=%d\n",__FILE__,__LINE__,cvs->mode,(int)event->x,(int)event->y);

    int x = cvs_ConvPixToGrid(cvs,event->x);
    int y = cvs_ConvPixToGrid(cvs,event->y);

    if ( cvs->mode==1 ) {
        // 1: select a line among ltos lines
        int i,ip,nb,imin; double dmin=0,d;
        Tint2 points[1000];
        for ( i=0 ; i<cvs->ltos->eleNb ; i+=1) {
            nb = gui_getPointsOfSegment(points,TABLE_NB(points),cvs->ltos->eles[i]);
            double d=0;
            for ( ip=0 ; ip<nb ; ip+=1) {
                d += (x-points[ip].x)*(x-points[ip].x);
                d += (y-points[ip].y)*(y-points[ip].y);
            }
            if ( i==0 || d<dmin ) { imin=i; dmin=d; }
//printf("%s:%d: mode=%d x=%d y=%d ip=%d d=%.0f
//dmin=%.0f\n",__FILE__,__LINE__,cvs->mode,x,y,ip,d,dmin);
        }
        cvs->ltosIdx = imin;
        cvs_redraw( cvs );
    } else if ( cvs->mode==0 && cvs->nbPoints==1 ) {
        // 0: select a line in canvas using 2 points
        cvs->line.p2 = int2_init(x,y);
        cvs_redraw( cvs );
    }
}

// called when a button is pressed in canvas.
static gboolean cvs_btn_press_cb (GtkWidget*widget, GdkEventButton *event, Tgui* gui)
{
    Tcvs* cvs = widget_getGuiUsrData(widget);
//printf("%s:%d: mode=%d x=%d y=%d\n",__FILE__,__LINE__,cvs->mode,(int)event->x,(int)event->y);

    if ( cvs->mode==1 ) {
        // 1: select a line among ltos lines
        Tsegm line = cvs->ltos->eles[ cvs->ltosIdx ];
        gui_setRetValues(gui, GUI_ACT_Segment,line);
        gtk_main_quit();
    } else  if ( cvs->mode==0) {
        // 0: select a line in canvas using 2 points
        int x = cvs_ConvPixToGrid(cvs,event->x);
        int y = cvs_ConvPixToGrid(cvs,event->y);
        if ( cvs->nbPoints==0 ) {
            cvs->nbPoints = 1;
            cvs->line.p1 = int2_init(x,y);
        } else {
            cvs->nbPoints = 2;
            cvs->line.p2 = int2_init(x,y);
            gui_setRetValues(gui, GUI_ACT_Segment,cvs->line);
            gtk_main_quit();
        }
    }
}

static gboolean cvs_enter_cb (GtkWidget*wid, GdkEventButton*event, Tgui*gui)
{
    Tcvs* cvs = widget_getGuiUsrData(wid);
//printf("%s:%d: mode=%d x=%d y=%d ENTER\n",__FILE__,__LINE__,cvs->mode,(int)event->x,(int)event->y);

    if ( cvs->mode==1 ) {
        // 1: select a line among ltos lines
    } else if ( cvs->mode==0 ) {
        // 0: select a line in canvas using 2 points
    }
}

static gboolean cvs_leave_cb (GtkWidget*wid, GdkEventButton*event, Tgui*gui)
{
    Tcvs* cvs = widget_getGuiUsrData(wid);
//printf("%s:%d: mode=%d x=%d y=%d LEAVE\n",__FILE__,__LINE__,cvs->mode,(int)event->x,(int)event->y);

    if ( cvs->mode==1 ) {
        // 1: select a line among ltos lines
        cvs->ltosIdx = -1;
        cvs_redraw( cvs );
    } else {
        // 0: select a line in canvas using 2 points
        cvs->nbPoints = 0;
        cvs_redraw( cvs );
    }
}

/****************************************************************************/
/** Create and delete the canvas widget                                    **/

extern Tcvs* cvs_new(int pixsize, int gridpixstep, Tgui*gui)
{
    Tcvs* cvs = cvs_dt_new(pixsize,gridpixstep);
    cvs->wid  = gtk_drawing_area_new ();
    widget_setGuiUsrData(cvs->wid,cvs);
    gtk_widget_set_size_request(cvs->wid, pixsize, pixsize);

    g_signal_connect (cvs->wid, "draw"                , G_CALLBACK (cvs_draw_cb)     , NULL);
    g_signal_connect (cvs->wid, "configure-event"     , G_CALLBACK (cvs_configure_cb), NULL);
    g_signal_connect (cvs->wid, "motion-notify-event" , G_CALLBACK (cvs_motion_cb)   , gui);
    g_signal_connect (cvs->wid, "button-press-event"  , G_CALLBACK (cvs_btn_press_cb), gui);
    g_signal_connect (cvs->wid, "enter-notify-event"  , G_CALLBACK (cvs_enter_cb)    , gui);
    g_signal_connect (cvs->wid, "leave-notify-event"  , G_CALLBACK (cvs_leave_cb)    , gui);

    gtk_widget_set_events (cvs->wid, gtk_widget_get_events(cvs->wid) | GDK_BUTTON_PRESS_MASK
                                                           | GDK_POINTER_MOTION_MASK
                                                           | GDK_ENTER_NOTIFY_MASK
                                                           | GDK_LEAVE_NOTIFY_MASK
    );

    return cvs;
}

extern void       cvs_delete(Tcvs*cvs) { cvs_dt_delete(cvs); }
extern void       cvs_redraw(Tcvs*cvs) { gtk_widget_queue_draw( cvs->wid ); }
extern GtkWidget* cvs_widget(Tcvs*cvs) { return cvs->wid; }
extern int        cvs_size  (Tcvs*cvs) { return (cvs->pixsize-2*cvs->pixstep)/cvs->pixstep; }

/****************************************************************************/
