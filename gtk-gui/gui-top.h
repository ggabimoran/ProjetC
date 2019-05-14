/****************************************************************************/
/** Software: GUI prototype dedicated to master-mind                       **/
/** Author  : Ivan Auge (Email: auge@ensiie.fr)                            **/
/****************************************************************************/
/** Base type and miscellaneous utilities                                  **/
/****************************************************************************/
#ifndef FILE_GUI_TOP_H
#define FILE_GUI_TOP_H
/****************************************************************************/

/****************************************************************************/
/* Some definitions                                                          */

#define Inline static inline

#define TABLE_NB(t) (sizeof(t)/sizeof(t[0]))

#define min2(x,y) ((x)<(y)?(x):(y))
#define max2(x,y) ((x)>(y)?(x):(y))
#define abs(x)    ((x)>=0?(x):(-(x)))

/****************************************************************************/
/* Memory allocation                                                        */
Inline void  gui_free   (void*p)         { if ( p ) free(p); }
Inline void* gui_malloc (int nb)         { void*ret=malloc(nb);    if (ret==0) { fprintf(stderr,"No more memory\n"); exit(1); } return ret; }
Inline void* gui_malloc0(int nb)         { void*ret=malloc(nb);    if (ret==0) { fprintf(stderr,"No more memory\n"); exit(1); } memset(ret,0,nb); return ret; }
Inline void* gui_realloc(void*p, int nb) { void*ret=realloc(p,nb); if (ret==0) { fprintf(stderr,"No more memory\n"); exit(1); } return ret; }

/****************************************************************************/
/* Base types                                                               */

// a 2D point
typedef struct _Tint2 { int   x ,y ; } Tint2;
typedef const   Tint2                  Cint2;

Inline Tint2 int2_init(int x  , int y)   { Tint2 pt; pt.x=x; pt.y=y; return pt; }
Inline int   int2_cmp (Tint2 l, Tint2 r) { int ret=l.x-r.x; return ret==0 ? 0 : l.y-r.y; }

// a line segment defined by 2 points
typedef struct _Tsegm { Tint2 p1,p2; } Tsegm;
typedef const   Tsegm                  Csegm;

/* set of 2D points */
typedef struct _Tint2Set     Tint2Set;
typedef const   Tint2Set     Cint2Set;
#define TPLSET int2
#define TPLSET_EleType Tint2
#define TPLSET_EleCype const Tint2
#define TPLSET_AddWithoutPtr
#include "set-header-tpl.h"

Inline void int2set_addXY    (Tint2Set*s, int x, int y) { Tint2 pt=int2_init(x,y); int2set_add(s,pt); }
extern int  int2set_getIdxXY (Tint2Set*s, int x, int y);
Inline void int2set_addUniqXY(Tint2Set*s, int x, int y) { if ( int2set_getIdxXY(s,x,y)<0 ) int2set_addXY(s,x,y); }
Inline void int2set_addUniqPT(Tint2Set*s, Tint2 pt    ) { int2set_addUniqXY(s,pt.x,pt.y); }

/* set of line segments */
typedef struct _TsegmSet     TsegmSet;
typedef const   TsegmSet     CsegmSet;
#define TPLSET segm
#define TPLSET_EleType Tsegm
#define TPLSET_EleCype Csegm
#define TPLSET_AddWithoutPtr
#include "set-header-tpl.h"

/****************************************************************************/
/** Miscellaneous utilities                                                **/

// Store at most the first pointNb points with integer coordinates of
// the line segment into the points array.
//
// Return the number of points stored into the points array.
extern int gui_getPointsOfSegment(Tint2*points, int pointNb, Tsegm line);

/****************************************************************************/
/* Debug                                                                    */

#define DEBUG
#ifdef DEBUG
# define TEMP_asf_printf(c,fmt,...) do { if (c) { fprintf(stderr,fmt,__VA_ARGS__); exit(1); }} while (0)
#else
# define TEMP_asf_printf(c,fmt,...) while (0)
#endif
#define ERROR_NotYetImplented() fprintf(stderr,"%s:%d:%s:ERROR: not yet implemented\n",__FILE__,__LINE__,__func__)
#define FATAL_NotYetImplented() do { fprintf(stderr,"%s:%d:%s:FATAL not yet implemented\n",__FILE__,__LINE__,__func__); exit(1); } while (0)

/****************************************************************************/
#endif // FILE_GUI_TOP_H
/****************************************************************************/
/** vim: set textwidth=0:                                                  **/
