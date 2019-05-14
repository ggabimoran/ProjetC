#include <stdlib.h>
#include "CUnit/CUnit.h"
#include "CUnit/Basic.h"
#include "linux/limits.h"
#include "../headers/board.h"
#include "../headers/utils.h"
#include "../headers/history.h"
#include "../headers/interface.h"


static Error* error;

/* The suite initialization function.
 * Opens a file and write the date of the execution of the test suite
 * Returns the zero on success, non-zero otherwise.
 */
int init_suite(void){
    error = malloc(sizeof(Error));
    system("date");
    return 0;
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int clean_suite(void)
{
    free(error);
    return 0;
}

/************* TESTS *************/
void test_read_file_char_is_invalid(void)
{
    *error = NO_ERR;
    char resolved_path[PATH_MAX];
    realpath("test-files/board-invalid-char", resolved_path);
    FILE *fp = fopen(resolved_path, "r");
    /* Check return value */
    CU_ASSERT_FALSE(check_file(fp,error))
    /* Deeper test: check error value */
    CU_ASSERT(*error == FILE_UNKNOWN_CHAR_ERR);
    fclose(fp);
    free(fp);
}

void test_read_file_dimensions_are_invalid(void)
{
    *error = NO_ERR;
    char resolved_path[PATH_MAX];
    realpath("test-files/board-invalid-dimensions", resolved_path);
    FILE *fp = fopen(resolved_path, "r");
    /* Check return value */
    CU_ASSERT_FALSE(check_file(fp,error));
    /* Deeper test: check error value */
    CU_ASSERT(*error == FILE_DIMENSION_ERR);
    fclose(fp);
    free(fp);
}

void test_read_file_path_is_invalid(void)
{
    *error = NO_ERR;
    char resolved_path[PATH_MAX];
    realpath("path/is/invalid", resolved_path);
    FILE *fp = fopen(resolved_path, "r");
    /* Check return value */
    CU_ASSERT_FALSE(check_file(fp,error));
    /* Deeper test: check error value */
    CU_ASSERT(*error == FILE_PTR_ERR);
    if(fp) {
        fclose(fp);
        free(fp);
    }
}

void test_read_file_board_is_valid(void)
{
    *error = NO_ERR;
    char resolved_path[PATH_MAX];
    realpath("test-files/board-valid", resolved_path);
    FILE *fp = fopen(resolved_path, "r");
    /* Check return value */
    CU_ASSERT(check_file(fp,error));
    /* Deeper test: check error value */
    CU_ASSERT(*error == NO_ERR);
    fclose(fp);
    free(fp);
}

void test_create_empty_board_size_300(void){
    Board* pboard = NULL;
    Board board;
    board = create_empty_board(300,300);
    pboard = &board;
    CU_ASSERT_NOT_EQUAL(pboard,NULL);
    CU_ASSERT_EQUAL(pboard->height,300);
    CU_ASSERT_EQUAL(pboard->width,300);
    free_board(&board);
}

void test_create_board_from_file(void)
{
    Board board;
    *error = NO_ERR;
    char resolved_path[PATH_MAX];
    realpath("test-files/board-valid", resolved_path);
    /* Check that initialization went well */
    CU_ASSERT(initialize_file(&board, resolved_path, error))
    /* Check board size */
    CU_ASSERT_TRUE(board.height == 5);
    CU_ASSERT_TRUE(board.width == 6);
    /* Deeper test: check error value */
    CU_ASSERT(*error == NO_ERR);
    /*remove_points(&board);
    free_board(&board);*/
}

void test_add_already_existing_point(void)
{
    Board board;
    *error = NO_ERR;
    char resolved_path[PATH_MAX];
    realpath("test-files/board-valid", resolved_path);
    /* Check that initialization went well */
    CU_ASSERT(initialize_file(&board, resolved_path, error))
    /* Check return value of a played move */
        /* Create the coord of the played move */
        Coord coord;
        coord.x = 3;
        coord.y = 0;
        /* A point exists in 3,0 */
    CU_ASSERT_FALSE(play_move(&board,coord,error));
    /* Deeper test: check error value */
    CU_ASSERT(*error == POINT_ALREADY_EXIST_ERR);
    /*remove_points(&board);
    free_board(&board);*/
}

void test_add_valid_alignment(void)
{
    Board board;
    *error = NO_ERR;
    char resolved_path[PATH_MAX];
    realpath("test-files/board-valid", resolved_path);
    /* Check that initialization went well */
    CU_ASSERT(initialize_file(&board, resolved_path, error))
    /* Check return value of a played move */
        /* Create the coord of the played move */
        Coord coord;
        coord.x = 4;
        coord.y = 3;
        /* An alignment is possible in 4,3 */
    CU_ASSERT(play_move(&board,coord,error));
    /* Deeper test: check error value */
    CU_ASSERT(*error == NO_ERR);
    /*remove_points(&board);
    free_board(&board);*/

}

void test_add_invalid_alignment(void)
{
    Board board;
    *error = NO_ERR;
    char resolved_path[PATH_MAX];
    realpath("test-files/board-valid", resolved_path);
    /* Check that initialization went well */
    CU_ASSERT(initialize_file(&board, resolved_path, error))
    /* Check return value of a played move */
        /* Create the coord of the played move */
        Coord coord;
        coord.x = 0;
        coord.y = 0;
        /* No alignment possible in 0,0 */
    CU_ASSERT_FALSE(play_move(&board,coord,error));
    /* Deeper test: check error value */
    CU_ASSERT(*error == ALIGNMENT_ERR);
    /*remove_points(&board);
    free_board(&board);*/
}

int main(void)
{
    CU_pSuite pSuiteReadFile = NULL;
    CU_pSuite pSuiteBoard = NULL;

    /* initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();
    /* add a suite to the registry */
    pSuiteReadFile = CU_add_suite("Read_file_suite", init_suite, clean_suite);
    pSuiteBoard = CU_add_suite("Board_suite", NULL, NULL);
    if (NULL == pSuiteReadFile) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    if (NULL == pSuiteBoard) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* add the tests to the suite */
    if((NULL == CU_add_test(pSuiteReadFile, "Read_board_file_invalid_char", test_read_file_char_is_invalid))
        ||
        (NULL == CU_add_test(pSuiteReadFile, "Read_board_file_dimensions_invalid", test_read_file_dimensions_are_invalid))
        ||
        (NULL == CU_add_test(pSuiteReadFile, "Read_board_file_path_invalid", test_read_file_path_is_invalid))
        ||
        (NULL == CU_add_test(pSuiteReadFile, "Read_board_valid", test_read_file_board_is_valid))
        ||
        (NULL == CU_add_test(pSuiteBoard, "Create_empty_board_size_300", test_create_empty_board_size_300))
        ||
        (NULL == CU_add_test(pSuiteBoard, "Create_board_from_file", test_create_board_from_file))
        ||
        (NULL == CU_add_test(pSuiteBoard, "Add_already_existing_point", test_add_already_existing_point))
        ||
        NULL == CU_add_test(pSuiteBoard, "Add_valid_alignment", test_add_valid_alignment)
        ||
        NULL == CU_add_test(pSuiteBoard, "Add_invalid_alignment", test_add_invalid_alignment))
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    // Run all tests using the basic interface
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    printf("\n");
    CU_basic_show_failures(CU_get_failure_list());
    printf("\n\n");
    /* Clean up registry and return */
    CU_cleanup_registry();
    return CU_get_error();
}
