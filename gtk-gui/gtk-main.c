#include <gtk/gtk.h>

#include "gui.h"
#include "canvas.h"

static Tint2 points[]    = { { 4,4}, {5,5} };
static Tsegm lines[]     = { { { 6,6}, {6,11} },  { { 6,6}, {11,6} },
                             { { 6,6}, {10,8} }, { { 2,4}, { 6,6} },
                             { { 6,6}, {10,4} }, { { 2,8}, { 6,6} },
};
static Tsegm linesHelp[] = { { { 1,1}, { 1,5} }, { { 1,2}, { 1, 6} },
                             { { 1,3}, { 1,7} }, { { 1,4}, { 1, 8} },
                             { { 1,5}, { 1,9} }, { { 1,6}, { 1,10} },
                             { { 1,1}, { 5,5} }, { { 1,1}, { 5, 1} },
};

int main()
{
    Taction action;
    Tsegm   line;
    Tgui* gui = gui_open(300,20);

    gui_addPoints(gui , points , TABLE_NB(points));
    gui_addLines (gui , lines  , TABLE_NB(lines));
    
    int fini=0;
    while ( ! fini ) {
        gui_redraw(gui);
        switch ( gui_getAction(gui,&line) ) {
          case GUI_ACT_Segment:
              if ( abs(line.p1.x-line.p2.x)>9||
                   abs(line.p1.y-line.p2.y)>9 )
                  gui_error(gui, "le segment (%d,%d) --> (%d,%d) est invalide",
                        line.p1.x,line.p1.x, line.p2.y,line.p2.y);
              else
                  gui_addLines(gui,&line,1);
              break;
          case GUI_ACT_Undo:
              gui_supLastLine(gui);
              break;
          case GUI_ACT_Help:
              if ( gui_getSegOfSet(gui,linesHelp, TABLE_NB(linesHelp), &line)==GUI_ACT_Quit)
                  goto end;
              gui_addLines(gui,&line,1);
              break;
          case GUI_ACT_Quit:
              fini = 1;
              break;
          default:
              fprintf(stderr,"unexpected action\n");
        }
    }

end:
    gui_close(gui);

    return 0;
}

