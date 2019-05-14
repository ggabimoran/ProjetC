/****************************************************************************/
/** Software: GUI prototype dedicated to master-mind                       **/
/** Author  : Ivan Auge (Email: auge@ensiie.fr)                            **/
/****************************************************************************/
/** Base type and miscellaneous utilities                                  **/
/****************************************************************************/

#include <math.h>
#include <gtk/gtk.h>

#include "gui-top.h"

/****************************************************************************/
/* set of 2D points                                                         */
#define TPLSET int2
#define TPLSET_EleType Tint2
#define TPLSET_EleCype const Tint2
#define TPLSET_AddWithoutPtr
#include "set-body-tpl.h"

extern int  int2set_getIdxXY (Tint2Set*s, int x, int y)
{
    int i;
    if (s==0) return -1;
    for (i=0;i<s->eleNb;i+=1) {
      if ( s->eles[i].x==x && s->eles[i].y==y )
          return i;
    }
    return -1;
}

/****************************************************************************/
/* set of line segments                                                     */
#define TPLSET segm
#define TPLSET_EleType Tsegm
#define TPLSET_EleCype Csegm
#define TPLSET_AddWithoutPtr
#include "set-body-tpl.h"

/****************************************************************************/
/** Miscellaneous utilities                                                **/

extern int gui_getPointsOfSegment(Tint2*points, int pointNb, Tsegm line)
{
    int x,y,dx,dy,x0,y0,ret=0;;
    Tint2 p1 = line.p1;
    Tint2 p2 = line.p2;
    if ( p1.x==p2.x ) {
        // vertical line
        for ( y=min2(p1.y,p2.y) ; y<=max2(p1.y,p2.y) ; y+=1) 
            points[ret++] = int2_init(p1.x,y);
    } else if ( p1.y==p2.y ) {
        // horizontal line
        for ( x=min2(p1.x,p2.x) ; x<=max2(p1.x,p2.x) ; x+=1) {
//fprintf(stderr,"%s:%d:%s: ptsNb=%d retNb=%d x=%d y=%d\n",__FILE__,__LINE__,__func__,dt->points->eleNb,ret->eleNb,x,p1.y);
            points[ret++] = int2_init(x,p1.y);
        }
    } else {
        dx = p1.x-p2.x;
        dy = p1.y-p2.y;
        if ( abs(dx) < abs(dy) ) { 
            if ( p1.x<=p2.x ) { x0=p1.x; y0=p1.y; }
            else              { x0=p2.x; y0=p2.y; }
            for (x=x0 ; x<=max2(p1.x,p2.x) ; x+=1 ) {
                // (y-y0)/(x-x0) = dy/dx --> y = y0 + (dy*(x-x0))/dx
                if ( (((x-x0)*dy)%dx)!=0 ) continue;
                points[ret++] = int2_init(x,y0+((x-x0)*dy)/dx);
            }
        } else {
            if ( p1.y<=p2.y ) { x0=p1.x; y0=p1.y; }
            else              { x0=p2.x; y0=p2.y; }
            for (y=y0 ; y<=max2(p1.y,p2.y) ; y+=1 ) {
                // (x-x0)/(y-y0) = dx/dy --> x = x0 + ((y-y0)*dx)/dy
                if ( (((y-y0)*dx)%dy)!=0 ) continue;
                points[ret++] = int2_init(x0 + ((y-y0)*dx)/dy,y);
            }
        }
    }
    return ret;
}

/****************************************************************************/
