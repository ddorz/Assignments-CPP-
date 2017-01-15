/* ======================================================================================================================= *
 * Dave Dorzback                                                                                                           *
 * COP4530                                                                                                                 *
 * Homework 4 - RescueRemy.cpp                                                                                             *
 * Maze progam that accepts input parameters to create a maze. Maze exit paths are then found/display with DFS & BFS.      *
 * ======================================================================================================================= */

#include "Position.h"
#include "Maze.h"
#include "PrintMacros.h"

int main(void) {
    
    // Variable for maze size
    int maze_size;
    
    // Display welcome message & instructions
    printx("Welcome to the Rat in the Maze program, where we will find a\n");
    printx("path from the start cell to the end cell of a maze so that Remy\n");
    printx("may escape.  You will first enter the data specifying the maze.\n");
    printx("After that, if escape is possible, we show an escape path using\n");
    printx("DFS and then a shortest possible path.\n");
    
    // Prompt for & input maze size
    printx("\nEnter the size (number of rows and columns of the square maze): ");
    std::cin >> maze_size;
    println(2);
    
    // Build maze
    Maze RatHaus1(maze_size + 2);
    RatHaus1.initialize();
    Maze RatHaus2(RatHaus1);
    
    // Display maze
    println(2, "Constructed Maze:", 2);
    RatHaus1.display(std::cout);
    
    // Get/Display DFS exit path
    RatHaus1.print_dfsExitPath();
    println();
    
    // Get/Display BFS (shortest) exit path
    RatHaus2.printShortestPath();
    println();
    
    return 0;
}
