/*****************************************************************************
* 
*			  NCSA HDF version 3.00
*				December, 1989
*
* NCSA HDF Version 3.00 source code and documentation are in the public
* domain.  Specifically, we give to the public domain all rights for future
* licensing of the source code, all resale rights, and all publishing rights.
* 
* We ask, but do not require, that the following message be included in all
* derived works:
* 
* Portions developed at the National Center for Supercomputing Applications at
* the University of Illinois at Urbana-Champaign.
* 
* THE UNIVERSITY OF ILLINOIS GIVES NO WARRANTY, EXPRESSED OR IMPLIED, FOR THE
* SOFTWARE AND/OR DOCUMENTATION PROVIDED, INCLUDING, WITHOUT LIMITATION,
* WARRANTY OF MERCHANTABILITY AND WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE
* 
* NCSA HDF Group:
*  Mike Folk, Swami Natarajan, Chin_Chau Low, Likkai Ng, Peter Webb,
*  Brian Calvert and Drew Hess.
*****************************************************************************/

#include "df.h"

union fpx
{
     float f;
     long  l;
};

union float_uint_uchar {
    float32 f;
    unsigned int32 i;
    unsigned char c[4];
};

DFIstrncpy(dest, source, len)
char *source, *dest;
int len;
{

    for(; (--len > 0) && (*dest++ = *source++););
    if (!len) *dest = '\0';
}


int DFconvert(source, dest, ntype, sourcetype, desttype)
char *source, *dest;
int ntype, sourcetype, desttype;
{
    char tmp[4];		/* size of float32 */

    if (ntype==DFNT_FLOAT) {

	if (((sourcetype==DFNTF_IEEE) && ( desttype==DFNTF_PC)) ||
	    ((sourcetype==DFNTF_PC) && (desttype==DFNTF_IEEE))) {
	    dest[0]=source[3];
	    dest[1]=source[2];
	    dest[2]=source[1];
	    dest[3]=source[0];
	    return(0);
	}

	/* if reversed IEEE convert first */
	if (sourcetype==DFNTF_PC) {
	    tmp[0]=source[3];
	    tmp[1]=source[2];
	    tmp[2]=source[1];
	    tmp[3]=source[0];
	    source=tmp;
	    sourcetype=DFNTF_IEEE;
	}

        if ((sourcetype==DFNTF_IEEE) && (desttype==DFNTF_CRAY)) {
            if (DFCVieeeF2unicosF(source,(union fpx*) dest)<0) return(-1);
        } else if ((sourcetype==DFNTF_CRAY) &&
		   ((desttype==DFNTF_IEEE) || (desttype==DFNTF_PC))) {
            if (DFCVunicosF2ieeeF((union fpx*)source, dest)<0) return(-1);
        } else if ((sourcetype==DFNTF_IEEE) && (desttype==DFNTF_VAX)) {
            if (DFCVieeeF2vaxF((union float_uint_uchar *) source,
			       (union float_uint_uchar *) dest)<0)
		return(-1);
        } else if ((sourcetype==DFNTF_VAX) &&
		   ((desttype==DFNTF_IEEE) || (desttype==DFNTF_PC))) {
            if (DFCVvaxF2ieeeF((union float_uint_uchar*) source,
			       (union float_uint_uchar*) dest)<0)
		return(-1);
        }

	/* if reversed IEEE convert */
	if (desttype==DFNTF_PC) {
	    char t;
	    t=dest[3];
	    dest[3]=dest[0];
	    dest[0]=t;
	    t=dest[2];
	    dest[2]=dest[1];
	    dest[1]=t;
	}
	return(0);
    }
        /* default */
    DFerror = DFE_BADCONV;
    return(-1);
}

char *DFIgetspace(qty)
unsigned qty;
{
    char *p;

    p = malloc(qty);
    if (p==NULL) {
        DFerror = DFE_NOSPACE;
        return(NULL);
    }
    return(p);
}

char *DFIfreespace(ptr)
char *ptr;
{
    if (ptr!=NULL) free(ptr);
    return(NULL);
}


#ifdef UNICOS

#define MINEXP    0x3f81000000000000  /* min valid Cray masked exponent */
#define MAXEXP    0x407e000000000000  /* max valid Cray masked exponent */

#define C_FMASK   0x00007fffff000000  /* Cray fraction mask (1st 23 bits)*/
#define C_EMASK   0x7fff000000000000  /* Cray exponent mask */
#define C_SMASK   0x8000000000000000  /* Cray sign mask */
#define C_IMPLICIT 0x0000800000000000 /* Cray implicit bit */

#define I_FMASK   0x007fffff          /* IEEE fraction mask */
#define I_EMASK   0x7f800000          /* IEEE exponent mask */
#define I_SMASK   0x80000000          /* IEEE sign mask     */

#define IEEE_BIAS 0177
#define CRAY_BIAS 040000

static long C2I_diff;
static long I2C_diff;

#endif /*UNICOS*/


    /*  convert from Cray2 floating point format to IEEE format */
/* shut lint up */
/* ARGSUSED */
int DFCVunicosF2ieeeF(cray_fp, ieee_fp)
union fpx *cray_fp;
char *ieee_fp;
{

#ifdef UNICOS
    long tmp;

    if (cray_fp->l == 0) {
        tmp = 0;
    }
         
    else {
        tmp = (C_EMASK & cray_fp->l);
        if (tmp < MINEXP || tmp > MAXEXP) {
            DFerror = DFE_BADFP;
            return(-1);
        }

        C2I_diff = (IEEE_BIAS - CRAY_BIAS - 1) << 48;
        tmp = (( tmp + C2I_diff ) << 7)
            | ( (cray_fp->l & C_FMASK) << 8 )
            | ( (cray_fp->l & C_SMASK));
    }
    DFmovmem(&tmp, ieee_fp, 4);
    return(0);

#else /*UNICOS*/
    DFerror = DFE_BADCONV;
    return(-1);
#endif /*UNICOS*/

}

 
/* Conversion from IEEE floating point format to Cray format */
 
/* shut lint up */
/* ARGSUSED */
int DFCVieeeF2unicosF(ieee_fp, cray_fp)
union fpx *cray_fp;
char *ieee_fp;
{

#ifdef UNICOS
    long tmp;

    tmp = 0;
    DFmovmem(ieee_fp, ((char *) &tmp) + 4, 4);      /* right 4 bytes of tmp */

    if ( (cray_fp->l = tmp & I_EMASK) == 0) {
        cray_fp->l = 0;
        return(0);
    }

    I2C_diff = (CRAY_BIAS - IEEE_BIAS + 1) << 23;
    cray_fp->l += I2C_diff;
    cray_fp->l = (cray_fp->l<< 25)  | ( (tmp & I_FMASK) << 24)
       | ( (tmp & I_SMASK) << 32) | C_IMPLICIT;
    return (0);

#else /*UNICOS*/
    DFerror = DFE_BADCONV;
    return(-1);
#endif /*UNICOS*/

}


DFIc2fstr(str, len)
char* str;
int len;
{
    int i;

    for(i=0; (str[i]); i++);
    for(; i<len; i++) str[i] = ' ';
}

char *DFIf2cstring(fstr, len)
#ifdef UNICOS
    _fcd fstr;
    int len;
{
    char *cstr, *str;
    int i;

    str = _fcdtocp(fstr);
    cstr = DFIgetspace(len+1);
    for(i=0;i<len && (cstr[i] = str[i]) != ' '; i++);
    cstr[i] = '\0';
    return cstr;
}
#else /*UNICOS*/
    char *fstr;
    int len;
{
    char *cstr;
    int i;

    cstr = DFIgetspace((unsigned) len + 1);
    for(i=0;(i<len)&&((cstr[i]=fstr[i]) != ' ');i++);
    cstr[i] = '\0';
    return cstr;
}
#endif /*UNICOS*/

int DFCVvaxF2ieeeF(in, out)
union float_uint_uchar *in, *out;
{
    register unsigned char exp;

    exp = (in->c[1] << 1) | (in->c[0] >> 7);    /* extract exponent */
    if (!exp && !in->c[1]) out->i = 0;          /* zero value */
    else if (exp>2) {                           /* normal value */
        out->c[0] = in->c[1] - 1;   /* actually subtracts 2 from exponent */
                /* copy mantissa, LSB of exponent */
        out->c[1] = in->c[0];
        out->c[2] = in->c[3];
        out->c[3] = in->c[2];
    }
    else if (exp) {                             /* denormalized number */
        register int shft;

        out->c[0] = in->c[1] & 0x80;            /* keep sign, zero exponent */
        shft = 3 - exp;
            /* shift original mant by 1 or 2 to get denormalized mant */
            /* prefix mantissa with '1'b or '01'b as appropriate */
        out->c[1] = ((in->c[0] & 0x7f) >> shft) | (0x10 << exp);
        out->c[2] = (in->c[0] << (8-shft)) | (in->c[3] >> shft);
        out->c[3] = (in->c[3] << (8-shft)) | (in->c[2] >> shft);
    }
    else {                                      /* sign=1 -> infinity or NaN */
        out->c[0] = 0xff;                       /* set exp to 255 */
                /* copy mantissa */
        out->c[1] = in->c[0] | 0x80;            /* LSB of exp = 1 */
        out->c[2] = in->c[3];
        out->c[3] = in->c[2];
    }
    return(0);
}


int DFCVieeeF2vaxF(in, out)
union float_uint_uchar *in, *out;
{
    register unsigned char exp;

    exp = (in->c[0] << 1) | (in->c[1] >> 7);    /* extract exponent */
    if (exp) {                                  /* non-zero exponent */
            /* copy mantissa, last bit of exponent */
        out->c[0] = in->c[1];
        out->c[2] = in->c[3];
        out->c[3] = in->c[2];
        if (exp<254)                            /* normal value */
            out->c[1] = in->c[0] + 1;           /* actually adds two to exp */
        else {                                  /* infinity or NaN */
            if (exp==254)                       /* unrepresentable - OFL */
                out->i = 0;                     /* set mant=0 for overflow */
            out->c[0] &= 0x7f;                  /* set last bit of exp to 0 */
            out->c[1] = 0x80;                   /* sign=1 exp=0 -> OFL or NaN */
        }
    }
    else if (in->c[1] & 0x60) {                 /* denormalized value */
        register int shft;
    
        shft = (in->c[1] & 0x40) ? 1 : 2;       /* shift needed to normalize */
            /* shift mantissa */
            /* note last bit of exp set to 1 implicitly */
        out->c[0] = (in->c[1] << shft) & (in->c[2] >> (8-shft));
        out->c[3] = (in->c[2] << shft) & (in->c[3] >> (8-shft));
        out->c[2] = in->c[3] << shft;
        out->c[1] = (in->c[0] & 0x80);          /* sign */
        if (shft==1) {                          /* set exp to 2 */
            out->c[1] |= 0x01;
            out->c[0] &= 0x7f;                  /* set LSB of exp to 0 */
        }
    }
    else out->i = 0;                            /* zero */

    return(0);
}
