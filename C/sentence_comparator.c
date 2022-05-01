// Unix-based program
/*
 * This program allows you to enter two sentences, which it then compares and tells you if they use the same words.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_INPUT 2000 //maximum number of letters in the input

typedef struct
{
        char *name;
} words; //struct of strings

int continue_or_exit ();
int string_word_counter ( char *string );
int compare_sentences ( char *string_a, char *string_b );
int compare_words_between_strings ( words *first_array, words *second_array, int first_array_word_count, int second_array_word_count);
void welcome_message ();
void output ( int result );
void sentence_correction ( char *string );
void free_memory ( char *first_string, char *second_string, words *first_array, words *second_array, int first_array_word_count, int second_array_word_count );
char *input_sentence ( int order );
words *split_sentence ( char* string, int *array_word_count );

int main ( void ){

        welcome_message();

        do {
                char *first_sentence = input_sentence( 1 );
                char *second_sentence = input_sentence( 2 );

                int result_value = compare_sentences( first_sentence, second_sentence );
                output(result_value);

                printf("Do you want to exit the application or continue?\n n - for continue\n y - to end the application\n");
                if ( continue_or_exit() > 0 ) break;

        } while( 1 );

        printf("Program will be now closed.\n");

        return 0;
}

void welcome_message(){

        printf("Welcome user <guest%d> \n", rand() % 1000 );
        return;
}

/*
 * Takes care of user main input
 *
 * @localvariable bytes_read -> number of characters readed by getline( function responsible for reading the input )
 *
 * @return input -> loaded input
 */
char *input_sentence ( int order ){

        char *input;
        char *tmp_input = (char *) malloc (MAX_INPUT);

        if ( order == 1 ) printf("Enter first sentence as input: ( max number of characters in input is %d)\n", MAX_INPUT);
        if ( order == 2 ) printf("Enter second sentence as input: ( max number of characters in input is %d)\n", MAX_INPUT);

        while ( 1 ) {
                int bytes_read;
                size_t max_size_of_input = MAX_INPUT;
                if ( (bytes_read = getline(&tmp_input, &max_size_of_input, stdin)) != -1 ) {
                        input = (char *) malloc (bytes_read);
                        int input_len = bytes_read - 1;
                        if( feof(stdin) == 1 ) {
                                printf("\n");
                                clearerr(stdin);
                                input_len++;
                        }
                        strncpy(input, tmp_input, input_len);
                        break;
                }
                else if( bytes_read == EOF ) {
                        printf("Quit application? ( type y/n for yes/no choice )\n");
                        clearerr(stdin);
                        if ( continue_or_exit() > 0 ) {
                                free(tmp_input);
                                exit(0);
                        }
                }
                else {
                        printf("Error occured. Program must be now terminated.\n");
                        free(tmp_input);
                        exit(1);
                }
        }

        free(tmp_input);
        return input;
}

/*
 * It takes care of user input when deciding whether to continue or end application.
 *
 * @return 1 -> if user entered succesfully y/Y
 *         0 -> if user entered succesfully n/N
 */
int continue_or_exit (){

        while( 1 ) {
                char user_choice;
                int input_validity = scanf(" %c", &user_choice);
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

        printf("Error occured. Program will be now terminated.\n");
        exit(1);
}

/*
 * The main function, which determining the similarity (of use) of the words between two strings
 * The function uses additional functions for better orientation and adjustment
 *
 * @param[in] first_string -> first passed string for comparing
 * @param[in] second_string -> second passed string for comparing
 *
 * @localvariable compare_result -> stores the result of the compare_words_between_strings function
 * @localvariables first_array, second_array -> will keep individual separated words
 *
 * @return compare_result -> 1 if strings are using same words
 *                        -> 0 if strings are not using same words
 */
int compare_sentences ( char *first_string, char *second_string ){

        words *first_array;
        words *second_array;

        sentence_correction(first_string);
        sentence_correction(second_string);

        int first_array_word_count = 0, second_array_word_count = 0;
        first_array = split_sentence(first_string, &first_array_word_count);
        second_array = split_sentence(second_string, &second_array_word_count);

        int compare_result = 0;
        compare_result = compare_words_between_strings(first_array, second_array, first_array_word_count, second_array_word_count);

        free_memory(first_string, second_string, first_array, second_array, first_array_word_count, second_array_word_count);

        return compare_result;
}

/*
 * Erases non-alphabet characters
 *
 * @param[in] string  -> original string from input (unmodified)
 */
void sentence_correction ( char *string ){

        for (; *string; string++) {
                if ( !isalpha(*string) ) {
                        *string = ' ';
                }
        }
        return;
}

/*
 * Breaks a sentence into words using strtok function
 *
 * @param[in] string -> original string
 * @param[in] array_word_count -> location where is stored count of separated words
 *
 * @return array -> the place where the individual words will be stored (struct of strings)
 */
words *split_sentence ( char *string, int *array_word_count ){

        char *support_string = strdup(string);
        words *array = (words *)malloc( string_word_counter(support_string) * sizeof(words) );
        free(support_string);

        char *support_array;
        support_array = strtok(string," ");

        int j = 0;
        while (support_array != NULL)
        {
                for(int i = 0; support_array[i]; i++) {
                        support_array[i] = tolower(support_array[i]);
                }
                (array + j)->name = strdup(support_array);
                support_array = strtok(NULL, " ");
                j++;
        }
        *array_word_count = j;
        return array;
}

/*
 * Find out the number of words in string
 *
 * @param[in] string -> corrected original string
 *
 * @return count -> total number of words in string
 */
int string_word_counter ( char *string ){

        int count = 0;

        char *tmp_array = strtok(string," ");

        while (tmp_array != NULL) {
                tmp_array = strtok(NULL, " ");
                count++;
        }

        return count;
}

/**
 * Function which determines if the words of one string are also used by the other string (regardless of their number)
 *
 * @param[in] first_array_word_count -> number of words in first array
 * @param[in] second_array_word_count -> number of words in second array
 * @param[in] first_array -> location of words from first string
 * @param[in] second_array -> location of words from second string
 *
 * @localvariable control_compare -> 1 words match
 *                                -> 0 word not found in the second array
 *
 * @return -> 1 if strings are using same words
 *         -> 0 if strings are not using same words
 */
int compare_words_between_strings ( words *first_array, words *second_array, int first_array_word_count, int second_array_word_count ){

        int i = 0, j = 0, control_compare = 0;

        while ( i < first_array_word_count ) {
                control_compare = 0;
                while ( j < second_array_word_count ) {
                        if( (strcmp(( first_array + i )->name, ( second_array + j )->name )) == 0 ) {
                                control_compare = 1;
                                break;
                        }
                        j++;
                }
                if ( control_compare == 0) return 0;
                j = 0;
                i++;
        }

        return 1;
}

/**
 * Frees all allocated memory
 */
void free_memory ( char *first_string, char *second_string, words *first_array, words *second_array, int first_array_word_count, int second_array_word_count ){
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
        if ( result == 1 ) printf("The sentences being compared consist of the same words.\n");
        if ( result == 0 ) printf("The sentences being compared dont consist of the same words.\n");
        return;
}
