
Total characters: 329
Code: int gVar1;int gVar2 = 42;int main() {printf("Hello World!\n");int x;int y = 42;scanf("%d",&x);printf("%d",x);x = y + x;x = x + 1;printf("%d",x);short a = 1;short b = 2;short c = a | b;printf("%d",c);x = x * 2;printf("%d",x);gVar1 = 41;gVar1 = gVar2 - gVar1;gVar1 = ~gVar1;printf("%d",gVar1);printf("Thats all folks\n");return 0;}
0 KW_INT
1 IDENTIFIER
2 SEMI
3 KW_INT
4 IDENTIFIER
5 ASSIGN
6 NUMBER
7 SEMI
8 KW_INT
9 KW_MAIN
10 LEFT_PAREN
11 RIGHT_PAREN
12 LEFT_BRACE
13 KW_PRINTF
14 LEFT_PAREN
15 STRING
16 RIGHT_PAREN
17 SEMI
18 KW_INT
19 IDENTIFIER
20 SEMI
21 KW_INT
22 IDENTIFIER
23 ASSIGN
24 NUMBER
25 SEMI
26 KW_SCANF
27 LEFT_PAREN
28 STRING
29 COMMA
30 AND
31 IDENTIFIER
32 RIGHT_PAREN
33 SEMI
34 KW_PRINTF
35 LEFT_PAREN
36 STRING
37 COMMA
38 IDENTIFIER
39 RIGHT_PAREN
40 SEMI
41 IDENTIFIER
42 ASSIGN
43 IDENTIFIER
44 ADD
45 IDENTIFIER
46 SEMI
47 IDENTIFIER
48 ASSIGN
49 IDENTIFIER
50 ADD
51 NUMBER
52 SEMI
53 KW_PRINTF
54 LEFT_PAREN
55 STRING
56 COMMA
57 IDENTIFIER
58 RIGHT_PAREN
59 SEMI
60 KW_SHORT
61 IDENTIFIER
62 ASSIGN
63 NUMBER
64 SEMI
65 KW_SHORT
66 IDENTIFIER
67 ASSIGN
68 NUMBER
69 SEMI
70 KW_SHORT
71 IDENTIFIER
72 ASSIGN
73 IDENTIFIER
74 OR
75 IDENTIFIER
76 SEMI
77 KW_PRINTF
78 LEFT_PAREN
79 STRING
80 COMMA
81 IDENTIFIER
82 RIGHT_PAREN
83 SEMI
84 IDENTIFIER
85 ASSIGN
86 IDENTIFIER
87 MULT
88 NUMBER
89 SEMI
90 KW_PRINTF
91 LEFT_PAREN
92 STRING
93 COMMA
94 IDENTIFIER
95 RIGHT_PAREN
96 SEMI
97 IDENTIFIER
98 ASSIGN
99 NUMBER
100 SEMI
101 IDENTIFIER
102 ASSIGN
103 IDENTIFIER
104 SUB
105 IDENTIFIER
106 SEMI
107 IDENTIFIER
108 ASSIGN
109 NEG
110 IDENTIFIER
111 SEMI
112 KW_PRINTF
113 LEFT_PAREN
114 STRING
115 COMMA
116 IDENTIFIER
117 RIGHT_PAREN
118 SEMI
119 KW_PRINTF
120 LEFT_PAREN
121 STRING
122 RIGHT_PAREN
123 SEMI
124 KW_RETURN
125 NUMBER
126 SEMI
127 RIGHT_BRACE
C Program (3 declarations)
  Declaration VAR gVar1 INIT (0 <not used> <not used>)
  Declaration VAR gVar2 INIT (42 <not used> <not used>)
  Declaration MAIN
    Block (20 elements)
      Statement PRINTF ("Hello World!\n" <not used> <not used>)
      Declaration VAR x INIT (0 <not used> <not used>)
      Declaration VAR y INIT (42 <not used> <not used>)
      Statement SCANF x
      Statement PRINTF (x <not used> <not used>)
      Statement VAR_ASSIGNMENT x <- (y ADD x)
      Statement VAR_ASSIGNMENT x <- (x ADD 1)
      Statement PRINTF (x <not used> <not used>)
      Declaration VAR a INIT (1 <not used> <not used>)
      Declaration VAR b INIT (2 <not used> <not used>)
      Declaration VAR c INIT (a OR b)
      Statement PRINTF (c <not used> <not used>)
      Statement VAR_ASSIGNMENT x <- (x MULT 2)
      Statement PRINTF (x <not used> <not used>)
      Statement VAR_ASSIGNMENT gVar1 <- (41 <not used> <not used>)
      Statement VAR_ASSIGNMENT gVar1 <- (gVar2 SUB gVar1)
      Statement VAR_ASSIGNMENT gVar1 <- (gVar1 NEG <not used>)
      Statement PRINTF (gVar1 <not used> <not used>)
      Statement PRINTF ("Thats all folks\n" <not used> <not used>)
      Statement RETURN (0 <not used> <not used>)

====================
Pep9 Generated Code
====================

;C Program to Pep9 (3 declarations)
;Declaration
	LDWA 0x0,i
	STWA gVar1,d		; store word from assembler
;Declaration
	LDWA 0x2a,i
	STWA gVar2,d		; store word from assembler
;Declaration
;MAIN
main:	NOP0
;Block (20 elements)
;Statement
	STRO msg0,d		; print string literal from STRO msg0

;Declaration
	LDWA 0x0,i
	STWA x,d		; store word from assembler

;Declaration
	LDWA 0x2a,i
	STWA y,d		; store word from assembler

;Statement
	DECI x,d		; take in decimal input from terminal, store to memory

;Statement
	DECO x,d		; output decimal value to terminal

;Statement
	LDWA y,d
	ADDA x,d
	STWA x,d		; store word from assembler

;Statement
	LDWA x,d
	ADDA 0x1,i

	STWA x,d		; store word from assembler

;Statement
	DECO x,d		; output decimal value to terminal

;Declaration
	LDWA 0x1,i
	STWA a,d		; store word from assembler

;Declaration
	LDWA 0x2,i
	STWA b,d		; store word from assembler

;Declaration
	LDWA a,d
	ORA b,d
	STWA c,d		; store word from assembler

;Statement
	DECO c,d		; output decimal value to terminal

;Statement
	LDWA x,d
	STWA -4,s
	LDWA 0x2,i
	STWA -6,s
	SUBSP 6,i
	CALL multiply
	LDWA 4,s
	STWA resTemp,d
	LDWA 0,i
	STWA 2,s
	STWA 4,s
	ADDSP 6,i
	LDWA resTemp,d

	STWA x,d		; store word from assembler

;Statement
	DECO x,d		; output decimal value to terminal

;Statement
	LDWA 0x29,i
	STWA gVar1,d		; store word from assembler

;Statement
	LDWA gVar2,d
	SUBA gVar1,d
	STWA gVar1,d		; store word from assembler

;Statement
	LDWA gVar1,d
	NOTA
	STWA gVar1,d		; store word from assembler

;Statement
	DECO gVar1,d		; output decimal value to terminal

;Statement
	STRO msg1,d		; print string literal from STRO msg1

;Statement
;Variables/Memory
STOP
gVar1:	.WORD	0x00

gVar2:	.WORD	0x00

msg0:	.ASCII	"Hello World!\n\x00"

x:	.WORD	0x00

y:	.WORD	0x00

a:	.WORD	0x00

b:	.WORD	0x00

c:	.WORD	0x00

resTemp: .BLOCK 2

;********* MULTIPLY **********
retVal:  .EQUATE 12          ;returned value #2d
mult1:   .EQUATE 10          ;formal parameter #2d
mult2:   .EQUATE 8           ;formal parameter #2d
m1Sign:  .EQUATE 5           ;local variable #1d
m2Sign:  .EQUATE 4           ;local variable #1d
k:       .EQUATE 2           ;local variable #2d
mresult: .EQUATE 0           ;local variable; calculated result #2d
multiply:SUBSP   6,i         ;push #m1Sign #m2Sign #k #result
         LDWA    0,i         ;reset possible lingering values in the stack before doing operations
         STWA    mresult,s   ;reset
         LDBA    0,i         ;reset
         STBA    m1Sign,s    ;reset
         STBA    m2Sign,s    ;reset
         LDWA    1,i         ;reset
         STWA    k,s         ;reset
chckM1:  LDWA    mult1,s     ;check mult1 if it is negative
         CPWA    0,i         ;compare
         BRGT    chckM2      ;move on if not
abs1:    NOTA                ;but if so, note that the sign is negative and negate mult1
         ADDA    1,i         ;negate
         STWA    mult1,s     ;negate
         LDBA    1,i         ;note it
         STBA    m1Sign,s    ;note it
chckM2:  LDWA    mult2,s     ;check mult2 if it is negative
         CPWA    0,i         ;
         BRGT    forM        ;move on if not
abs2:    NOTA                ;here, note that the sign is negative and negate mult2
         ADDA    1,i         ;negate
         STWA    mult2,s     ;negate
         LDBA    1,i         ;note it
         STBA    m2Sign,s    ;note it
forM:    LDWA    k,s         ;load k for comparison if not loaded already, to see if we are done looping yet
         CPWA    mult2,s     ;see if k <= mult2, which means we have added mult1 to itself mult2 times
         BRGT    endForM     ;if so, we're done! branch to endForM
         LDWA    mresult,s   ;if not, we'll keep going! load the current added result to keep adding
         ADDA    mult1,s     ;add mult1 again
         STWA    mresult,s   ;store it to result
         LDWA    k,s         ;load k to add one to it
         ADDA    1,i         ;add one to it so we can see when we reach mult2 and stop adding
         STWA    k,s         ;store it to k
         BR      forM        ;do the loop again!
endForM: LDBA    m1Sign,s    ;check if the signs of each num are different
         CPBA    m2Sign,s    ;if so, we set the result as negative
         BREQ    endForM2    ;
         LDWA    mresult,s   ;
         NOTA                ;
         ADDA    1,i         ;
         STWA    mresult,s   ;
endForM2:LDWA    mresult,s   ;load result and store to result variable
         STWA    retVal,s    ;
         LDWA    0,i         ;cleanup
         STWA    k,s         ;reset values
         STWA    mresult,s   ;reset
         LDBA    0,i         ;reset
         STBA    m1Sign,s    ;reset
         STBA    m2Sign,s    ;reset
         ADDSP   6,i         ;pop #result #k #m1Sign #m2Sign
         RET

msg1:	.ASCII	"Thats all folks\n\x00"

.end
