/****************************************************************************/
/** Software: GUI prototype dedicated to master-mind                       **/
/** Author  : Ivan Auge (Email: auge@ensiie.fr)                            **/
/****************************************************************************/
/** Une toile (canvas) est un une zone de dessin carré sur la quelle on    **/
/** peut dessinner des points et des segments.                             **/
/**                                                                        **/
/** Les points et les points extrémitées des segments ont des coordonnées  **/
/** entières (élément de l'ensemble des entiers Z). La toile est carrée.   **/
/**                                                                        **/
/**         0    1    2    3    4 --> x                                    **/
/**         |    |    |    |    |                                          **/
/**     0 --+----+----+----+----+--                                        **/
/**         |    |    |    |    |                                          **/
/**     1 --+----+----+----+----+--                                        **/
/**         |    |    |    |    |                                          **/
/**     2 --+----+----+----+----+--                                        **/
/**         |    |    |    |    |                                          **/
/**     3 --+----+----+----+----+--                                        **/
/**         |    |    |    |    |                                          **/
/**     4 --+----+----+----+----+--                                        **/
/**     |   |    |    |    |    |                                          **/
/**     V                                                                  **/
/**     y                                                                  **/
/**                                                                        **/
/****************************************************************************/
#ifndef FILE_CANVAS_H
#define FILE_CANVAS_H
/****************************************************************************/

typedef struct _Tcvs Tcvs;

/****************************************************************************/

/**
 * The cvs_new function create a canvas widget. Its size
 * is set to pixsize x pixsize pixels. The space between 
 * two grid line is gridpixstep pixels.
 * The created canvas is empty, it contains neither point
 * nor line.
 *
 * Return:
 *  The function returns the created canvas.
**/
extern Tcvs* cvs_new   (int pixsize, int gridpixstep, Tgui*gui);
extern void  cvs_delete(Tcvs*cvs);
extern void  cvs_redraw(Tcvs*cvs);

// some cvs getters
extern GtkWidget* cvs_widget(Tcvs*cvs);
extern int        cvs_size  (Tcvs*cvs);

// start a FSM that selects a line in canvas using 2 points
extern void cvs_setModeStd(Tcvs*cvs);

// start a FSM that selects a line among the first lineNb segments
// of the lines array.
extern void cvs_setModeLAS (Tcvs*cvs,  Tsegm*lines, int lineNb);

// Suppress all the points and lines of cvs canvas
extern void cvs_clrPointsLines(Tcvs*cvs);

// Add to cvs canvas the nb first points of the pts array.
extern void cvs_addPoints     (Tcvs*cvs, Cint2* pts, int nb);

// Add to cvs canvas the nb first segments of the lines array.
extern void cvs_addLines      (Tcvs*cvs, Csegm*lines, int nb);

// suppress the last segment stored into the cvs canvas.
extern void cvs_supLastLine   (Tcvs*cvs);

/****************************************************************************/
#endif // FILE_CANVAS_H
/****************************************************************************/
