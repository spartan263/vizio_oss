/*
 * set.h	Macros that look like sigaddset et al. but
 *		aren't. They are used to manipulate bits in
 *		an integer, to do our signal bookeeping.
 */
#define ISMEMBER(set, val) ((set) & (1 << (val)))
#define DELSET(set, val)   ((set) &= ~(1 << (val)))
#define ADDSET(set, val)   ((set) |=  (1 << (val)))
#define EMPTYSET(set)      ((set) = 0)

