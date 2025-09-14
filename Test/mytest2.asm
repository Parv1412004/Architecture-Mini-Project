; TITLE: mytest12																										
; Name:	Parv Gadhethariya
; ROLL NO:	2201CS91
; Declaration of Authorship
; This asm file mytest2.asm, is a part of the miniproject of CS210. 
; This program stores 1 if a number is present in the given numbers
; else stores 0 in the main memory.

main:	ldc 2
		adj -1
		stl 0
		ldl 0
		ldc el0
		ldnl 0
		sub
		brz find
		ldl 0
		ldc el1
		ldnl 0
		sub
		brz find
		ldl 0
		ldc el2
		ldnl 0
		sub
		brz find
		ldl 0
		ldc el3
		ldnl 0
		sub
		brz find
		ldc notpresent
		ldnl 0
		stl 0
		br 0
		HALT

find:	ldc present
		ldnl 0
		stl 0
		br 0
		HALT


el0: data 1
el1: data 2
el2: data 3
el3: data 4
notpresent: data 0
present: data 1