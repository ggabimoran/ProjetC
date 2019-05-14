#ifndef HISTORY_H
#define HISTORY_H

/**
 * \file history.h
 * \brief Records player moves during a game.
 * \authors Geoffrey DELVAL, Rémi GUIJARRO ESPINOSA, Gabriel MORAN, Clément PERESSE
 * \version lot_b
 * \date 04.2019
 *
 * Represents board.h upper layer. 
 * This class records player moves and allows to cancel or replay a move.
 *
 */

#include "board.h"

#include <stdbool.h>
#include <stddef.h>


/**
 * \struct HistoryList
 * \brief Linked list of previously played moves.
 *
 * PlastPlayedMove and PlastSavedMove may hold different values:
 * After the player cancels a move, PlastSavedMove still points to the
 * same value while PlastPlayedMove points to the 'previous' move.
 * This allows to replay a canceled move.
 *
 * It is intended to be used as a static variable inside the history source file.
 */
typedef struct{
	size_t moves; /**< Number of played moves since the beginning of the game */
	Move PfirstMove; /**< Pointer to first move */
	Move PlastPlayedMove; /**< Pointer to last played move */
	Move PlastSavedMove; /**< Pointer to last saved move (i.e. move before a canceled move) */
} HistoryList; /* PlastPlayedMove and PlastSavedMove are two seperate lists : allows cancelling and replaying consecutively multiple times */

/**
 * \struct LinesList
 * \brief Linked list of previously formed alignments.
 *
 * Holds all moves forming alignments. Every 5 moves represents an alignment.
 *
 * It is intended to be used as a static variable inside the history source file.
 */
typedef struct{
  size_t n_lines;
  Move lines_history;
} LinesList;

/**
 * \fn bool play_move(Board* pboard, Coord coord)
 * \brief Play a move, i.e. add a point on the Board.
 *
 * \param pboard Pointer to the Board being played.
 * \param coord Coordinates of the point to be played.
 * \return true if point successfully added, false otherwise.
 */
bool play_move(Board* pboard, Coord coord, Error* error);

/**
 * \fn void cancel_move(Board* pboard)
 * \brief Withdraw a point from the Board.
 *
 * \param pboard Pointer to the Board being played.
 */
void cancel_move(Board* pboard, Error* error);

/**
 * \fn void replay_move(Board* pboard)
 * \brief Replay a previously canceled move.
 *
 * \param pboard Pointer to the Board being played.
 */
void replay_move(Board* pboard,Error* error);

/**
 * \fn void free_history();
 * \brief Free allocated memory of static history list
 */
void free_history(void);

/**
 * \fn Move Move_create();
 * \brief Create an empty Move.
 */
Move Move_create();

/**
 * \fn bool Move_isEmpty(Move move);
 * \brief Check if a Move is empty.
 *
 * \param move Move to check.
 */
bool Move_isEmpty(Move move);

/**
 * \fn void Move_addM(Move* pMove,int x,int y);
 * \brief Add Move to pMove by allocating memory on heap.
 *
 * \param pMove Pointer to Move.
 * \param x X coordinate.
 * \param y Y coordinate.
 */
void Move_addM(Move* pMove, int x, int y);

/**
 * \fn void Move_popM(Move* pMove);
 * \brief Remove last Move from pMove.
 *
 * pMove points to previous Move and frees allocated memory.
 *
 * \param pMove Pointer to Move.
 */
void Move_popM(Move* pMove);

/**
 * \fn int pMove_length(Move* pMove);
 * \brief Get pMove length.
 *
 * \param pMove Pointer to Move.
 * \return pMove length.
 */
int pMove_length(Move* pMove);

/**
 * \fn void pMove_free(Move* pMove);
 * \brief Free all allocated memory of pMove
 *
 * \param pMove Pointer to Move.
 */
void pMove_free(Move* pMove);

/**
 * \fn void Move_print(Move move);
 * \brief Print a Move list
 *
 * \param move Move to be printed.
 */
void Move_print(Move move);

/**
 * \fn bool Move_search(Move move, int x, int y, int index[]);
 * \brief Search for Move of coordinates x,y in Move list and assigns index to positions.
 *
 * \param move Move to be searched.
 * \param x X coordinate to be searched.
 * \param y Y coordinate to be searched.
 * \param index Array of length 8.
 * \return Ordered index list of element position of length 4 (a point can belong to a maximum 4 lines)
*/  
bool Move_search(Move move, int x, int y, int index[]);

/**
 * \fn void initialize_HistoryList(void);
 * \brief Initialize HistoryList linked list.
 */
void initialize_HistoryList(void);

/**
 * \fn Move get_lines_history(void);
 * \brief Get LinesList static list.
 *
 * Allows for LineList to be accessible outside of history.
 *
 * \return A pointer to the first Coord of the line history list.
 */
Move get_lines_history(void);

/**
 * \fn Move get_points_history(void);
 * \brief Get history of points from PlastPlayedMove.
 *
 * Allows for points history to be accessible outside of history.
 *
 * \return The PlastPlayedMove
 */
Move get_points_history(void);

/**
 * \fn Move get_points_saved_history(void);
 * \brief Get history of points from PlastSavedMove.
 *
 * Allows for points history to be accessible outside of history.
 *
 * \return The PlastSavedMove
 */
Move get_points_saved_history(void);

/**
 * \fn void initialize_LinesList(void);
 * \brief Initialize LinesList linked list.
 */
void initialize_LinesList(void);

/**
 * \fn void add_line(Move* pmove)
 * \brief Add a line to LinesList.
 *
 * \param pmove Pointer to Move.
 */
void add_line(Move* pmove);

/**
 * \fn void line_numbers_of_Move(Move move,int index[]);
 * \brief Ensure that index contains positions of lines containing move in lines history.
 *
 * \param move Move
 * \param index Array of length 8 initialized with -1 values.
 */
void line_numbers_of_Move(Move move,int index[]);

/**
 * \fn bool no_more_than_one_move_in_two_lines(Move* line1,Move* line2);
 * \brief Check that no more than two lines can be added.
 *
 * \param line1 Line to be checked.
 * @param line2 Line to be checked.
 * @return True if no more than one common move in both lines, false otherwise.
 */
bool no_more_than_one_move_in_two_lines(Move* line1,Move* line2);

/**
 * \fn bool candidate_line(Move* cand_line);
 * \brief Check if candidate line has more than one move in common with other lines.
 *
 * \param cand_line Candidate line
 * \return True if candidate line has no more than one move in common with other lines, false otherwise.
 */
bool candidate_line(Move* cand_line);

/**
 * \fn void remove_lines(Move move)
 * \brief Remove all lines containing move from lines history.
 *
 * \param move Move to be removed.
 */
void remove_lines(Move move);

/**
 * \fn void initialize_HistoryList_from_string(char* s);
 * \brief Initialize HistoryList static variable from a string.
 *
 * \param s String used to initialise HistoryList
 */
void initialize_HistoryList_from_string(char* s);

/**
 * \fn size_t get_move_count(void);
 * \brief Get number of moves in history.
 *
 * \return The number of moves.
 */
size_t get_move_count(void);

/**
 * \fn void translate_history_x_axis(void);
 * \brief Translate history on a x axis.
 */
void translate_history_x_axis(void);

/**
 * \fn void translate_history_y_axis(void);
 * \brief Translate history on a y axis.
 */
void translate_history_y_axis(void);

/**
 * \fn void board_expansion_history_translation(Coord coord);
 * \brief Translate history on a x or y (or both) axis depending on Coord
 *
 * \param coord Coordinates
 */
void board_expansion_history_translation(Coord coord);

#endif

