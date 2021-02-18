/* 
 *  This is a simple way to maintain a dynamically allocated array that
 *  grows in chunks. Essentially, obj is a a pointer to the object
 *  (starts off as NULL), and count is the number of elements in it.
 *  size is the currently allocated size, and incr is the size to
 *  increase it by if count == size. type is the type of the object - it
 *  callocs size*sizeof(type) elements for it, and casts this to (type
 *  *). Type is the reason this is a macro and not a procedure.
 *  basically you call this before you store something in obj[count].
 *  Greate for adding things at the end of lists
 */
/* Mark Moraes */
#define test_and_grow_object(obj, count, size, incr, type)\
	if((count) == (size) || (obj) == NULL) { \
		size += (incr);	\
		if ((obj) == NULL) \
			obj = (type *) XtCalloc(size, sizeof(type));\
		else \
			obj = (type *) XtRealloc((char *) obj, \
			 (size)*sizeof(type));\
	} else

