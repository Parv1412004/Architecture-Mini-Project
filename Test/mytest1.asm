; TITLE: mytest1																											
; Name:	Parv Gadhethariya
; ROLL NO:	2201CS91
; Declaration of Authorship
; This asm file mytest1.asm, is a part of the miniproject of CS210.
; This program stores the sum of four variables in the main memory.

main:	ldc el0
		ldnl 0
		add
		ldc el1
		ldnl 0
		add
		ldc el2
		ldnl 0
		add
		ldc el3
		ldnl 0
		add
		ldc 2
		shr
		adj -1
		stl 0
		ldl 0
		br 0
		HALT
el0: data 1
el1: data 2
el2: data 3
el3: data 4
