/* ======================================================================================================================= *
 * Dave Dorzback                                                                                                           *
 * COP4530                                                                                                                 *
 * Homework 4 - Position.h                                                                                                 *
 * ======================================================================================================================= */

#ifndef __POSITION
#define __POSITION

#include <iostream>

/*
 * Direction Enumeration
 */
enum Direction {
    DOWN,
    LEFT,
    UP,
    RIGHT,
    NONE
};

/*
 * Next Direction - Get next direction from direction d
 */
extern Direction next_direction(Direction d);

/*
 * Position class - Represents rxc grid for maze
 */
class Position {
    
    friend std::ostream& operator<<(std::ostream &out,const Position &P);
    friend std::istream& operator>>(std::istream &in, Position &P);

    public:
        static const std::size_t POSMAX = 22;
    
        Position()                              : row(0),  col(0) {};
        Position(std::size_t m)                 : row(m),  col(m) {};
        Position(std::size_t r, std::size_t c)  : row(r),  col(c) {};
    
        std::size_t getRow() const { return row; }
        std::size_t getCol() const { return col; }
    
        void input();
        Position Neighbor(Direction d) const;
    
        bool operator==(const Position &other) const;
        bool operator!=(const Position &other) const;
    
        void display(std::ostream &out) const;
        void input(std::istream &in);
        
    private:
        std::size_t row;
        std::size_t col;
};

#endif
