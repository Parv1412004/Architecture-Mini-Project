00000000		 main:
00000000 00001300 main:	ldc el0
00000001 00000004 		ldnl 0
00000002 00000006 		add
00000003 00001400 		ldc el1
00000004 00000004 		ldnl 0
00000005 00000006 		add
00000006 00001500 		ldc el2
00000007 00000004 		ldnl 0
00000008 00000006 		add
00000009 00001600 		ldc el3
0000000a 00000004 		ldnl 0
0000000b 00000006 		add
0000000c 00000200 		ldc 2
0000000d 00000009 		shr
0000000e ffffff0a 		adj -1
0000000f 00000003 		stl 0
00000010 00000002 		ldl 0
00000011 00000011 		br 0
00000012 00000012 		HALT
00000013		 el0:
00000013 00000001 el0: data 1
00000014		 el1:
00000014 00000002 el1: data 2
00000015		 el2:
00000015 00000003 el2: data 3
00000016		 el3:
00000016 00000004 el3: data 4