/* ======================================================================================================================= *
 * Dave Dorzback                                                                                                           *
 * COP4530                                                                                                                 *
 * Homework 4 - PrintMacros.h                                                                                              *
 * ======================================================================================================================= */

#ifndef PrintMacros_h
#define PrintMacros_h

/** ======== println/printx - Print/Display Macros ======== **/

/*- println - Print newline(s) / with newlines -*/
#define println(...) _println_x(, ##__VA_ARGS__ ,          \
    _println_3(__VA_ARGS__), _println_2(__VA_ARGS__),      \
    _println_1(__VA_ARGS__), _println_0(__VA_ARGS__)       \
)

/*- printx - Print var to cout / to other stream -*/
#define printx(...) _printx_x(, ##__VA_ARGS__ ,            \
    _printx_2(__VA_ARGS__), _printx_1(__VA_ARGS__)         \
)

/*** ========= println/printx - Utility Macros ========== ***/
/*- println: 0-3 arguments: -*/
#define _println_x(x, s, t, u, FUNC, ...) FUNC

// 0 - Print single newline
#define _println_0() (printx(std::endl))

// 1 - Print 's' newlines
#define _println_1(s) do {                                 \
for (int i {0}; i < s; ++i)                                \
    _println_0();                                          \
} while (0)

// 2 - Print 's' newlines then 't' on current line
#define _println_2(s,t) do {                               \
    _println_1(s);                                         \
    printx(t);                                             \
} while (0)

// 3 - Print 's' newlines, then 't', then 'u' newlines
#define _println_3(s,t,u) do {                             \
    _println_2(s,t);                                       \
    _println_1(u);                                         \
} while (0)

/*- printx: 1-2 arguments: -*/
#define _printx_x(x, s, t, FUNC, ...) FUNC

// 1 arg - Print 's' to cout
#define _printx_1(s) (std::cout << s)

// 2 args - Print 's' to output stream 't'
#define _printx_2(s, t) (t << s)

/** ====================================================== **/

#endif /* PrintMacros_h */
