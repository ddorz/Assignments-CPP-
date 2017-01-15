/* ======================================================================================================================= *
 * Dave Dorzback                                                                                                           *
 * COP4530                                                                                                                 *
 * Homework 4 - Maze.h                                                                                                     *
 * ======================================================================================================================= */

#ifndef __MAZE
#define __MAZE

#include "Position.h"
#include <stack>
#include <queue>

/*
 * State enumeration - three possible states: open, visited, wall
 */
enum State {
    OPEN,
    WALL,
    VISITED
};

/*
 * Position object with values -1, -1. Represents a null position.
 */
const Position NULLPOS(-1, -1);

/*
 * Allocates dynamic 2-d array of states having rows and coloumns of size. Returns pointer used to create array.
 */
State **getMazeArray(std::size_t size);

/*
 * Maze class
 */
class Maze {
    
    private:
        /* Maze Size (row and column size) */
        std::size_t size;
    
        /* Maze Start Position & Exit Positions */
        Position start, exitpos;
    
        /* State Double Pointer - for 2-D array of maze state values */
        State **M;
    
    public:
        /* Defualt Maze Size */
        static const std::size_t MAZEMAX = 12;
    
        /* Constructors */
        Maze();              // Start set to upper left corner, exit to lower right corner, size to MAZEMAX
        Maze(std::size_t n); // Start set to upper left corner, exit to lower right corner, size to n
        Maze(Position s, Position e);                // Start set to s, exit to e, size to MAZEMAX
        Maze(std::size_t n, Position s, Position e); // Start set to s, exit to e, size to n
        Maze(const Maze &other);                     // Copy constructor
    
        /* Destructor */
        ~Maze();
    
        /* Overloaded Assignment Operator */
        Maze& operator=(const Maze &origMaze);

        /* Initialize Maze */
        void initialize();
    
        /* Display Maze */
        void display(std::ostream &out) const;
    
        /* Get State of Position */
        State getState(const Position &P) const;
    
        /* Set State of Position - P must correspond to non-hedge position within maze */
        void setState(const Position &P, State s);

        /* Print DFS Exit Path - Uses DFS to find maze exit path */
        void print_dfsExitPath();
    
        /* Print BFS Exit Path - Uses BFS to find shortest maze exit path */
        void printShortestPath();

        /* Display DFS Exit Path - Displays the maze with the DFS exit path in 'path' para */
        void display_exit_path(std::ostream &out, std::stack<Position> path) const;
    
        /* Display BFS Exit Path - Prints the maze along with the BFS exit path in 'path' para */
        void display_shortest_exit_path(std::ostream &out, Position **path) const;
    
        /* Print Stack Path - Prints the maze's exit path coordinates obtained via DFS */
        void printStackPath(std::stack<Position> S) const;
    
        /* Print Predecessor Path - Prints the maze's shortest exit path coordinates obtained via BFS */
        void printPredecessorPath(Position **pred, Position target, char **m) const;
};

#endif
