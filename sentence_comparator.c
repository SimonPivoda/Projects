// Unix-based program
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <unistd.h>
#include <pwd.h>

#define MAX_INPUT 2000 //maximum number of letters in the input

typedef struct
{
        char * name;
} words; //struct of strings

int  compare_words_between_strings ( int first_array_word_count, int second_array_word_count,  words * first_array, words * second_array );
int  separate ( char * string, words * array );
int  sameWords ( char * string_a, char * string_b );
int  continue_or_exit ();
int  input ();
void output ( int result );
void free_memory ( int first_array_word_count, int second_array_word_count, char * first_string, char * second_string, words * first_array, words * second_array );

int main ( void )
{
        struct passwd * user_passwd;
        int input_value;

        if ( (user_passwd = getpwuid(geteuid())) == NULL) {
                printf("Error occured during loading of user informations. Program will be now closed.\n");
                return -1;
        }

        printf("Welcome user <%s>: \n", user_passwd->pw_name);

        do {
                input_value = input();
                if ( input_value == 2 || input_value == 3 ) break;
                output(input_value);
                printf("Do you want to exit the application or continue?\n n - for continue\n y - to end the application\n");
                if ( continue_or_exit() == 1 ) break;

        } while( 1 );

        printf("Program will be now closed.\n");

        return 0;
}

/**
 * Takes care of user main input
 *
 * @localvariable input_succeed -> serves to store the number of successfully entered inputs ( 2 max per function call )
 *
 * @return  sameWord -> 0/1
 *          2 -> if the user decided to quit the program during the first entry
 *          3 -> if the user decided to quit the program during the second entry
 */
int input (){

        int bytes_read, input_succeed = 0, first_input_len = 0, second_input_len = 0;
        size_t size_of_string = MAX_INPUT;
        char * first_input;
        char * second_input;
        char * string = (char *) malloc (size_of_string);

        printf("Enter first sentence as input: ( max number of characters in input is %d)\n", MAX_INPUT);

        while ( input_succeed != 2 ) {
                if ( (bytes_read = getline(&string, &size_of_string, stdin)) != -1 ) {
                        if( input_succeed == 0 ) {
                                first_input = (char *) malloc (bytes_read);
                                first_input_len = bytes_read - 1;
                                if( feof(stdin) == 1 ) {
                                        printf("\n");
                                        clearerr(stdin);
                                        first_input_len++;
                                }
                                strncpy(first_input, string, first_input_len);
                                printf("Enter second sentence as input: ( max number of characters in input is %d)\n", MAX_INPUT);
                        }
                        if( input_succeed == 1 ) {
                                second_input = (char *) malloc (bytes_read);
                                second_input_len = bytes_read - 1;
                                if( feof(stdin) == 1 ) {
                                        printf("\n");
                                        clearerr(stdin);
                                        second_input_len++;
                                }
                                strncpy(second_input, string, second_input_len);
                        }
                        input_succeed++;
                }
                else{
                        if( bytes_read == EOF ) {
                                if ( input_succeed == 0 ) {
                                        printf("Quit application: ( type y/n for yes/no choice )\n");
                                        clearerr(stdin);
                                        if ( continue_or_exit() == 1 ) {
                                                free(string);
                                                return 2;
                                        }
                                }
                                if ( input_succeed == 1 ) {
                                        printf("Quit application? You already entered first input, which will be lost after quit.  ( type y/n for yes/no choice )\n");
                                        clearerr(stdin);
                                        if ( continue_or_exit() == 1 ) {
                                                free(first_input);
                                                free(string);
                                                return 3;
                                        }
                                }
                        }
                        else{
                                printf("Error occured during input control. Program must be now terminated.\n");
                                if ( input_succeed == 1 ) free(first_input);
                                free(string);
                                exit(-1);
                        }

                }
        }

        free(string);
        return sameWords( first_input, second_input );
}

/**
 * It takes care of user input when deciding whether to continue or end application.
 *
 * @return 1 -> if user entered succesfully y/Y
 *         0 -> if user entered succesfully n/N
 *        -1 -> if error occured
 */
int continue_or_exit (){

        int input_validity = 0;
        char user_choice;

        while( 1 ) {

                input_validity = scanf(" %c", &user_choice);
                if ( input_validity == EOF ) {
                        return 1;
                }
                user_choice = tolower(user_choice);
                while( getchar() != '\n') {}
                if ( input_validity != 1 || ( user_choice != 'y'  && user_choice != 'n' ) ) {
                        printf("Invalid input. Repeat again:\n");
                }
                if ( input_validity == 1 ) {
                        if ( user_choice == 'y' ) return 1;
                        if ( user_choice == 'n' ) return 0;
                }
        }

        printf("Error occured during continue_or_exit function.\n");
        return -1;
}

/**
 * The main function, which determining the similarity (of use) of the words between two strings
 * The function uses additional functions for better orientation and adjustment
 *
 * @param[in] string_a -> first passed string for comparing
 * @param[in] string_b -> second passed string for comparing
 *
 * @localvariable correction -> helps remove unnecessary characters from strings
 * @localvariable compare_result -> stores the result of the compare_words_between_strings function
 * @localvariables first_array, second_array -> will keep individual separated words
 *
 * @return compare_result -> 1 if strings are using same words
 *                        -> 0 if strings are not using same words
 */
int sameWords ( char * string_a, char * string_b ){
        int number = 0, first_array_word_count = 0, second_array_word_count = 0, compare_result = 0;

        words * first_array;
        words * second_array;

        for (char * correction = string_a; *correction; correction++) if (!isalpha(*correction)) *correction = ' ';
        for (char * correction = string_b; *correction; correction++) if (!isalpha(*correction)) *correction = ' ';

        number = strlen(string_a);
        first_array = (words*)malloc( (number/2) * sizeof(words) );
        number = strlen(string_b);
        second_array = (words*)malloc( (number/2) * sizeof(words) );

        first_array_word_count = separate( string_a, first_array );
        second_array_word_count = separate( string_b, second_array );

        compare_result = compare_words_between_strings( first_array_word_count, second_array_word_count, first_array, second_array );

        free_memory( first_array_word_count, second_array_word_count, string_a, string_b, first_array, second_array );

        return compare_result;
}

/**
 * Breaks a sentence into words using strtok function and spaces
 *
 * @param[in] string  -> original string from function sameWords
 * @param[in] array  -> the place where the individual words will be stored (struct of strings)
 *
 * @return word_count -> total number of words in string
 */
int separate ( char * string, words * array ){

        int word_count = 0;
        char * support_array;

        support_array = strtok (string," ");

        while (support_array != NULL)
        {
                for(int i = 0; support_array[i]; i++) {
                        support_array[i] = tolower(support_array[i]);
                }
                (array + word_count)->name = strdup(support_array);
                support_array = strtok (NULL, " ");
                word_count++;
        }

        return word_count;
}

/**
 * Function which determines if the words of one string are also used by the other string (regardless of their number)
 *
 * @param[in] first_array_word_count -> number of words in first array
 * @param[in] second_array_word_count -> number of words in second array
 * @param[in] first_array -> location of words from first string
 * @param[in] second_array -> location of words from second string
 *
 * @localvariables first_array_position, second_array_position -> support variables that are using for cycle shift
 * @localvariable control_compare -> 1 words match
 *                                -> 0 word not found in the second array
 *
 * @return -> 1 if strings are using same words
 *         -> 0 if strings are not using same words
 */
int compare_words_between_strings ( int first_array_word_count, int second_array_word_count,  words * first_array, words * second_array ){

        int first_array_position = 0, second_array_position = 0, control_compare = 0;

        while ( first_array_position < first_array_word_count ) {
                control_compare = 0;
                while ( second_array_position < second_array_word_count ) {
                        if( (strcmp((first_array + first_array_position)->name, (second_array + second_array_position)->name )) == 0 ) {
                                control_compare = 1;
                                break;
                        }
                        second_array_position++;
                }
                if ( control_compare == 0) {
                        return 0;
                }
                second_array_position = 0;
                first_array_position++;
        }

        return 1;
}

/**
 * Frees all allocated memory
 */
void free_memory ( int first_array_word_count, int second_array_word_count, char * first_string, char * second_string, words * first_array, words * second_array ){
        int i = 0;
        while ( i < first_array_word_count ) {
                free((first_array + i)->name);
                i++;
        }
        i = 0;
        while ( i < second_array_word_count ) {
                free((second_array + i)->name);
                i++;
        }
        free(first_string);
        free(first_array);
        free(second_string);
        free(second_array);
        return;
}

/**
 * Prints the given sentence according to the result
 *
 * @param[in] result -> stores finall result of application
 */
void output ( int result ){
        if ( result == 1 ) printf("Sentences using same words.\n");
        if ( result == 0 ) printf("Sentences dont have all words same.\n");
        return;
}
