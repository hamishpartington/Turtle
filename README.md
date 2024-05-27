# Turtle

This was my final piece of coursework for the C module in my Computer Science MSc. The task was to implement a parser and interpreter for the Turtle language (A language designed to teach people programming) in C. The interpreter can output the result of the input script as a a .txt, .pdf, .ps or in the console using ASCII art depending on the arguments input by the user.

## Extension

I chose to use SDL for my extension. My program allows the user to move the the turtle arround in an SDL window using keyboard inputs:

- The up arrow is the equivalent of `FORWARD 1`
- The down arrow is the equivalent of `FORWARD -1`
- The right arrow is the equivalent of `RIGHT 1`
- the left arrow is the equivalent of `RIGHT -1`
- W is equivalent of `COLOUR "WHITE"`
- B is equivalent of `COLOUR "BLUE"`
- K is equivalent of `COLOUR "BLACK"`
- C is equivalent of `COLOUR "CYAN"`
- R is equivalent of `COLOUR "RED"`
- M is equivalent of `COLOUR "MAGENTA"`
- Y is equivalent of `COLOUR "YELLOW"`
- G is equivalent of `COLOUR "GREEN"`

After the user presses the x on the SDL window a .ttl script will be output which will allow the user to run it through the interpreter to genrate a pdf of what they have drawn. The output script goes through an optimisation process in that it will combine similar commands in sequence e.g.:

`FORWARD 1 FORWARD 1 FORWARD 1`
becomes
`FORWARD 3`

If the user wants to output the ps/txt they can enter the output file name as `argv[1]`. The program will then run the interpreter with a system command and save the file as a .txt or .pdf and .ps depending on what the user inputs; saving the file in the Results folder.

The dimensions of the SDL window and the distance moved by each forward instruction were chosen to best match the txt grid specified for the interpreter

## Mark

I revieved a distinction for this coursework, with a mark of 76.
