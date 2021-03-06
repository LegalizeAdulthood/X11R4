/*
** Xtank
**
** Copyright 1988 by Terry Donahue
**
** linetext.h
*/

#define MAX_FPOINTS 6
#define MAX_FLINES  5
#define MAX_LINECHARS  26

typedef char Num;

typedef struct {
  Num x;
  Num y;
} Fpoint;

typedef struct {
  Num start;
  Num end;
} Fline;

typedef struct {
  Num num_points;
  Fpoint point[MAX_FPOINTS];
  Num num_lines;
  Fline line[MAX_FLINES];
} Linechar;

static Linechar linechar[MAX_LINECHARS] = {
  { 5, {{0,-30},{-10,0},{10,0},{-20,3},{20,30}},              /* A */
    3, {{0,3},{0,4},{1,2}} },
  { 5, {{-20,-30},{20,-15},{-20,0},{20,15},{-20,30}},         /* B */
    5, {{0,1},{1,2},{2,3},{3,4},{4,0}} },
  { 3, {{20,-30},{-20,0},{20,30}},                            /* C */
    2, {{0,1},{1,2}} },
  { 3, {{-20,-30},{20,0},{-20,30}},                           /* D */
    3, {{0,1},{1,2}} },
  { 4, {{20,-30},{-20,0},{20,0},{20,30}},                     /* E */
    3, {{0,1},{1,2},{1,3}} },
  { 5, {{-20,-30},{20,-30},{-20,0},{10,0},{-20,30}},          /* F */
    3, {{0,1},{0,4},{2,3}} },
  { 5, {{0,-30},{-20,0},{0,0},{20,0},{0,30}},                 /* G */
    4, {{0,1},{1,4},{4,3},{3,2}} },
  { 6, {{-20,-30},{20,30},{20,0},{20,0},{-20,30},{20,30}},    /* H */
    3, {{0,4},{1,5},{2,3}} },
  { 2, {{0,-30},{0,30}},                                      /* I */
    1, {{0,1}} },
  { 3, {{20,-30},{-20,0},{20,30}},                            /* J */
    2, {{0,2},{1,2}} },
  { 5, {{-20,-30},{20,-30},{-20,0},{-20,30},{20,30}},         /* K */
    3, {{0,3},{1,2},{2,4}} },
  { 3, {{-20,-30},{-20,30},{20,30}},                          /* L */
    2, {{0,1},{1,2}} },
  { 5, {{-20,-30},{20,-30},{0,0},{-20,30},{20,30}},           /* M */
    4, {{0,3},{0,2},{1,2},{1,4}} },
  { 4, {{-20,-30},{20,-30},{-20,30},{20,30}},                 /* N */
    3, {{0,2},{0,3},{1,3}} },
  { 4, {{0,-30},{-20,0},{20,0},{0,30}},                       /* O */
    4, {{0,1},{0,2},{1,3},{2,3}} },
  { 4, {{-20,-30},{20,-15},{-20,0},{-20,30}},                 /* P */
    3, {{0,3},{0,1},{1,2}} },
  { 6, {{0,-30},{-20,0},{0,0},{20,0},{0,30},{20,30}},         /* Q */
    5, {{0,1},{0,2},{1,4},{3,4},{2,5}} },
  { 5, {{-20,-30},{20,-15},{-20,0},{-20,30},{20,30}},         /* R */
    4, {{0,3},{0,1},{1,2},{2,4}} },
  { 6, {{0,-30},{-20,-15},{20,-15},{-20,15},{20,15},{0,30}},  /* S */
    5, {{0,2},{0,1},{1,4},{4,5},{5,3}} },
  { 4, {{-20,-30},{0,-30},{20,-30},{0,30}},                   /* T */
    2, {{0,2},{1,3}} },
  { 4, {{-20,-30},{20,-30},{-20,30},{20,30}},                 /* U */
    3, {{0,2},{2,3},{3,1}} },
  { 3, {{-20,-30},{20,-30},{0,30}},                           /* V */
    2, {{0,2},{2,1}} },
  { 5, {{-20,-30},{20,-30},{0,0},{-20,30},{20,30}},           /* W */
    4, {{0,3},{3,2},{2,4},{4,1}} },
  { 4, {{-20,-30},{20,-30},{-20,30},{20,30}},                 /* X */
    2, {{0,3},{1,2}} },
  { 4, {{-20,-30},{20,-30},{0,0},{0,30}},                     /* Y */
    3, {{0,2},{1,2},{2,3}} },
  { 4, {{-20,-30},{20,-30},{-20,30},{20,30}},                 /* Z */
    3, {{0,1},{1,2},{2,3}} }
};

