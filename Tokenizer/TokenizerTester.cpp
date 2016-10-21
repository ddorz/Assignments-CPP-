/* ======================================================================================================================= *
 * Dave Dorzback                                                                                                           *
 * COP4530 - Data Structures                                                                                               *
 * TokenizerTester.cpp                                                                                                     *
 * ======================================================================================================================= */

#include "Tokenizer.h"
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>

/*
 * Macros (color escape sequences):
 */

#define DEF "\033[0m"       // Delete this line to remove color from output

#ifdef DEF
#   define MAG "\x1b[35m"
#   define KCYN "\x1B[36m"
#   define RED "\x1b[31m"
#else
#   define DEF ""
#   define MAG ""
#   define KCYN ""
#   define RED ""
#endif 

/*
 * Local function declarations:
 */

void printTokens(std::list<std::string> &token_list);

/*
 * Main:
 */

int main(int argc, const char *argv[]) {
    
    std::list<std::string> my_token_list;   // Token list for tokenizer function
    std::string input_string;               // Input string for tokenizer function
    std::string delimiter_string;           // Delimiter string for tokenizer function
    
    /* Allow input via command line arguments. If command line arguments used, verify user wants to use them */
    if (argc >= 3) {
        
        std::string user_input;
        
        // Ask user if command line arguments should be used as input / delimiter strings. Loop until valid input is entered.
        std::cout << KCYN "Command line arguments detected.\nUse arguments as input/delimiter strings? (Y/N): ";
        do {
            std::getline(std::cin, user_input);
            
            // Use command line arguments for input/delimiter strings if user enters 'Y'. Else, if user enters 'N', break and prompt for strings.
            if ((user_input[0]= tolower(user_input[0])) == 'y') {
                
                // Call tokenizer function using command line arguments and my_token_list
                tokenizer(std::string(argv[1]), std::string(argv[2]), my_token_list);
                
                // Print tokens & exit message. Restore console color.
                printTokens(my_token_list);
                std::cout << KCYN "\nNow Exiting. . .\n" DEF;
                return 0;
            } else if (user_input[0] == 'n') break;
            
        } while (std::cout << "Error: invalid input. Please enter 'Y' or 'N': ");
    }
    
    /* Prompt user for input string. Loop until valid input is entered. */
    std::cout << KCYN "Enter an input string: ";
    do {
        std::getline(std::cin, input_string);
    } while (input_string.empty() && std::cout << "Error: invalid input. Please enter an input string: ");
    
    /* Prompt user for delimiter string. Loop until valid input is entered. */
    std::cout << "Enter the delimiter string: ";
    do {
        std::getline(std::cin, delimiter_string);
    } while (delimiter_string.empty() && std::cout << "Error: invalid input. Please enter the delimiter string: ");
    
    /* Call tokenizer function using user input strings and my_token_list. */
    tokenizer(input_string, delimiter_string, my_token_list);
    
    /* Print tokens & exit message. Restore console color. */
    printTokens(my_token_list);
    std::cout << KCYN "\nNow Exiting. . .\n" DEF;
    return 0;
}

/*
 * Functions:
 */

// Macro to replace to_string (g++ 4.2.1 on circe does not recognize to_string)
#define to_str(string) ((static_cast<std::ostringstream&>(std::ostringstream().flush() << string)).str())

/* **************************************************************************************************************************
 * printTokens:     param 1 - list of strings (tokens)                                                                      *
 *                                                                                                                          *
 * Prints each token string in token_list on a separate line. Each item is removed from the list after being printed, so    *
 * the list will be empty after the function returns.                                                                       *
 * **************************************************************************************************************************/
void printTokens(std::list<std::string> &token_list) {

    std::cout <<( token_list.size() ? MAG "Tokens: " : RED "Token list is empty." )<< std::endl;
    
    for (int i {0}; !token_list.empty(); token_list.pop_front()) {  //
        std::cout << std::left << std::setw(5) << to_str(++i) + "." << *token_list.begin() << std::endl;
    }
}