/****************************************************************************/
/** Software: GUI prototype dedicated to master-mind                       **/
/** Author  : Ivan Auge (Email: auge@ensiie.fr)                            **/
/****************************************************************************/
/** Handle a dialog window dedicated to master-mind                        **/
/**                                                                        **/
/**       +-----------------+----------+                                   **/
/**       +                 + btn undo +                                   **/
/**       +                 +----------+                                   **/
/**       +     canvas      + btn help +                                   **/
/**       +                 +----------+                                   **/
/**       +                 + btn quit +                                   **/
/**       +-----------------+----------+                                   **/
/**                                                                        **/
/****************************************************************************/
#ifndef FILE_GUI_H
#define FILE_GUI_H
/****************************************************************************/

#include "gui-top.h"

/****************************************************************************/
/* main gui interface                                                       */

typedef enum {  GUI_ACT_Undef=0, GUI_ACT_Segment, GUI_ACT_Undo,
                GUI_ACT_Help,    GUI_ACT_Quit } Taction;
typedef struct _Tgui  Tgui;

extern Tgui* gui_open  (int cvspixsize, int cvspixstep);
extern void  gui_close (Tgui* gui);
extern void  gui_redraw(Tgui* gui);

/**
 * The gui_getAction function runs the gui GUI until the user selects an
 * action. This last is returned and if he selects a line (GUI_ACT_Segment)
 * the selected line is returned into *segm.
 * 
 * The gui_getAction function runs the gui GUI until the user
 *   - selects a segment among lines[0], ... lines[lineNb-1] segment,
 *   - or he clicks the quit button.
 * The function returns GUI_ACT_Segment and the selected segment in *segm in the
 * first case. In the second case it returns GUI_ACT_Quit.
**/
extern Taction gui_getAction  (Tgui* gui, Tsegm*segm);
extern Taction gui_getSegOfSet(Tgui* gui, Tsegm*lines, int lineNb, Tsegm*segm);

/****************************************************************************/
/* points & lines interface                                                 */

/**
 * The gui_supPointsLines function suppresses all the points and lines
 * of the gui canvas.
 * The gui_addPoints function adds to the gui canvas the nb first points
 * of the pts array.
 * The gui_addLines  function adds to the gui canvas the nb first lines
 * of the pts array.
 * The cvs_supLastLine function suppressess the last line of to gui canvas.
 *
 * Theses functions do not redraw the gui canvas after adding or suppressing
 * the points and lines.
**/
extern void gui_supPointsLines (Tgui* gui);
extern void gui_addPoints      (Tgui* gui, Cint2* points, int pointNb);
extern void gui_addLines       (Tgui* gui, Csegm* lines , int lineNb);
extern void gui_supLastLine    (Tgui* gui);

/****************************************************************************/
/** Modal error dialog                                                     **/

// work like printf but it prints in a modal dialog window.
extern void gui_error(Tgui*gui, const char* fmt,...);

/****************************************************************************/
/** Interface to abstract gui structure                                    **/

extern void gui_setRetValues(Tgui*gui, Taction action, Tsegm segm);

/****************************************************************************/
#endif // FILE_GUI_H
/****************************************************************************/
/** vim: set textwidth=0:                                                  **/
