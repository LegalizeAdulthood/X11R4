

/* 
 * This file is used to configure the scan program
 * for various systems.
 */


#undef SUPPORTTED_PLATFORM

#ifdef AIX
#ifndef i386
#define SUPPORTTED_PLATFORM 1
#endif /* #ifndef AIX */
#endif /* #ifndef i386 */

#ifdef ibm032
#define SUPPORTTED_PLATFORM 1
#endif /* #ifndef ibm032 */







