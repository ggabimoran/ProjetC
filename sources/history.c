#include "../headers/history.h"
#include "../headers/board.h"
#include "../headers/interface.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

/*@requires nothing
  @assigns nothing
  @ensures returns empty move*/
Move Move_create(void){
  return NULL;
}

/*@requires nothing
  @assigns nothing
  @ensures returns true if move is empty else false*/
bool Move_isEmpty(Move move){
  return (move==NULL);
}

/*@requires pMove not null
  @assigns pMove
  @ensures adds(with allocation) struct Coord of coordinates x,y to Move list*/
void Move_addM(Move* pMove,int x,int y){
  Move newMove=(Move)malloc(sizeof(Coord));
  newMove->x=x;
  newMove->y=y;
  newMove->previous=*pMove;
  *pMove=newMove;
}

/*@requires pMove not null
  @assigns pMove
  @ensures removes and frees allocated last added struct Coord of Move list*/
void Move_popM(Move* pMove){
  Move tmp=*pMove;
  *pMove=(*pMove)->previous;
  free(tmp);
  tmp=NULL;
}

/* @requires nothing
   @assigns nothing
   @ensures prints Move list */
void Move_print(Move move){
  int len = pMove_length(&move);
  Move currentMove=move;
  if(len > 0) {
    printf("[%d,%d]",currentMove->x,currentMove->y);
    currentMove=currentMove->previous;
  }
  if(len > 1) {
  while (!Move_isEmpty(currentMove)){
    printf("->[%d,%d]",currentMove->x,currentMove->y);
    currentMove=currentMove->previous;
  }
  }
  printf("\n");
}
/*@requires pMove not null
  @assigns nothing
  @ensures returns Move list length*/
int pMove_length(Move* pMove){
  Move current=*pMove;
  int counter=0;
  while (!Move_isEmpty(current)){
    current=current->previous;
    counter+=1;
  }
  return counter;
}
/*@requires pMove not null
  @assigns pMove
  @ensures frees allocated memory of Move list*/
void pMove_free(Move* pMove){
  Move move=*pMove;
  while (!Move_isEmpty(move)){
    Move_popM(&move); /*while move list isn't empty pop the first Move */
  }
}
/*@requires index of length 8 (this function is mainly used by lines_list and a point can belong to 8 lines maximum) 
  @assigns index
  @ensures searches for Move of coordinates x,y in Move list and assigns index to positions*/
bool Move_search(Move move,int x, int y,int index[]){
  Move current=move;
  int i=0;
  int line=0;
  while (!Move_isEmpty(current)){
    if (current->x==x && current->y==y){
      index[line]=i;
      line++;
    }
    i++;
    current=current->previous;
  }/* at this stage, index contains positions of all the occurences of Coord(x,y) in Move list */
  if (line > 0){
    return true;
  }
  return false;
}
/* history of points (played and saved)*/
static HistoryList history;
void initialize_HistoryList(){
    if(history.moves == 0) {
        history.moves=0;
        Move move=Move_create();
        history.PfirstMove=move;
        history.PlastPlayedMove=move;
        history.PlastSavedMove=move;
    }
}

size_t get_move_count(void){
    return history.moves;
}
/* history of alignements (all allignements are concatenated into one list) */
static LinesList lines;
void initialize_LinesList(){
    if(lines.n_lines == 0) {
        lines.n_lines=0;
        lines.lines_history=Move_create();
    }
}
/*@requires nothing
  @assigns nothing
  @ensures makes lines history accessible outside of history.c */
Move get_lines_history(void){
  return lines.lines_history;
}

/*@requires nothing
  @assigns nothing
  @ensures makes points history accessible outside of history.c */
Move get_points_history(void){
  return history.PlastPlayedMove;
}

/*@requires nothing
  @assigns nothing
  @ensures makes points saved history accessible outside of history.c */
Move get_points_saved_history(void){
  return history.PlastSavedMove;
}

/*@requires pboard not null
  @assigns pboard
  @ensures plays desired move, returning true if successful, else false */
bool play_move(Board* pboard,Coord coord, Error* error){
  if (!add_point(pboard,coord,error)){
    return false;
  }
  while (pMove_length(&history.PlastPlayedMove)!=pMove_length(&history.PlastSavedMove)){
    Move_popM(&history.PlastSavedMove);
  }
  Move_addM(&history.PlastPlayedMove,coord.x,coord.y);
  Move_addM(&history.PlastSavedMove,coord.x,coord.y);
  history.moves+=1;
  if (history.moves==1){ 
    history.PfirstMove=&coord;
  }
  if (coord.x==0 || coord.x==pboard->width-1 || coord.y==0 || coord.y==pboard->height-1){
    board_expansion(pboard,coord);
  }
  return true;
}

/*@requires pboard not null
  @assigns pboard
  @ensures cancels last played move by calling remove point*/
void cancel_move(Board* pboard, Error* error)
{
  Move cancelled_move=history.PlastPlayedMove;
  if (pMove_length(&history.PlastPlayedMove)==0){
      *error=CANCEL_ERR;
  }
  else{
    remove_point(pboard,*cancelled_move);
    Move_popM(&history.PlastPlayedMove);
    history.moves-=1;
    update_points_scored_val(-1);
  }
}

/*@requires pboard not null
  @assigns pboard
  @ensures replays last saved move */
void replay_move(Board* pboard, Error* error)
{
  Move cancelled_move=history.PlastSavedMove;
  if (pMove_length(&history.PlastPlayedMove)==pMove_length(&history.PlastSavedMove)){
    *error=REPLAY_ERR;
  }
  else{
    while (pMove_length(&history.PlastPlayedMove)+1!=pMove_length(&cancelled_move)){
      cancelled_move=cancelled_move->previous;
    }
    add_point(pboard,*cancelled_move, error);
    Move_addM(&history.PlastPlayedMove,cancelled_move->x,cancelled_move->y);
    history.moves+=1;
  }
}

/*@requires nothing
  @assings lines and points histories
  @ensures frees allocated memory of lines and points histories */
void free_history(void)
{
  Move moveHS=history.PlastSavedMove;
  Move moveHP=history.PlastPlayedMove;
  Move moveL=lines.lines_history;
  pMove_free(&moveHS);
  pMove_free(&moveHP);
  pMove_free(&moveL);
}

/*@requires pmove not null
  @assings pmove,lines history
  @ensures adds line to lines history, calls select line if more than 1 */
void add_line(Move* pmove){
  Move line=*pmove;
  if (pMove_length(&line)>5){
    select_line(&line);
  }
  while(!Move_isEmpty(line)){
    Move_addM(&lines.lines_history,line->x,line->y);
    Move_popM(&line);
  }
  lines.n_lines+=1;
    update_points_scored();
}

/*@requires line1 and line2 not null AND BOTH LISTS OF LENGTH AT LEAST 5
  @assigns nothing
  @ensures returns true if no more than one common move in both lines, else false*/
bool no_more_than_one_move_in_two_lines(Move* line1,Move* line2){
  int i,j;
  int same_move=0;
  Move current1=*line1;
  Move current2=*line2;
  for (i=0;i<5;i++){/*loop on first list */
    current2=*line2;
    for (j=0;j<5;j++){/*loop on second list */
      if (current1->x == current2->x && current1->y == current2->y){
	same_move+=1;
	if (same_move > 1){
	  return false;
	}
      }
      current2=current2->previous;
    }
    current1=current1->previous;
  }
  return true;
}

/*@requires index of length 8 initialized with -1 values
  @assigns index
  @ensures index contains positions of lines containing move in lines history */
void line_numbers_of_Move(Move move,int index[]){ /* <- function mainly useful for remove_line */
  int x=move->x;
  int y=move->y;
  int i;
  Move_search(lines.lines_history,x,y,index);/*at this stage index contains positions of move in lines history */
  for (i=0;i<8;i++){
    if (index[i]!=-1){
      index[i]=(int)index[i]/5;
    }
  }/* at this stage index contains positions of lines containing move in lines history */
}

/*@requires cand_line not null and of length at least 5
  @assigns nothing
  @ensures returns true if candidate line has no more than one move in common with other lines, else false */
bool candidate_line(Move* cand_line){
  int i;
  Move current=lines.lines_history;
  while (!Move_isEmpty(current)){/*loop on lines history to compare with each line of lines history */
    if(!no_more_than_one_move_in_two_lines(cand_line,&current)){
      return false;
    }
    for (i=0;i<5;i++){
      current=current->previous;
    }
  }
  return true;
}

/*@requires nothing
  @assigns lines history
  @ensures removes all lines containing move from lines history */
void remove_lines(Move move){
  int index[8]={-1,-1,-1,-1,-1,-1,-1,-1};
  line_numbers_of_Move(move,index);
  int i,j;
  int counter=0;
  Move current=lines.lines_history;
  Move next=current;
  for (i=0;i<4;i++){
    if (index[i]!=-1){
      while (!Move_isEmpty(current) && counter<5*(index[i]-i)){
	counter++;
	next=current;
	current=current->previous;
      }
      if (counter==0){
	for (j=0;j<5;j++){
	  Move_popM(&lines.lines_history);
	}
      }
      else{
	Move* Pcurrent=&current;
	for (j=0;j<5;j++){
	  Move_popM(Pcurrent);
	}
	next->previous=*Pcurrent;
      }
    }
  }
}

void initialize_HistoryList_from_string(char* s) {
    char* pch;

    pch = strtok (s,",|");
    bool xySwitch = true;
    int x, y;
    int count = 0;
    Move move;
    while (pch != NULL)
    {
        if(count % 10 == 0)
            move = Move_create();

        if(xySwitch) {
            x = atoi(pch);
        }
        else {
            y = atoi(pch);
            Move_addM(&move, x, y);
        }
        xySwitch = !xySwitch;
        count++;
        if (count % 10 == 0) {
            add_line(&move);
        }

        pch = strtok (NULL, ",|");
    }
}

void translate_history_x_axis(void){
  Move currentPS=history.PlastPlayedMove;
  Move currentPP=history.PlastSavedMove;
  Move currentL=lines.lines_history;
  while (!Move_isEmpty(currentL)){
    currentL->x+=1;
    currentL=currentL->previous;
  }
  while (!Move_isEmpty(currentPP)){
    currentPP->x+=1;
    currentPP=currentPP->previous;
  }
  while (!Move_isEmpty(currentPS)){
    currentPS->x+=1;
    currentPS=currentPS->previous;
  }
}

void translate_history_y_axis(void){
  Move currentPS=history.PlastPlayedMove;
  Move currentPP=history.PlastSavedMove;
  Move currentL=lines.lines_history;
  while (!Move_isEmpty(currentL)){
    currentL->y+=1;
    currentL=currentL->previous;
  }
  while (!Move_isEmpty(currentPP)){
    currentPP->y+=1;
    currentPP=currentPP->previous;
  }
  while (!Move_isEmpty(currentPS)){
    currentPS->y+=1;
    currentPS=currentPS->previous;
  }
}

void board_expansion_history_translation(Coord coord){
  if (coord.x==0 && coord.y==0){
    translate_history_x_axis();
    translate_history_y_axis();
  }
  else if (coord.x==0){
    translate_history_x_axis();
  }
  else if (coord.y==0){
    translate_history_y_axis();
  }
}
