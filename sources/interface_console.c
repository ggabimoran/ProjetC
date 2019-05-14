#include "../headers/history.h"
#include "../headers/interface.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <linux/limits.h>

/* Colors and effects support */
#define RESET   "\033[0m"
#define RED     "\033[31m"              /* Red */
#define BOLDRED     "\033[1m\033[31m"   /* Bold Red */
#define LIGHTYELLOW "\033[93m"          /* Light Yellow */
#define LIGHTGREEN   "\033[92m"         /* Light Green */
#define BLINK   "\033[5m"               /* Blinking effect */

static bool hint;


void set_hint(bool boolean){
  hint=boolean;
}

bool get_hint(){
  return hint;
}

void set_fullscreen(bool set) {
    if (set)
        system("wmctrl -r ':ACTIVE:' -b add,fullscreen &> /dev/null");
    else
        system("wmctrl -r ':ACTIVE:' -b remove,fullscreen &> /dev/null");
}

/*@requires pboard not null
  @assigns nothing
  @ensures prints board, shows possible moves if hint set to true */
void print_board(Board* pboard)
{
  int width = pboard->width;
  int height = pboard->height;
  int indexLines[8] = {-1,-1,-1,-1,-1,-1,-1,-1};
  int i, j;
  Move possible_moves=Move_create();
  if (hint){
    get_valid_moves(pboard,&possible_moves);
  }
  /* print header (y coordinates) */
  printf("\t\t\t\t Points scored :  %d \n\n",get_points_scored());
  printf("  ");
  for(i = 0 ; i < width ; i++) {
    /* Header not displayed correctly if size > 99
     * Accepted because a board should not be as big.
     * A game on a board this big would be way too
     * long anyway.
     */
    if(i < 10)
      printf(" %d ", i);
    else {
      printf(" %d", i);
    }
  }
  printf("\n");
  /* print x coordinates and board */
  for(i = 0 ; i < height ; i++) {
    /* print x coordinates */
    if(i < 10)
      printf("%d ", i);
    else
      printf("%d", i);

    for(j = 0 ; j < width ; j++) {

      /* print board line */
      printf("[");
      if(pboard->points[i][j]) {
	if (Move_search(get_lines_history(),j,i,indexLines)) {
	  printf(LIGHTGREEN "x" RESET);
	} else {
	  printf("\u2022");
	}
      }
      else if (hint && Move_search(possible_moves,j,i,indexLines)){
	printf(BLINK LIGHTYELLOW "*" RESET);
      }
      else {
	printf(" ");
      }
      printf("]");
    }
    printf("\n");
  }
}

Coord select_move(void)
{
	int x,y;
	printf("Type the point coordinates [x,y] (type x <space> y <enter>) :\n");
	int succAffect = -1; /* number of successful affectations */
	while(succAffect != 2) {
	    printf(":> ");
        succAffect = scanf(" %d%d", &x, &y);
	    if (succAffect != 2) {
            print_error(WRONG_INPUT_ERR);
        }
	    empty_input_buffer();
    }
	Coord coord;
	coord.x=x;
	coord.y=y;
	coord.previous=NULL;
	return coord;
}


enum action select_action(void)
{
    printf("\nSelect : Play move [p] / Cancel previous move [c] / Replay cancelled move [r]\n         List valid moves [l] / Ask help [h] / Quit game [q]\n");
	char c;
	while (true) {
        printf(":> ");
        scanf(" %c",&c);
	    /*c = getchar();*/
	    while('\n'!=getchar()); /* only capture the first character */
	    c = tolower(c);
	    switch(c) {
            case 'p':
                return PLAY_MOVE;
            break;
            case 'c':
                return CANCEL_MOVE;
            break;
            case 'r':
                return REPLAY_MOVE;
            break;
            case 'l':
                return LIST_MOVES;
            break;
            case 'h':
                return ASK_HELP;
            break;
	        case 'q':
		        return QUIT_GAME;
	        break;
	        default:
	            print_error(WRONG_INPUT_ERR);
                break;
            }
    }
}

void print_help(void)
{
	printf("------- HELP -------\n\n");

	printf("> The goal is to add points on the board.\n");
	printf("> A point can be added to the board if it creates a line of 5 points vertically, horizontally or diagonally.\n");
	printf("> Two lines can't have more than one point in common.\n\n");
    press_enter_to_continue();
}

/*@requires pmove not null
  @assings pmove
  @ensures asks player to chose amongst possible lines and frees all those not chosen*/
void select_line(Move* pmove){
  int choice;
  Move current=*pmove;
  int i=0;
  printf("List of possible moves :\n");
  printf("1 : ");
  printf("[%d,%d]",current->x,current->y);
  current=current->previous;
  i++;

  while(!Move_isEmpty(current)){
    if (i%5==0){
      printf("\n");
      printf("%d : ",(i/5)+1);
      printf("[%d,%d]",current->x,current->y);
    } else {
	  printf("->[%d,%d]",current->x,current->y);
    }
    current=current->previous;
    i++;
  }
  printf("\nChoose your line.\n");
  do{
  	scanf("%d",&choice);
  }while(0 > choice || choice > (i/5));
  i=0;
  current=*pmove;
  for (i=0;i<(choice-1)*5;i++){
    Move_popM(&current);
  }
  Move current2=current;
  for (i=0;i<4;i++){
    current2=current2->previous;
  }
  Move tmp=current2->previous;
  current2->previous=NULL;
  pMove_free(&tmp);
  printf("You have chosen:\n");
  Move_print(current);
  *pmove=current;
}

bool confirm_quit_save(Board* pboard) {
    char quit;
    do {
        printf("Do you really want to exit the game? (y/n)\n");
        scanf(" %c", &quit);
        if (quit != 'y' && quit != 'n')
            printf("\nWrong input.\n");

    } while (quit != 'y' && quit != 'n');

    if(quit == 'y') {
        char save;
        do {
            printf("Do you want to save the current board? (y/n)\n");
            scanf(" %c", &save);
            if (save != 'y' && save != 'n')
                printf("\nWrong input.\n");

        } while (save != 'y' && save != 'n');

        if (save == 'y')
            save_board(pboard);
        return true;
    }
    else
        return false;
}


char* ask_savefile_name(void){
    char* fileName = (char*)malloc(sizeof(char)*MAX_INPUT);
    printf("What name does the file should have ? :> ");
    scanf(" %s", fileName);
    return fileName;
}


void display_logo(void){
    char resolved_path[PATH_MAX];
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    realpath("assets/logo", resolved_path);
    fp = fopen(resolved_path, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);
    while ((getline(&line, &len, fp)) != -1) {
        printf("%s", line);
    }
    fclose(fp);
    if (line)
        free(line);
    press_enter_to_continue();
    clear_screen();
}

void clear_screen(void){
    system("clear");
}

void press_enter_to_continue(void){
    printf("\n\n \t\t\t\t =================== Press enter to continue ===================\n");
    while (true) {
        char c=getchar();
        if (c=='\n' || c==EOF)
            break;
    }
}

void print_error(Error err) {
    switch (err) {
        case WRONG_INPUT_ERR:
            fprintf(stderr, RED "Wrong input.\n" RESET);
            break;
        case FILE_PTR_ERR:
            fprintf(stderr, BOLDRED "File could not be opened.\n" RESET);
            break;
        case FILE_DIMENSION_ERR:
            fprintf(stderr, BOLDRED "File error: board width must be equal for each line.\n" RESET);
            break;
        case FILE_UNKNOWN_CHAR_ERR:
            fprintf(stderr, BOLDRED "File error: unknown character.\n" RESET);
            break;
        case POINT_ALREADY_EXIST_ERR:
            fprintf(stderr, BOLDRED "This point already exists.\n" RESET);
            break;
        case INVALID_COORDINATES_ERR:
            fprintf(stderr, BOLDRED "Coordinates are invalid.\n" RESET);
            break;
        case ALIGNMENT_ERR:
            fprintf(stderr, BOLDRED "No valid alignment.\n" RESET);
            break;
        case CANCEL_ERR:
            fprintf(stderr, BOLDRED "Cannot cancel: no move to cancel.\n" RESET);
            break;
        case REPLAY_ERR:
            fprintf(stderr, BOLDRED "Cannot replay: no move has been cancelled.\n" RESET);
            break;
        default:
            fprintf(stderr, RED "Undefined error (requires implementation)." RESET);
            exit(EXIT_FAILURE);
            break;

        /* TODO: press_enter_to_continue() should be called be functions calling print_error */
    }
}

void print_game_over(void) {
    printf("No more point to be played: the game is over!\n");
}

void print_score(void) {
    printf("Your final score is : %d\n", get_points_scored());
}