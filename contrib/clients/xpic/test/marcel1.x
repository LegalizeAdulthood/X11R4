24 53 616 792 8
6 104 182 337 218 351 0 0
200 344 5 0 3
uCode
6 103 24 161 47 175 1000 0
24 168 3 1 3
IFU
6 102 187 313 213 327 0 0
200 320 3 0 3
IDU
6 101 108 329 164 343 0 0
136 336 8 0 3
EU uCode
6 100 136 329 136 343 0 0
136 336 0 0 3

6 99 141 297 180 311 0 0
160 304 4 0 3
uROM
1 98 176 312 176 368 0 1
2
 176 368 176 312
6 97 322 537 366 551 0 0
344 544 8 0 3
(2-port)
6 96 310 513 378 527 0 0
344 520 13 0 3
Register File
6 95 326 497 362 511 0 0
344 504 4 0 3
BASE
6 94 192 514 240 527 0 0
216 520 9 0 2
Increment
6 93 424 514 472 527 0 0
448 520 9 0 2
Increment
6 92 442 498 454 511 0 0
448 504 2 0 2
SP
6 91 209 498 223 511 0 0
216 504 2 0 2
PC
6 90 185 410 215 423 0 0
200 416 5 0 2
Check
6 89 190 394 211 407 0 0
200 400 5 0 2
Stall
3 88 176 392 224 424 0 1
6 87 109 739 164 750 0 0
136 744 15 0 1
Decoded Instr 3
6 86 109 619 164 630 0 0
136 624 15 0 1
Decoded Instr 2
3 85 88 736 184 752 0 1
1 84 136 592 216 592 20 1
2
 136 592 216 592
1 83 136 664 216 664 20 1
2
 136 664 216 664
3 82 88 616 184 632 0 1
3 81 88 448 184 464 0 1
6 80 448 450 464 463 0 0
456 456 3 0 2
sop
6 79 99 451 174 462 0 0
136 456 19 0 1
Decoded Instruction
6 78 355 450 382 463 0 0
368 456 7 0 2
field 2
6 77 267 450 294 463 0 0
280 456 7 0 2
field 1
6 76 264 250 329 263 0 0
296 256 13 0 2
Decode Buffer
6 75 295 186 426 199 0 0
360 192 30 0 2
Instruction Queue  (7x18 bits)
3 74 256 176 472 208 0 1
6 73 273 130 367 143 0 0
320 136 22 0 2
PC Stack  (16x30 bits)
3 72 256 120 384 152 0 1
6 71 136 141 171 163 1000 0
136 152 3 0 6
IFU
6 70 112 173 198 195 1000 0
112 184 10 0 6
Controller
6 69 392 373 452 395 1000 0
392 384 5 0 6
(ODL)
6 68 312 373 359 395 1000 0
312 384 5 0 6
Logic
6 67 312 341 374 363 1000 0
312 352 6 0 6
Decode
6 66 312 309 387 331 1000 0
312 320 7 0 6
Operand
6 65 272 685 319 707 1000 0
272 696 5 0 6
Logic
1 64 312 264 312 288 20 1
2
 312 264 312 288
6 63 272 653 311 675 1000 0
272 664 4 0 6
Unit
6 62 272 621 357 643 1000 0
272 632 9 0 6
Execution
6 61 538 761 582 775 0 0
560 768 8 0 3
(2-port)
6 60 543 729 577 743 0 0
560 736 5 0 3
Cache
6 59 545 705 575 719 0 0
560 712 5 0 3
Stack
6 58 571 497 598 511 0 0
584 504 3 0 3
BUS
6 57 560 529 609 543 0 0
584 536 9 0 3
Interface
1 56 232 80 432 80 0 1
2
 232 80 432 80
6 55 231 53 425 75 0 0
328 64 19 1 7
SLIME  Architecture
6 54 24 321 49 335 1000 0
24 328 3 1 3
IDU
6 53 24 753 50 767 1000 0
24 760 4 1 3
EU 3
6 52 24 657 50 671 1000 0
24 664 4 1 3
EU 2
6 51 24 513 50 527 1000 0
24 520 4 1 3
EU 1
1 50 392 528 408 528 20 1
2
 408 528 392 528
1 49 280 528 296 528 20 1
2
 280 528 296 528
1 48 224 408 240 408 20 1
2
 240 408 224 408
1 47 312 208 312 248 20 1
2
 312 208 312 248
1 46 280 208 280 248 20 1
2
 280 208 280 248
1 45 192 368 256 376 20 1
3
 192 368 192 376 256 376
1 44 280 464 280 576 20 1
2
 280 464 280 576
1 43 456 408 456 448 20 1
2
 456 408 456 448
1 42 376 408 376 448 20 1
2
 376 408 376 448
1 41 280 408 280 448 20 1
2
 280 408 280 448
1 40 216 272 280 328 20 1
4
 280 272 248 272 248 328 216 328
1 39 280 264 280 288 20 1
2
 280 264 280 288
1 38 456 728 512 728 30 1
2
 456 728 512 728
1 37 456 712 512 712 30 1
2
 456 712 512 712
1 36 248 512 296 512 30 1
2
 248 512 296 512
1 35 136 512 192 512 20 1
2
 136 512 192 512
1 34 176 208 240 496 20 1
4
 240 496 240 232 176 232 176 208
1 33 168 208 232 400 20 1
5
 224 400 232 400 232 240 168 240
 168 208
3 32 192 496 248 536 0 1
1 31 136 368 136 448 20 1
2
 136 368 136 448
1 30 136 752 216 760 20 1
3
 136 752 136 760 216 760
1 29 136 632 136 736 20 1
2
 136 632 136 736
1 28 136 464 136 616 20 1
2
 136 464 136 616
1 27 456 464 456 496 20 1
2
 456 464 456 496
1 26 408 464 408 576 20 1
2
 408 464 408 576
1 25 472 192 528 568 20 1
3
 472 192 528 192 528 568
1 24 528 568 552 568 30 1
2
 528 568 552 568
1 23 528 568 528 688 30 1
2
 528 688 528 568
1 22 456 632 528 632 30 1
2
 456 632 528 632
1 21 392 512 424 512 30 1
2
 392 512 424 512
1 20 368 552 368 576 30 1
2
 368 552 368 576
1 19 320 552 320 576 30 1
2
 320 552 320 576
1 18 216 184 256 184 30 1
2
 216 184 256 184
1 17 216 136 256 136 30 1
2
 216 136 256 136
3 16 552 472 616 640 0 1
3 15 512 688 608 792 0 1
3 14 96 128 216 208 0 1
3 13 256 248 336 264 0 1
3 12 256 288 472 408 0 1
3 11 96 288 216 368 0 1
3 10 256 448 304 464 0 1
3 9 328 448 416 464 0 1
3 8 440 448 472 464 0 1
3 7 296 488 392 552 0 1
3 6 424 496 480 536 0 1
3 5 216 576 456 768 0 1
3 4 72 104 496 792 1 1
1 3 72 232 496 232 1 1
2
 72 232 496 232
1 2 72 432 496 432 1 1
2
 72 432 496 432
1 1 72 600 496 600 2 1
2
 72 600 496 600
1 0 72 720 496 720 2 1
2
 72 720 496 720
