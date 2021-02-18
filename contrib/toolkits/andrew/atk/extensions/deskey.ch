/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* deskey.H -- EZ package to describe key sequences.
 * David Nichols
 * September, 1987 
 */

package deskey {
    classprocedures:
	InitializeClass() returns boolean;
	DescribeKey(char *keys, long *pRock) returns struct proctable_Entry *;
		/* search keystate for the keys and return the proctable entry
		   set pRock to the rock value that will be used */
};
