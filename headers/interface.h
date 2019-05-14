#ifndef INTERFACE_H
#define INTERFACE_H

/**
 * \file interface.h
 * \brief Displays the Board and manages the user/program interactions.
 * \authors Geoffrey DELVAL, Rémi GUIJARRO ESPINOSA, Gabriel MORAN, Clément PERESSE
 * \version lot_b
 * \date 04.2019
 *
 * Handles displaying and asks the user for input.
 *
 */

#include "board.h"
#include "utils.h"

#include <stdbool.h>

/**
 * \fn void set_hint(bool boolean);
 * \brief Set hint value.
 *
 * \param boolean Value to set.
 */
void set_hint(bool boolean);

/**
 * \fn void set_fullscreen(bool set);
 * \brief Set full screen for terminal.
 *
 * \param set True to activate full screen, false to disable full screen.
 */
void set_fullscreen(bool set);

/**
 * \fn print_board(Board* pboard)
 * \brief Print the Board, as well as a hint (i.e. a list of available moves) if required.
 *
 * \param pboard Board pointer 
 */
void print_board(Board* pboard);

/**
 * \fn select_move(void)
 * \brief Ask the user to play a move.
 *
 * \return Coord of the Move to play.
 */
Coord select_move(void);

/**
 * \fn enum action select_action(void)
 * \brief Ask the user for the next action.
 *
 * \return Desired action.
 */
enum action select_action(void);

/**
 * \fn print_help(void)
 * \brief Print help for the user.
 */
void print_help(void);

/**
 * \fn select_line(Move* pmove)
 * \brief Select among available lines (if more than 1 are available)
 */
void select_line(Move* pmove);

/**
 * \fn bool confirm_quit_save(Board* pboard);
 * \brief Ask user for confirmation to quit and/or save game.
 *
 * \param pboard Pointer to Board.
 * \return True if user decides to quit the game, false otherwise.
 */
bool confirm_quit_save(Board* pboard);

/**
 * \fn ask_savefile_name(void)
 * \brief Ask the user for the saved file name.
 *
 * \return The filename.
 */
char* ask_savefile_name(void);

/**
 * \fn void display_logo(void);
 * \brief Print game logo (starting screen)
 */
void display_logo(void);

/**
 * \fn void clear_screen(void);
 * \brief Clear the terminal.
 */
void clear_screen(void);

/**
 * \fn void press_enter_to_continue(void);
 * \brief Block terminal and wait for user to type <enter>
 */
void press_enter_to_continue(void);

/**
 * \fn void print_error(Error error);
 * \brief Print error message.
 *
 * \param err Error to be displayed.
 */
void print_error(Error err);

/**
 * \fn void print_game_over(void);
 * \brief Print game over message.
 */
void print_game_over(void);

/**
 * \fn void print_score(void);
 * \brief Print user score.
 */
void print_score(void);

#endif
