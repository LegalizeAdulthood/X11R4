/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

#define search_VERSION 1

package search {
classprocedures:
    CompilePattern(char *string, struct SearchPattern **result) returns char *;
    MatchPattern(struct text *d, long pos, struct SearchPattern *p) returns int;
    MatchPatternReverse(struct text *d, long pos, struct SearchPattern *p) returns int;
    GetMatchLength() returns int;
};
