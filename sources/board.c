#include "../headers/board.h"
#include "../headers/history.h"
#include "../headers/interface.h"
#include "../headers/utils.h"

#include <stdlib.h>
#include <time.h>
#include <linux/limits.h>
#include <string.h>


static int points_scored;
static const char* saving_path = "boards/saves/";

void update_points_scored(void){
    points_scored = get_move_count()+1;
}

void update_points_scored_val(int val){
    points_scored += val;
}

int get_points_scored(void){
    return points_scored;
}


/*@requires width and height greater than 0
  @assigns board
  @ensures return empty board (all points set to NULL) */
Board create_empty_board(int width,int height){
  Board board;
  board.width=width;
  board.height=height;
  /*allocated memory for points*/
  board.points = (Ppoint**)malloc(board.height*sizeof(Ppoint*));
  int i;
  for (i=0;i<board.height;i++){
    board.points[i] = (Ppoint*)malloc(board.width*sizeof(Ppoint));
  }
  i=0;
  int j;
  for (i=0;i<board.height;i++){
    for (j=0;j<board.width;j++){
      board.points[i][j]=NULL; /*all points set to NULL */
    }
  }
  return board;
}

/*@requires pboard not NULL
  @assigns pboard
  @ensures frees allocated memory of board */
void free_board(Board* pboard){
  int i;
  for (i=0;i<pboard->height;i++){
    free(pboard->points[i]);
    pboard->points[i]=NULL;
  }
  free(pboard->points);
  pboard->points=NULL;
}

/*@requires pboard not null
  @assigns pboard
  @ensures calls is_move_valid, allocates memory for square, sets it to 1 and adds associated line*/
bool add_point(Board* pboard, Coord coord, Error* error){
  int i=coord.y;
  int j=coord.x;
  Move move=Move_create(); /* <- becomes list of possible moves after is_move_valid is called */
  if (is_move_valid(pboard,coord,&move,error)){
    pboard->points[i][j] = (Ppoint)malloc(sizeof(int));
    *(pboard->points[i][j])=1;
    add_line(&move);/* adds line to line history, calls select_line if there is more than 1 possible line*/
    return true;
  }
  else{
    return false;
  }
}

/*@requires pboard not null,point of coord exists in board
  @assigns pboard->points[i][j]
  @ensures frees allocated memory of point of coordinates coord and removes associates line*/
void remove_point(Board* pboard,Coord coord){
  int j=coord.x;
  int i=coord.y;
  free(pboard->points[i][j]);/* free point from board */
  pboard->points[i][j]=NULL;
  remove_lines(&coord); /* remove all lines containing coord in lines history */
}

/*@requires pboard not null
  @assigns pboard
  @ensures frees all points from board */
void remove_points(Board* pboard){
  int i;
  int j;
  for (i=0;i<pboard->height;i++){
    for (j=0;j<pboard->width;j++){
      if (pboard->points[i][j]){
	free(pboard->points[i][j]);
	pboard->points[i][j]=NULL;
      }
    }
  }
}

bool initialize_file(Board* pboard, char* path, Error* error)
{
    FILE *fp = fopen(path, "r");
    if(!check_file(fp, error)) {
        return false;
    }
    size_t width = get_file_board_width(fp);
    size_t height = get_file_board_height(fp);
    *pboard = create_empty_board(width, height);

    size_t line = 0, col = 0;
    char* lineBuffer;
    size_t bufferSize;
    fseek(fp, 0, SEEK_SET);
    while(getline(&lineBuffer, &bufferSize, fp) != EOF) {
        if(strcmp(lineBuffer, "====\n") == 0) {
            getline(&lineBuffer, &bufferSize, fp);
            initialize_HistoryList_from_string(lineBuffer);
            break;
        }
        for(col = 0 ; col < width ; col++) {
            if(lineBuffer[col] == 'o') {
                Ppoint point = NULL;
                point = (int*)malloc(sizeof(int));
                pboard->points[line][col] = point;
            }
        }
        line++;
    }
    points_scored = 0;
    free(lineBuffer);
    fclose(fp);
    return true;
}

/*@requires pboard,pvalid_points not null
  @assigns pvalid_points
  @ensures pvalid_points points to a list of all the possible moves to be played */
void get_valid_moves(Board* pboard,Move* pvalid_points)
{
  int i;
  int j;
  Error error;/* needs to be called by is_move_valid */
  Move valid_points=*pvalid_points;
  Move valid_moves=Move_create();/* list of available alignements : needs to be called by is_move_valid */
  Coord coord_temp;
  for(i=0 ; i < pboard->height ; i++){
    for(j=0 ; j < pboard->width ; j++){
      coord_temp.x = j;
      coord_temp.y = i;
      if(is_move_valid(pboard,coord_temp,&valid_moves,&error)){
	pMove_free(&valid_moves);/*free list of available alignements */
	Move_addM(&valid_points,coord_temp.x,coord_temp.y);
      }
    }
  }
  *pvalid_points=valid_points;
}

/*@requires pboard not null
  @assigns pMove
  @ensures tests if point is valid -> see subsequent functions for details on conditions */
bool is_move_valid(Board* pboard,Coord coord,Move* pMove,Error* error){
  if (!is_move_in_board(pboard,coord)){
    *error=INVALID_COORDINATES_ERR;
    return false;
  }
  if (is_move_exists_already(pboard,coord)){
    *error=POINT_ALREADY_EXIST_ERR;
    return false;
  }
  Move candidate_lines=Move_create();
  horizontal_search(&candidate_lines,coord,pboard);/*search in each direction */
  vertical_search(&candidate_lines,coord,pboard);
  NE_diagonal_search(&candidate_lines,coord,pboard);
  NW_diagonal_search(&candidate_lines,coord,pboard);
  if (Move_isEmpty(candidate_lines)){
    *error=ALIGNMENT_ERR;
    return false;
  }
  *pMove=candidate_lines; /*at this stage *pMove is the list of all possible lines */
  return true;
}

/*@requires pboard not null
  @assigns nothing
  @ensures returns true if point of coordinates coord is in board, else false*/
bool is_move_in_board(Board* pboard,Coord coord){
  int x=coord.x;
  int y=coord.y;
  if (x >= 0 && y>= 0 && x < pboard->width && y < pboard->height){
    return true;
  }
  return false;
}

/*@requires pboard not null
  @assigns nothing
  @ensures returns true if point of coordinates coord exists already, else false */
bool is_move_exists_already(Board* pboard,Coord coord){
  int x=coord.x;
  int y=coord.y;
  if (pboard->points[y][x]){
    return true;
  }
  return false;
}

/*@requires pboard,pcand_lines not null
  @assigns pcand_lines
  @ensures searches horizontally for hypothetical lines, calls candidate_line to test if they are possible, then adds them to pcand_lines*/
void horizontal_search(Move* pcand_lines,Coord coord,Board* pboard){
  Move cand_lines=*pcand_lines;
  int i=coord.y;
  int test=coord.x;
  int j;
  int counter=1;
  for (j=test+1;j<test+5;j++){
    if (!(j<pboard->width && pboard->points[i][j])){
      break;
    }
    counter++;
  }/*searching for points on the horizontal right*/
  j=test;
  int firstpoint=test; /* <- useful to determine number of lines possible */
  for (j=test-1;j>test-5;j--){
    if (!(j>=0 && pboard->points[i][j])){
      break;
    }
    counter++;
    firstpoint=j;
  }/*searching for points on the horizontal left*/
  if (counter >= 5){
    j=firstpoint;
    int k=firstpoint;
    for (firstpoint=k;firstpoint<=k+counter-5;firstpoint++){/* for each firstpoint (every possible line)*/
      for (j=firstpoint;j<firstpoint+5;j++){/* add line to cand_lines */
	Move_addM(&cand_lines,j,i); 
      }
      if (!candidate_line(&cand_lines)){ /*if candidate line has more than one point in common with another line remove it*/
	j=0;
	for (j=0;j<5;j++){
	  Move_popM(&cand_lines);
	}
      }
    }
  }
  *pcand_lines=cand_lines;/*updates list of possible lines*/
}

/*@requires pboard,pcand_lines not null
  @assigns pcand_lines
  @ensures searches vertically for hypothetical lines, calls candidate_line to test if they are possible, then adds them to pcand_lines*/
void vertical_search(Move* pcand_lines,Coord coord,Board* pboard){
  Move cand_lines=*pcand_lines;
  int j=coord.x;
  int test=coord.y;
  int i;
  int counter=1;
  for (i=test+1;i<test+5;i++){
    if (!(i<pboard->height && pboard->points[i][j])){
      break;
    }
    counter++;
  }/*searching for points on the downoard vertical */
  i=test;
  int firstpoint=test; /* <- useful to determine number of lines possible */
  for (i=test-1;i>test-5;i--){
    if (!(i>=0 && pboard->points[i][j])){
      break;
    }
    counter++;
    firstpoint=i;
  }/*searching for points on the upwoard vertical */
  if (counter >= 5){
    i=firstpoint;
    int k=firstpoint;
    for (firstpoint=k;firstpoint<=k+counter-5;firstpoint++){/* for each firstpoint (every possible line)*/
      for (i=firstpoint;i<firstpoint+5;i++){/* add line to cand_lines */
	Move_addM(&cand_lines,j,i);
      }
      if (!candidate_line(&cand_lines)){ /*if candidate line has more than one point in common with another line remove it*/
	i=0;
	for (i=0;i<5;i++){
	  Move_popM(&cand_lines);
	}
      }
    }
  }
  *pcand_lines=cand_lines;/*updates list of possible lines*/
}

/*@requires pboard,pcand_lines not null
  @assigns pcand_lines
  @ensures searches on the north east diagonal for hypothetical lines, calls candidate_line to test if they are possible, then adds them to pcand_lines*/
void NE_diagonal_search(Move* pcand_lines,Coord coord,Board* pboard){
  Move cand_lines=*pcand_lines;
  int testx=coord.x;
  int testy=coord.y;
  int i;
  int counter=1;
  for (i=1;i<5;i++){
    if (!(testx+i<pboard->width && testy-i>=0 && pboard->points[testy-i][testx+i])){
      break;
    }
    counter++;
  }/*searching for points on the north east diagonal */
  i=0;
  int firstpoint=0;/* <- useful to determine number of lines possible */
  for (i=1;i<5;i++){
    if (!(testy+i<pboard->height && testx-i>=0 && pboard->points[testy+i][testx-i])){
      break;
    }
    counter++;
    firstpoint=i;
  }/*searching for points on the south west diagonal */
  if (counter >= 5){
    i=firstpoint;
    int k=firstpoint;
    for (firstpoint=k;firstpoint>=k-(counter-5);firstpoint--){/* for each firstpoint (every possible line)*/
      for (i=firstpoint;i>firstpoint-5;i--){/* add line to cand_lines */
	Move_addM(&cand_lines,testx-i,testy+i);
      }
      if (!candidate_line(&cand_lines)){/*if candidate line has more than one point in common with another line remove it*/
	i=0;
	for (i=0;i<5;i++){
	  Move_popM(&cand_lines);
	}
      }
    }
  }
  *pcand_lines=cand_lines;/*updates list of possible lines*/
}

/*@requires pboard,pcand_lines not null
  @assigns pcand_lines
  @ensures searches on the north west diagonal for hypothetical lines, calls candidate_line to test if they are possible, then adds them to pcand_lines*/
void NW_diagonal_search(Move* pcand_lines,Coord coord,Board* pboard){
  Move cand_lines=*pcand_lines;
  int testx=coord.x;
  int testy=coord.y;
  int i;
  int counter=1;
  for (i=1;i<5;i++){
    if (!(testx-i>=0 && testy-i>=0 && pboard->points[testy-i][testx-i])){
      break;
    }
    counter++;
  }/*searching for points on the north west diagonal */
  i=0;
  int firstpoint=0;/* <- useful to determine number of lines possible */
  for (i=1;i<5;i++){
    if (!(testy+i<pboard->height && testx+i<pboard->width && pboard->points[testy+i][testx+i])){
      break;
    }
    counter++;
    firstpoint=i;
  }/*searching for points on the south east diagonal */
  if (counter >= 5){
    i=firstpoint;
    int k=firstpoint;
    for (firstpoint=k;firstpoint>=k-(counter-5);firstpoint--){/* for each firstpoint (every possible line)*/
      for (i=firstpoint;i>firstpoint-5;i--){/* add line to cand_lines */
	Move_addM(&cand_lines,testx+i,testy+i);
      }
      if (!candidate_line(&cand_lines)){/*if candidate line has more than one point in common with another line remove it*/
	i=0;
	for (i=0;i<5;i++){
	  Move_popM(&cand_lines);
	}
      }
    }
  }
  *pcand_lines=cand_lines;/*updates list of possible lines*/
}

/*@requires nothing
  @assigns board
  @ensures creates empty board and fills it with random distribution of points*/
Board initialize_rand(void)
{
  int width = get_random_number(10, 15);
  int height = get_random_number(10, 15);
  Board board=create_empty_board(width,height);
  int i, j;
  int random;
  for (i = 1 ; i < height-1 ; i++)
  {
    for (j = 1 ; j < width-1 ; j++)
    {
      random = get_random_number(0, 100);
      /* 50% chance to add a point */
      if (random < 30)
      {
        Ppoint point = malloc(sizeof(int));
        board.points[i][j] = point;
      }
    }
  }
  return board;
}

/*@requires pboard not null
  @assigns pboard
  @ensures executes action of the enum action action*/
void execute_action(Board* pboard, enum action action, bool* quit, Error* error)
{
    if (action == PLAY_MOVE){
        Coord coord=select_move();
        play_move(pboard,coord,error);
    } else if (action == CANCEL_MOVE){
        cancel_move(pboard,error);
    } else if (action == REPLAY_MOVE){
        replay_move(pboard,error);
    } else if (action == LIST_MOVES){
        /*list_available_moves(pboard);  NOT NEEDED */
      set_hint(true);
    }else if (action == ASK_HELP){
        print_help();
    } else if (action == QUIT_GAME){
        *quit = confirm_quit_save(pboard);
    } else{
        fprintf(stderr, "Execute action: action is undefined.");
        exit(EXIT_FAILURE);
    }
}

/*@requires pboard not null
  @assigns nothing
  @ensures returns true if game is over(if no moves available), else false*/
bool is_game_over(Board* pboard)
{
  Move possible_moves=Move_create();
  get_valid_moves(pboard,&possible_moves);
  if (pMove_length(&possible_moves)==0){ /* if no moves available */
    pMove_free(&possible_moves);/* deallocate memory of possible moves */
    return true;
  }
  pMove_free(&possible_moves);/* deallocate memory of possible moves */
  return false;
}

void save_board(Board* pboard) {
    char resolved_path[PATH_MAX];
    char* fileName = ask_savefile_name();
    char path[PATH_MAX];
    strcpy(path,saving_path);
    strcat( path, fileName );
    free(fileName); /* No longer needed */
    realpath(path, resolved_path);
    FILE *fp = fopen(resolved_path, "w");
    int line, col;
    for(line = 0 ; line < pboard->height ; line++) {
        for(col = 0 ; col < pboard->width ; col++) {
            if (pboard->points[line][col]) {
                fputc('o', fp);
            } else {
                fputc('.', fp);
            }
        }
        fputc('\n', fp);
    }
    /* Save lines */
    fputs("====\n", fp);
    Move history = get_lines_history();
    int len = pMove_length(&history);
    Move currentMove = history;
    char buffer[16] = {0};
    if(len > 0) {
        sprintf(buffer, "%d,%d", currentMove->x, currentMove->y);
        fputs(buffer, fp);
        currentMove=currentMove->previous;
    }
    if(len > 1) {
        while (!Move_isEmpty(currentMove)){
            sprintf(buffer, "|%d,%d", currentMove->x, currentMove->y);
            fputs(buffer, fp);
            currentMove=currentMove->previous;
        }
    }
    fputc('\n', fp);

    fclose(fp);
}

void board_expansion(Board* pboard,Coord coord){
  int i,j;
  int dx,dy;
  Board board=*pboard;
  Board board_copy;
  board_expansion_history_translation(coord);
  if (coord.x>0 && coord.x<board.width-1){
    board_copy=create_empty_board(board.width,board.height+1);
    dx=0;
    if (coord.y==0){
      dy=1;
    }
    else{
      dy=0;
    }
  }
  else if (coord.y>0 && coord.y<board.height-1){
    board_copy=create_empty_board(board.width+1,board.height);
    dy=0;
    if (coord.x==0){
      dx=1;
    }
    else{
      dx=0;
    }
  }
  else{
    board_copy=create_empty_board(board.width+1,board.height+1);
    if (coord.x==0){
      if (coord.y==0){
	dx=1;
	dy=1;
      }
      else{
	dx=1;
	dy=0;
      }
    }
    if (coord.x==board.width-1){
      if (coord.y==0){
	dx=0;
	dy=1;
      }
      else{
	dx=0;
	dy=0;
      }
    }
  }
  for (i=0;i<board.height;i++){
    for (j=0;j<board.width;j++){
      if (board.points[i][j]){
	board_copy.points[i+dy][j+dx] = (Ppoint)malloc(sizeof(int));
        *(board_copy.points[i+dy][j+dx])=1;
      }
    }
  }
  remove_points(&board);
  free_board(&board);
  *pboard=board_copy;
}
