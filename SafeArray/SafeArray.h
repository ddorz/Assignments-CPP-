/* ======================================================================================================================= *
 * Dave Dorzback                                                                                                           *
 * COP4530 - Data Structures                                                                                               *
 * Homework 2 - SafeArray.h                                                                                                *
 * ======================================================================================================================= */

#ifndef SafeArray_h
#define SafeArray_h

#include <iostream>
#include <string>
#include <cstdlib>
#include <cstddef>

template <class Type>
class SafeArray {
public:

    // Constructor 1 (invalid index parameter only):
    SafeArray(int first_invalid_idx);

    // Constructor 2 (valid index & invalid index parameters):
    SafeArray(int first_valid_idx, int first_invalid_idx);

    // Constructor 2 (copy constructor):
    SafeArray(SafeArray const &otherSafeArray);

    // Overloaded subscript operator 1 (for non-const objects, can be used for access/assignment):
    Type& operator[] (int const idx);

    // Overloaded subscript operator 2 (for const objects, can be used for access only):
    Type const& operator[](int const idx) const;

    // Overloaded assignment operator:
    SafeArray<Type> const& operator= (SafeArray const &otherSafeArray);

    // Get length of SafeArray array
    std::size_t length(void);

    // Print function:
    void print();

    // Destructor:
    ~SafeArray();

private:

    // First valid index member variable:
    int first_valid;

    // First invalid index member variable:
    int first_invalid;

    // Pointer to Type member to be allocated for array:
    Type *A;

};


/***************************************************************************************************************************************************************/


/*
 * Constructor 1 - first valid index parameter only:
 */
template <class Type>
SafeArray<Type>::SafeArray(int first_invalid_idx) {

    // Set index member variables
    first_valid   = 0;
    first_invalid = first_invalid_idx;

    // Allocate SafeArray. Absolute value used to allow both positive and negative values for first invalid index (e.g. -9 ==> 0 or 0 ==> 9)
    A = new Type[std::abs(first_invalid)];
}

/*
 * Constructor 2 - first valid & first invalid index parameters:
 */
template <class Type>
SafeArray<Type>::SafeArray(int first_valid_idx, int first_invalid_idx) {

    // Set indexes
    first_valid   = first_valid_idx;
    first_invalid = first_invalid_idx;

    // Verify the index values are valid. If first index values are equal, print error and exit program
    if (first_valid == first_invalid) {
        std::cout << "SafeArray: first valid index ("     << first_valid   << ") "
                  << "cannot equal first invalid index (" << first_invalid << "). \n";
        std::exit(EXIT_FAILURE);
    }

    // If indexes are valid, allocate the proper amount of space by getting the SafeArray length
    // Since length always returns a positive value, the only instance where parameter values are not valid is when they are equal
    A = new Type[length()];
}

/*
 * Constructor 3 - Copy Constructor:
 */
template <class Type>
SafeArray<Type>::SafeArray(SafeArray const &otherSafeArray) {

    // Copy the index member variables from otherSafeArray object
    first_valid   = otherSafeArray.first_valid;
    first_invalid = otherSafeArray.first_invalid;

    // Allocate the proper amount of space
    A = new Type[length()];

    // Copy all elements from otherSafeArray to new object
    for (std::size_t i {0}, max {length()}; i < max; ++i) {
        A[i] = otherSafeArray.A[i];
    }
}

/*
 * Overloaded subscript operator (non-const objects, access/assignment):
 */
template <class Type>
Type& SafeArray<Type>::operator[](int const idx) {

    // Verify the index parameter is valid. If invalid, save the corresponding result to string (e.g. 'too small' or 'too large')
    std::string error_str = (first_valid > first_invalid && idx <= first_invalid) || (first_invalid > first_valid && idx < first_valid)    ? "too small." :
                            (first_valid > first_invalid && idx  > first_valid)   || (first_invalid > first_valid && idx >= first_invalid) ? "too large." : "";

    // If the index is invalid, print corresponding error message and exit program
    if (!error_str.empty()) {
        std::cout << "SafeArray index " << idx << " out of bounds: " << error_str << std::endl;;
        std::exit(EXIT_FAILURE);
    }

    // Otherwise, return the proper element from the SafeArray array
    return A[std::abs(idx - first_valid)];
}

/*
 * Overloaded subscript operator (const objects, access only):
 */
template <class Type>
Type const& SafeArray<Type>::operator[](int const idx) const {

    // Verify the index parameter is valid. If invalid, save the corresponding result to string (e.g. 'too small' or 'too large')
    std::string error_str = (first_valid > first_invalid && idx <= first_invalid) || (first_invalid > first_valid && idx < first_valid)    ? "too small." :
                            (first_valid > first_invalid && idx  > first_valid)   || (first_invalid > first_valid && idx >= first_invalid) ? "too large." : "";

    // If the index is invalid, print corresponding error message and exit program
    if (!error_str.empty()) {
        std::cout << "SafeArray index " << idx << " out of bounds: " << error_str << std::endl;;
        std::exit(EXIT_FAILURE);
    }

    // Otherwise, return the proper element from the SafeArray array
    return A[std::abs(idx - first_valid)];
}

/*
 * Overloaded assignment operator:
 */
template <class Type>
SafeArray<Type> const& SafeArray<Type>::operator=(SafeArray const &otherSafeArray) {

    // Deallocate the current SafeArray array member
    delete[] A;

    // Copy first_valid / first_invalid members from otherSafeArray object to new SafeArray object
    first_valid   = otherSafeArray.first_valid;
    first_invalid = otherSafeArray.first_invalid;

    // Get length of SafeArray and save it to max
    std::size_t max = length();

    // Allocate SafeArray array using the max value calculated above
    A = new Type[max];

    // Copy all elements from the otherSafeArray object's array into new SafeArray array
    for (std::size_t i {0}; i < max; ++i) {
        A[i] = otherSafeArray.A[i];
    }

    // Return the copied SafeArray object
    return *this;
}

/*
 * Length function:
 */
template <class Type>
std::size_t SafeArray<Type>::length(void) {

    // Return absolute value of first_invalid - first_valid
    // Absolute value taken to ensure a positive length when first_valid > first_invalid
    return std::abs(first_invalid - first_valid);
}

/*
 * Print SafeArray function:
 */
template <class Type>
void SafeArray<Type>::print(void) {

    // Calculate the number of elements in SafeArray array
    std::size_t max = length();

    // Print each element on its own line
    for (std::size_t i {0}; i < max; ++i) {
        std::cout << A[i] << std::endl;
    }
}

/*
 * Destructor:
 */
template <class Type>
SafeArray<Type>::~SafeArray(void) {

    // Deallocate SafeArray array
    delete[] A;
}

#endif /* End SafeArray_h */
