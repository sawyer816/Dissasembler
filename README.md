Alexander Ray
RedID: 823978916
Sawyer Thompson
RedID: 823687079

Our process for programming the Disassembler went as followed.
First & Second Day, Brainstorm/Pseudo Code
Third-Finish Begin/Finish Coding

The initial code that we did even prior to brainstorming was setting up the main program to accept the input and output files to work consistently in our C++ program.

Through our brainstorming process we decided that we would have 3 different functions that would break apart the object code into header, text and end lines. These would easily allow us to break up our code. This is our runHeader, runText, and runEnd functions.

We also decided to follow a program that was based around formatting. We decided that we would have a center function that would do specific instructions if it was format 2, format 3, or format 4. When we began checking literals, we made an honorary "format 1". This is our putInstructions function.

When we first began psuedocoding, we brainstormed how to best store the given arrays ops and mnemonics and settled on maps as we could easily pull out a value given the key of "ops". We also realized that maps would be the best way to store everything as it is both convient and quick since we weren't changing things inside the map. This is how we stored the format2 registers as well as the symbols and literals from the input symbol table. 

We also knew that in order to get the ops, you would take the first two bits and shift them twice and shift them back twice to get rid of the NI bits which we would then be able to compare in the opsMnemonics map. 

We also made sure to have a function that checked the NI code to be able to return an @ for indirect and # for immediate.

Along those same lines we created a function that checked the X bit by taking the third byte, shifting and then using a % to test if it is present or not

We knew we would have to break apart the object code for the text, and we used bit moving to test if it was format 2, 3, or 4. Something is format 2 if its NI bits are 00, so we first tested for that, and then we checked if the e bit was 0 or 1 to either have it be format 3 or 4. This was done in a helper method checkFormat.

In our putInstructions function we loop through the text line and based on the format, add the specified number or bytes to our itorator i (format 2+=4 format 3+= 6 etc.). We also update location +2 for f2, +3 for f3 and +4 for f4 this way we can easily track it to ouput the addresses in the output file. 

We had a few hard coded cases for LDB and LD, we updated the base and LDX to be used throughout the program.

Most of our functions were implemented in putInstruction, which output most of the code to the output file. 


One of the most difficult parts was finding the RESB directives which could be in between Text Records.  We found that there was a RESB every time the location after completing a text record was different than the starting address of the next Record. To implement this, we called our putRESB function before parsing the instructions of a text record and before finishing the End Record. To calculate the size of the RESB, we had to subtract the next address from the current address. This could either be the end of the program or another RESB directive. Then we output that to the file.

One problem we did run into as we were coding was the literal and symbols. Adding format 1, we were able to extract the literals and ensure they were placed in the correct spot, and then we had to go through the both the symbol and literal map again when testing our Displacement function which adds the symbols to the second to last column. 

There were a few edge cases we had to test such as changing a label to a * or BYTE, but overall this was our process. 

