/* ======================================================================================================================= *
 * Dave Dorzback                                                                                                           *
 * COP4530                                                                                                                 *
 * Homework 4 - Position.cpp                                                                                               *
 * ======================================================================================================================= */

#include "Position.h"

/*
 * Get Next Direction (DOWN->LEFT->UP->RIGHT)
 */
Direction next_direction(Direction d) {
    switch(d) {
        case DOWN:  return LEFT;
        case LEFT:  return UP;
        case UP:    return RIGHT;
        default:    return NONE;
    }
}

/*
 * Get Neighbor Position
 */
Position Position::Neighbor(Direction d) const {
    switch(d) {
        case DOWN:  return Position(row + 1, col    );
        case LEFT:  return Position(row,     col - 1);
        case UP:    return Position(row - 1, col    );
        case RIGHT: return Position(row,     col + 1);
        default:    abort();
    }
}

/*
 * Overloaded Equality Operator
 */
bool Position::operator==(const Position &other) const {
    return row == other.row && col == other.col;
}

/*
 * Overloaded Inequality Operator
 */
bool Position::operator!=(const Position &other) const {
    return row != other.row || col != other.col;
}

/*
 * Display Position Coordinates
 */
void Position::display(std::ostream & out) const {
    out << '('<< row << ',' << col << ')';
}

/*
 * Input Position Coordinates
 */
void Position::input(std::istream & in) {
    std::cout << "row index: ";
    in >> row;
    std::cout << "column index: ";
    in >> col;
}

/*
 * Position Friend Func - Overloaded Binary LS Operator (outputs position to stream)
 */
std::ostream& operator<<(std::ostream &out, const Position &P) {
    P.display(out);
    return out;
}

/*
 * Position Friend Func - Overloaded Binary RS Operator (inputs position from stream)
 */
std::istream& operator>>(std::istream &in, Position &P) {
    P.input(in);
    return in;
}
