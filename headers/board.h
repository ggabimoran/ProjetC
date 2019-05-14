#ifndef BOARD_H
#define BOARD_H

/**
 * \file board.h
 * \brief This file handles the Board.
 * \authors Geoffrey DELVAL, Rémi GUIJARRO ESPINOSA, Gabriel MORAN, Clément PERESSE
 * \version lot_b
 * \date 04.2019
 *
 * Handles the grid, points and lines on it.
 *
 */

#include "utils.h"

#include <stdbool.h>
#include <stdio.h>

/**
 * \enum action
 * \brief Represents a game action.
 */
enum action 
{ 
	PLAY_MOVE, /**< Play a move */ 
	CANCEL_MOVE, /**< Cancel last move */
	REPLAY_MOVE, /**< Replay a canceled move */
	LIST_MOVES, /**< List all valid moves */
	ASK_HELP, /**< Ask for help to be displayed */
	QUIT_GAME /**<Quit the game */
};



typedef struct coord* Move; ///< Pointer to Coord, represents a move

/**
 * \struct coord
 * \brief Coordinates on the Board
 */
typedef struct coord {
  int x; /**< Horizontal address */
  int y; /**< Vertical address */
  Move previous; /**< Previous Move */
} Coord; /**< struct coord alias */

/**
 * \brief Represents a point on the board
 *
 * Pointer to int, is NULL when the point is nonexistant, has a value if the point exists.
 */
typedef int* Ppoint;

/**
 * \struct Board
 * \brief Represents the Board of the game with width, height and a 2d table of Coord pointers.
 */
typedef struct {
    int width; /**< Board width */
    int height; /**< Board height */
    Ppoint** points; /**< Bidimensionnal array of Ppoint */ 
} Board;

/**
 * \fn void update_points_scored(void);
 * \brief Update the user score by incrementing the move count.
 */
void update_points_scored(void);

/**
 * \fn int get_points_scored(void);
 * \brief Get the current user score.
 *
 * \return The score.
 */
int get_points_scored(void);

/**
 * \fn void update_points_scored_val(int val);
 * \brief Update the user score by adding passed value to current score.
 *
 * \param val Value to add to score.
 */
void update_points_scored_val(int val);

/**
 * \fn void create_empty_board(int width,int height)
 * \brief Initialize an empty Board
 *
 * \param width Width of the Board.
 * \param height Height of the Board.
 */
Board create_empty_board(int width,int height); 

/**
 * \fn void get_valid_moves(Board* pboard, Move* pvalid_points)
 * \brief Give a list of all valid moves.
 *
 * \param pboard Pointer to current Board.
 * \param pvalid_points Pointer to Move that will hold valid points
 */
void get_valid_moves(Board* pboard, Move* pvalid_points);

/**
 * \fn bool is_move_valid(Board* pboard,Coord coord,Move* pmove, Error* error);
 * \brief Check if a move is valid.
 *
 * \param pboard Pointer to current Board.
 * \param coord Coordinates of the move to check.
 * \param pmove Pointer to Move.
 * \para error Error pointer to hold to potential error.
 * \return True if no error occurred, false otherwise.
 */
bool is_move_valid(Board* pboard,Coord coord,Move* pmove, Error* error);

/**
 * \fn bool is_move_in_board(Board* pboard,Coord coord);
 * \brief Test if desired move is within the Board.
 *
 * \param pboard Pointer to Board.
 * \param coord Coord structure of desired point.
 * \return True if move is in board, false otherwise.
 */
bool is_move_in_board(Board* pboard,Coord coord);

/**
 * \fn bool is_move_exists_already(Board* pboard,Coord coord);
 * \brief Test if desired move already exists.
 *
 * \param pboard Pointer to Board.
 * \param coord Coord structure of desired point.
 * \return True if move already exists, false otherwise.
 */
bool is_move_exists_already(Board* pboard,Coord coord);

/**
 * \fn void horizontal_search(Move* pcand_lines,Coord coord,Board* pboard);
 * \brief Search for possible alignments on horizontal axis.
 *
 * \param pboard Pointer to Board.
 * \param coord Coord structure of desired point.
 * \param pcand_lines Pointer to list of possible alignments.
 */
void horizontal_search(Move* pcand_lines,Coord coord,Board* pboard);

/**
 * \fn void vertical_search(Move* pcand_lines,Coord coord,Board* pboard);
 * \brief Search for possible alignments on vertical axis.
 *
 * \param pboard Pointer to Board.
 * \param coord Coord structure of desired point.
 * \param pcand_lines Pointer to list of possible alignments.
 */
void vertical_search(Move* pcand_lines,Coord coord,Board* pboard);

/**
 * \fn void NE_diagonal_search(Move* pcand_lines,Coord coord,Board* pboard);
 * \brief Search for possible alignments on north east diagonal axis.
 *
 * \param pboard Pointer to Board.
 * \param coord Coord structure of desired point.
 * \param pcand_lines Pointer to list of possible alignments.
 */
void NE_diagonal_search(Move* pcand_lines,Coord coord,Board* pboard);

/**
 * \fn void NW_diagonal_search(Move* pcand_lines,Coord coord,Board* pboard);
 * \brief Search for possible alignments on north west diagonal axis
 *
 * \param pboard Pointer to Board.
 * \param coord Coord structure of desired point.
 * \param pcand_lines Pointer to list of possible alignments.
 */
void NW_diagonal_search(Move* pcand_lines,Coord coord,Board* pboard);


/**
 * \fn bool add_point(Board* pboard, Coord coord)
 * \brief Add a point on the Board.
 *
 * \param pboard Pointer to the Board being played
 * \param coord Coordinates of the point being played
 * \return True if point successfully added, false otherwise.
 */
bool add_point(Board* pboard, Coord coord, Error* error);

/**
 * \fn void execute_action(Board* pboard, enum action action, bool* quit)
 * \brief Execute action passed as a parameter.
 * 
 * \param pboard Pointer to the Board being played.
 * \param action Action to execute.
 * \param quit Pointer to bool holding true if user wishes to quit the game.
 */
void execute_action(Board* pboard, enum action action, bool* quit, Error* error);

/**
 * \fn bool is_game_over(Board* pboard);
 * \brief Check if game is over. 
 *
 * \param pboard Pointer to the Board being played.
 * \return True if game is over, false otherwise.
 */
bool is_game_over(Board* pboard);

/**
 * \fn void free_board(Board* pboard)
 * \brief Free allocated memory of Board.
 *
 * \param pboard Pointer to Board.
 */
void free_board(Board* pboard);

/**
 * \fn void remove_point(Board* pboard,Coord coord);
 * \brief removes point from board.
 *
 * \param pboard Pointer to Board.
 * \param coord Coord structure of the point to be removed.
 */
void remove_point(Board* pboard,Coord coord);

/**
 * \fn void remove_points(Board* pboard);
 * \brief Remove all points from Board.
 *
 * \param pboard Pointer to Board
 */
void remove_points(Board* pboard);

/**
 * \fn void save_board(Board* pboard)
 * \brief Save the current board into a file.
 *
 * @param pboard Pointer to the Board being played
 */
void save_board(Board* pboard);

/**
 * \fn Board* initialize_rand(void)
 * \brief Randomly initialize a Board.
 *
 * \return Pointer to a Board
 */
Board initialize_rand(void);

/**
 * \fn bool initialize_file(Board* pboard, char* path)
 * \brief Read the given file and initialize the Board accordingly.
 *
 * \param pboard Board pointer, SHOULD be NULL
 * \param path File absolute path
 * \return True if no error occurred, false otherwise
 */
bool initialize_file(Board* pboard, char* path, Error* error);

/**
 * \fn void board_expansion(Board* pboard,Coord coord);
 * \brief Extend the board size.
 *
 * \param pboard Pointer to Board being played.
 * \param coord Coordinates impacting the Board dimensions.
 */
void board_expansion(Board* pboard,Coord coord);

#endif
