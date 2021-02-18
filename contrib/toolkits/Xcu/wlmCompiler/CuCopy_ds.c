/*
 * Given the address of a pointer to a target character string, and
 * a string to be copied, this function allocates space for the target
 * and copies the string into the target.
 */

void
Cu_copy_ds (destination, source)
    char **destination ;
    char *source ;
{
char *temp = source ;
*destination = XtMalloc ((strlen(temp)+1) * sizeof (char)) ;
strcpy (*destination, temp) ;

return ;
}


