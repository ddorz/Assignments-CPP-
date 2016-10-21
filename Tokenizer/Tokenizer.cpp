/* ======================================================================================================================= *
 * Dave Dorzback                                                                                                           *
 * COP4530 - Data Structures                                                                                               *
 * Tokenizer.cpp                                                                                                           *
 * ======================================================================================================================= */

#include "Tokenizer.h"
#include <iostream>

/* **************************************************************************************************************************
 * tokenizer:   param 1 - input string to be tokenized.                                                                     *
 *              param 2 - delimiter string.                                                                                 *
 *              param 3 - string list for tokens to be stored in.                                                           *
 *                                                                                                                          *
 * String tokenization function - tokens are extracted from the input string and saved to tokenList.                        *
 * A finite state machine implementation is used to keep track of delimiter/token positions within the input string.        *
 * While iterating over the input string there are three possible states:                                                   *
 * Mismatch  - element of input string did not match element of delimiter.                                                  *
 * Match     - element of input string matched element of delimiter, but entire delimiter not matched.                      *
 * Delimiter - entire delimiter has been matched, so add new token and get next token starting idx.                         *
 *                                                                                                                          *
 * Note: Empty strings will be added to tokenList if a delimiter is entered that directly proceeds another delimiter.       *
 * The function can be modified not to add empty strings by adding a conditional statement to the DELIMITER case and the    *
 * end of the function.                                                                                                     *
 * **************************************************************************************************************************/

void tokenizer(const std::string input, const std::string delimiter, std::list<std::string> &tokenList) {
    
    // Print error message and return if the input or delimiter strings are empty
    if (input.empty() || delimiter.empty()) {
        std::cout << "Error: input and delimiter strings cannot be empty.\n";
        return;
    }
    
    std::string::const_iterator in_iter  = input.begin();       // Iterator for input string
    std::string::const_iterator del_iter = delimiter.begin();   // Iterator for delimiter string
    size_t token_pos = 0;                                       // Token string start position
    
    // Three possible 'states' while iterating through the input string:
    enum {MISMATCH, MATCH, DELIMITER} state = MISMATCH;

    while (in_iter != input.end()) {
        switch (state) {
            case MISMATCH:  // Mismatch state: <=> see if current character matches first delimiter character
                if (*in_iter == *del_iter)
                    state = ++del_iter == delimiter.end()                                         // If they match, see if entire delimiter has been matched
                    ?       DELIMITER
                    :       MATCH;
            break;
            case MATCH:     // Match state <=> see if the current character matches the next delimiter character.
                if (*in_iter != *del_iter && *in_iter != *(del_iter= delimiter.begin()))          // If they don't match, reset the delimiter iterator and compare again
                    state = MISMATCH;
                else if (++del_iter == delimiter.end())                                           // If they do match, see if the entire delimiter has been matched
                    state = DELIMITER;
            break;
            case DELIMITER: // Delimiter state <=> Add new token, set next token position, test current character.
                tokenList.push_back(input.substr(token_pos, in_iter - input.begin() - delimiter.size() - token_pos));
                token_pos = in_iter - input.begin();                                               // Set the new token token pos to current pos
                if (*in_iter != *(del_iter= delimiter.begin()))                                    // Reset delimiter iterator and set new state accordingly
                    state = MISMATCH;
                else if (++del_iter != delimiter.end())
                    state = MATCH;
            break;
        }
        ++in_iter;
    }
    // Generate final token string and add it to list. Token size will depend on whether final state was DELIMITIER state or not
    tokenList.push_back(input.substr(token_pos, state != DELIMITER ?  std::string::npos  :  input.size() - delimiter.size() - token_pos));
}