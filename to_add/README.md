== Introduction ==

This directory contains test cases that haven't yet been added to the
ZZT Crashopedia.

== Contents ==

- CHURN.ZZT

	Allocates and deallocates lots of memory (2 MB) from repeatedly
	entering the board it exited. Crashes DOS ZZT with a heap overflow.

- CR2.ZZT, CR3.ZZT, CR4.ZZT

	Garbled metadata or too small a board size - find out which later.

- CR8.ZZT

	Too large BoardLen (exceeding the size of the game file).

- CR9.ZZT

	Use of a board tile with RLE count 0 causes desynchronization and
	subsequent crash on cleanup.

- CR11.ZZT

	The player stat has a data reference (DataLen < 0) to a later object
	with data. This causes a double free and a crash.

- CR14\_DUP.ZZT

	N/A

- CR19.ZZT

	N/A

- DISCREP.ZZT

	A discrepancy in board loading can in some cases load a board without
	placing a player or monitor on it, thus violating the player invariant.
	This test case exercises that bug. (Also crashes plain ZZT, probably
	for another reason; need to determine which.)

- OBJ\_OOB.ZZT

	Out-of-bounds object commands (FLOW and stepx, stepy out of bounds)
	cause a range error reading from outside of the board. May also
	clobber stuff outside board memory.

- STRANGE.ZZT

	Exercises a lot of centipede out-of-range bugs related to stepX and
	stepY.
