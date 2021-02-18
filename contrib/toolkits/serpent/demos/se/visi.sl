/* $Log:	visi.sl,v $
% Revision 1.1  89/10/10  22:13:23  serpent
% Initial revision
% 
Revision 1.3  89/06/19  16:28:57  ljb
fix up rcs header
  */
/*  
   this file defines the visible abstraction for inclusion in a
   slang program.  Mapped_when_managed is used to determine whether
   an object is visible.  If the current page is the same as the parameter
   page then it is set to true, otherwise false
*/

#define make_visible(fpage)\
   mapped_when_managed: {\
     if (page = fpage) then\
         mapped_when_managed := TRUE;\
     else\
         mapped_when_managed := FALSE;\
     end if;\
   }
