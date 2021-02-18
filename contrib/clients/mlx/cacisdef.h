
#define		END	0xff
#define		MAX_RAD	37

typedef	unsigned short int Hanzi ;	/*CHINESE CHARACTER CODE*/

typedef struct 	{
                 Hanzi code; 		/* GBcode for Stroke */
		 Hanzi *index;		/* point to radicals'order no */
		} Stroke;

typedef struct 	{
                 Hanzi code	; 	/* GCcode for radical */
		 short int size	;	/* character no. */
		} Radical;

typedef struct	{
                 Hanzi code    ; 	/* GCcode */
               	 Hanzi alter[5]; 	/* alternate radicals' GCcode*/
		} AltRad;

typedef	struct	{
		 Hanzi	code	;	/* Order no in char.tbl file */
		 char	cnt	;	/* erpeat count */
		} Char;

