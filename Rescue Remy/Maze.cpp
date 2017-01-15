/* ======================================================================================================================= *
 * Dave Dorzback                                                                                                           *
 * COP4530                                                                                                                 *
 * Homework 4 - Maze.cpp                                                                                                   *
 * ======================================================================================================================= */

#include <cstdlib>
#include <cassert>
#include "Maze.h"
#include "PrintMacros.h"

/* Constructors/Destructor: */

/*
 * Construct Maze - Default Size (12)
 */
Maze::Maze() {
    
    size    = MAZEMAX;
    start   = Position(1, 1);                       // Upper left corner position
    exitpos = Position(MAZEMAX - 2, MAZEMAX - 2);   // lower right corner position
    M       = getMazeArray(size);
    
    for (std::size_t i {0}; i < size; i++) {
        for (std::size_t j {0}; j < size; j++) {
            M[i][j] = WALL;
        }
    }
}

/*
 * Construct Maze - Input Size (max)
 */
Maze::Maze(std::size_t max) {
    
    size    = max;
    start   = Position(1, 1);
    exitpos = Position(size - 2, size - 2);
    M       = getMazeArray(size);
    
    for (std::size_t i {0}; i < size; i++) {
        for (std::size_t j {0}; j < size; j++) {
            M[i][j] = WALL;
        }
    }
}

/*
 * Construct Maze from Another (Copy Constructor)
 */
Maze::Maze(const Maze &other):
size(other.size), start(other.start), exitpos(other.exitpos) {
    
    M = getMazeArray(size);
    for(std::size_t i {0}; i < size; i++) {
        for (std::size_t j {0}; j < size; j++) {
            M[i][j] = other.M[i][j];
        }
    }
}

/*
 * Destructor - Deallocate maze array
 */
Maze::~Maze() {
    
    for(std::size_t i {0}; i < size; ++i) {
        delete []M[i];
    }
    delete []M;
}


/* Maze Functions: */

/*
 * print_dfsExitPath - Uses depth first search to find an exit path out of the maze.
 *                     Prints the path, if found.
 */
void Maze::print_dfsExitPath() {
    
    // S - holds the coordinates exit path
    // current/adj - holds the current position/adjacent positions
    std::stack<Position> S;
    Position current, adj;

    // Push the starting position onto stack and set state
    S.push(start);
    setState(start, VISITED);
    
    // Iterate while stack is not empty or exit position has not been found
    while (!S.empty() && !((current = S.top()) == exitpos)) {
        Direction d {DOWN};
        // Check adjacent cells for open position. When an open position is found, push it onto the stack
        while (d != NONE) {
            if (getState((adj = current.Neighbor(d))) == OPEN) {
                setState(adj, VISITED);
                S.push(adj);
                break;
            } else d = next_direction(d);
        }
        // If no adjacent open positions were located, pop the current position off the stack
        // Otherwise, we transverse the maze deeper
        if (d == NONE) S.pop();
    }
    
    // Print the exit path if one was found. Otherwise, print no path message
    if (S.size()) {
        std::stack<Position> tmp = S, S2;
        while (!tmp.empty()) {
            S2.push(tmp.top());
            tmp.pop();
        }
        printx("Remy, here is a sequence of positions to escape the maze:\n");
        printStackPath(S);
        println(2);
        display_exit_path(std::cout, S2);
    } else printx("Oh no, Poor Remy! There is now way to escape from the maze.\n");
}

/*
 * printShortestPath - Uses breadth first search to find the shortest exit path of the of maze.
 *                     Prints the path, if found.
 */
void Maze::printShortestPath() {
    
    // Q - holds positions to be checked for open adjacent positions
    std::queue<Position> Q;
    
    // Initialize predecessor array
    Position **predecessor = new Position *[size];
    for(int i {0}; i < size; ++i) {
        predecessor[i] = new Position[size];
        for(int j {0}; j < size; ++j) {
            predecessor[i][j] = NULLPOS;
        }
    }
    
    // Push the starting position onto queue and set state
    Q.push(start);
    setState(start, VISITED);
    
    // Starting with front of the queue, iterate until the queue is empty
    for (Position current {Q.front()}, adj; !Q.empty(); Q.pop(), current = Q.front()) {
        // Check adjacent cells for openings.
        // When an open position is found, push it onto the queue and set state/predecessor array accordingly
        for (Direction d {DOWN}; d != NONE; d = next_direction(d)) {
            if (getState((adj = current.Neighbor(d))) == OPEN) {
                predecessor[adj.getRow()][adj.getCol()] = current;
                Q.push(adj);
                setState(adj, VISITED);
            }
        }
    }
    
    // Print the shortest path if an exit path was found. Otherwise print no path message
    if (predecessor[exitpos.getRow()][exitpos.getCol()] != NULLPOS) {
        printx("Remy, here is a shortest sequence of positions to escape the maze:\n");
        display_shortest_exit_path(std::cout, predecessor);
    } else printx("Oh no, Poor Remy! There is now way to escape from the maze.\n");
}


/*
 * Get Maze Array - Attempt to allocate array for maze
 */
State **getMazeArray(std::size_t size) {
    
    State **A;
    
    try {
        A = new State *[size];
    } catch(std::bad_alloc) {
        printx("Unable to allocate array of state pointers.\n", std::cerr);
        exit(1);
    }
    
    for(std::size_t i {0}; i < size; i++) {
        try {
            A[i] = new State[size];
        } catch (std::bad_alloc) {
            printx("Unable to allocate row of state values.\n", std::cerr);
            exit(1);
        }
    }
    return A;
}

/*
 * Maze Obj Overloaded Assignment Operator
 */
Maze& Maze::operator=(const Maze &origMaze) {
    
    if (size != origMaze.size) {
        State **hold = M;
        size         = origMaze.size;
        M            = getMazeArray(size);
        delete []hold;
    }
    
    for(std::size_t i {0}; i < size; i++) {
        for (std::size_t j {0}; j < size; j++) {
            M[i][j] = origMaze.M[i][j];
        }
    }
    
    return *this;
}

/*
 * Initialize Maze - Input start/exit positions & open positions and initialize maze
 */
void Maze::initialize() {
    
    printx("\nEnter the start position\n");
    std::cin >> start;
    printx("\nEnter the exit position\n");
    std::cin >> exitpos;
    
    printx("\n\nFor each row, enter the column indexes of the open positions,\n");
    printx("separated by spaces and terminated by an entry of 0\n");
    
    for (std::size_t i {1}, j; i < size-1; i++) {
        printx("\nrow " << i << ": ");
        std::cin >> j;
        while (j > 0){
            M[i][j] = OPEN;
            std::cin >> j;
        };
    }
    
    if (getState(start) != OPEN) M[start.getRow()][start.getCol()] = OPEN;
}

/*
 * Get State - Get state of specified maze position
 */
State Maze::getState(const Position &P) const {
    return M[P.getRow()][P.getCol()];
}

/*
 * Set State - Set state of specified maze position
 */
void Maze::setState(const Position &P, State s) {
    std::size_t i = P.getRow();
    std::size_t j = P.getCol();
    assert(1 <= i && i <= size && 1 <= j && j <= size);
    M[i][j] = s;
}

/*
 * Display Maze - Print maze to provided output stream
 */
void Maze::display(std::ostream &out) const {
    for (std::size_t i {0}; i < size; ++i) {
        for (std::size_t j {0}; j < size; ++j) {
            printx(((Position(i,j) == start && start == exitpos) ? 'b'
                :   (Position(i,j) == start)                     ? 's'
                :   (Position(i,j) == exitpos)                   ? 'e'
                :   (M[i][j]       == WALL)                      ? '*'  : '|') , out);
            printx(((j+1 == size ? (i+1 == size ? "\n\n" :  "\n") : "")), out);
        }
    }
}

/*
 * Display Exit Path - Displays the maze with the exit path found via DFS
 */
void Maze::display_exit_path(std::ostream &out, std::stack<Position> path) const {
    
    // Allocate a temp. 2D array to hold maze display. Assign maze display like above display function
    char **maze = new char *[size + 1];
    for (std::size_t i {0}; i < size; ++i) {
        maze[i] = new char[size];
        for (std::size_t j {0}; j < size; ++j) {
            maze[i][j] = (Position(i, j) == start && start == exitpos) ? 'b' :
                         (Position(i, j) == start)                     ? 's' :
                         (Position(i, j) == exitpos)                   ? 'e' :
                         (M[i][j]        == WALL)                      ? '*' : '|';
        }
        maze[i][size] = '\n';
    }
    
    // Use the exit path held in stack parameter to build a maze display with the exit path
    Position current, adj;
    while (!path.empty()) {
        current = path.top();
        path.pop();
        // Overwrite open positions in path with corresponding arrow char. Don't overwrite start/exit position characters ('s'/'e')
        if (current != start && current != exitpos) {
            adj = path.top();
            maze[current.getRow()][current.getCol()] = (current.Neighbor(DOWN)  == adj)  ? 'v' :
                                                       (current.Neighbor(UP)    == adj)  ? '^' :
                                                       (current.Neighbor(RIGHT) == adj)  ? '>' : '<';
        }
    }
    
    // Output maze display to provided output stream
    for (int i {0}; i < size; ++i) {
        printx(maze[i], out);
    }
    printx('\n', out);
    
    // Delete tmp maze display
    delete[] maze;
}


/*
 * Display Shortest Exit Path - Displays the maze with the shortest exit path found via BFS
 */
void Maze::display_shortest_exit_path(std::ostream &out, Position **pred) const {
    
    // Allocate a temp. 2D array to hold maze display. Assign maze display like above display function
    char **maze = new char *[size + 1];
    
    // Build the maze display based on type of each position
    for (std::size_t i {0}; i < size; ++i) {
        maze[i] = new char[size];
        for (std::size_t j {0}; j < size; ++j) {
            maze[i][j] = (Position(i, j) == start && start == exitpos) ? 'b' :
                         (Position(i, j) == start)                     ? 's' :
                         (Position(i, j) == exitpos)                   ? 'e' :
                         (M[i][j]        == WALL)                      ? '*' : '|';
        }
        maze[i][size] = '\n';
    }
    
    // Print the exit path coordinates and add exit path to maze display using predessecor array
    printPredecessorPath(pred, exitpos, maze);
    
    // Print maze display to provided stream
    printx("\n\n", out);
    for (std::size_t i {0}; i < size; ++i) {
        printx(maze[i], out);
    }
    printx('\n', out);
    
    // Delete maze display
    delete[] maze;
}

/*
 * Print Stack Path - Print the exit path coordinates from stack param obtained via DFS
 */
void Maze::printStackPath(std::stack<Position> S) const {
    
    if (S.empty()) {
        return;
    }
    
    Position target = S.top();
    S.pop();
    printStackPath(S);
    printx(target << " ");
}

/*
 * Print Predecessor Path - Print the shortest exit path coordinates from predecessor param obtained via BFS
 */
void Maze::printPredecessorPath(Position **pred, Position target, char **m) const {
    
    // Base case - exit when target is nullpos
    if (target == NULLPOS) {
        return;
    }
    
    // Get position preceeding target position
    Position p = pred[target.getRow()][target.getCol()];
    
    // Using the target and predecessor positions, assign the corresponding path arrow to the maze display
    if (target != start && target != exitpos) {
        m[target.getRow()][target.getCol()] = (target.getCol() != p.getCol()) ? (target.getCol() - p.getCol() == 1 ? '>' : '<')
                                                                              : (target.getRow() - p.getRow() == 1 ? 'v' : '^');
    }

    // Recursively print exit position coordinates / build maze exit display
    printPredecessorPath(pred, p, m);
    printx(target << " ");
}
