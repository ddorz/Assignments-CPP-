/* ======================================================================================================================= *
 * Dave Dorzback                                                                                                           *
 * COP4530 - Data Structures                                                                                               *
 * Tokenizer.h                                                                                                             *
 * ======================================================================================================================= */

#ifndef Tokenizer_h
#define Tokenizer_h

#include <string>
#include <list>

using namespace std;

/* 
 * tokenizer:    param 1 - input string to be tokenized
 *               param 2 - delimiter string
 *               param 3 - string list for tokens to be stored in
 */

void tokenizer(const std::string input, const std::string delimiter, std::list<std::string> &tokenList);

#endif /* Tokenizer_h */
