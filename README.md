# YM2151 Testbed

This program is designed to act as a kind of test set for probing and verifying the functionality / behavior of the YM2151 in the Commander X16. Its architecture is inspired by the old telco circuit test sets like the venerable TBERD where the unit just performs tests in the background and displays results in real time on the screen.


The display is split up into 3 major sections:

Section|Description
--|----
Counters|Show a running count of various read/write errors occurring during communication with the YM2151. Some tests (e.g. Audible) perform multiple writes per pass, so counters may exceed the number of test passes.
Tests|A menu of test units to run. Different tests may use the counters slightly differently than others. When active, the selected test is performed once per frame, and may return 1 or more failures (currently, only the Audible test performs multiple writes per frame)
Write Method|The tests will use one of three subroutines to read/write the YM chip. Some tests require using their own interface to the YM and will ignore this selection

<br>

### Counter Descriptions:

##### YM Write Counters:
Name|Type|Description
--|-|----
Busy Timeout|F|Write failure due to a busy state that never clears.
Never Busy|F|Failure when write expects to see busy state transition from busy to clear (busy flag read test unit)
Dirty Reads|F|A YM status read returned a value other than `0x00` or `0x80`
OK Busy|I|Write was successful. Busy state was observed and then cleared during the write (YMBUSY mode) or was initially clear but became busy and then cleared again (busy flag read test unit)

Types: &nbsp; **F**: *Failure* &nbsp;&nbsp;  **I**: *Informational*

<br>

##### Test Result Counters:
Name|Description
--|-----
Errors|Each pass may return any number of errors. Most simply return 1 error if the test pass was unsuccessful, or 0 if successful.
Iterations|Counts the number of times the active test has been performed.


<br>

### Test Descriptions:

**No Test**:<br>Does nothing. If test is running, will simply increment the iterations counter.

**Audible Music Test:**<br>Plays a C-major scale folowed by a C-major chord. Repeats indefinitely. This test performs multiple writes per pass, depending on what the sound is doing at the moment. The error counters reflect the total number of errors encountered, but the Iterations is only bumped one per frame.

**Busy Flag Read Test:**<br>
_**This test ignores the selected YM Write method**_.
It first performs a 256-count busy loop to ensure YM is not busy. It then writes zero into YM register 0 (an unused register) and immediately begins polling the busy status flag. The busy flag must be seen to transition from 1 to 0 in order to return success. It bumps `ERRORS` if a timeout occurs before observing this transition.
- An initial 0->1 transition is optional. Bumps `OK BUSY` counter if the test otherwise succeeds.
- If the busy flag is never set, the test bumps `NEVER BUSY`.
- If the busy flag never clears, the test bumps `BUSY TIMEOUT`.

**Clean Status Read Test:**<br>
Simply reads the YM status byte, and returns success if 0, fail if nonzero.
Performs one "safe write" at beginning of test to clear the timer status flags.
This one write is the only part of the test that will bump any of the YM counters.
