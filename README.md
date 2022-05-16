# WORDLE

Wordle is a word guessing game. 
Players have six attempts to guess a five-letter word, with feedback given for each guess in the form of colored tiles indicating when letters match or occupy the correct position. 

Colors:

- Green: Correct letter in the correct position.
- Yellow: The word contains this letter at a different position.
- Gray: The word does not contain this letter.

This game is made using [FTXUI](https://github.com/ArthurSonzogni/ftxui).

Currently works on Linux/MacOS only.


## Build instructions:
```
git clone https://github.com/ayush933/wordle.git
cd wordle
cmake -S . -B build
cmake --build build
./build/wordle
```