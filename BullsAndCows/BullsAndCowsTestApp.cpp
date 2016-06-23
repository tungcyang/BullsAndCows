// BullsAndCowsTestApp.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "BullsAndCows.h"

// This game is based on the description in Wikipedia page "Bulls and Cows" in https://en.wikipedia.org/wiki/Bulls_and_Cows.

int main(int argc, char* argv[])
{
	BullsCowsDigits		bcComputerNextGuess;
	BullsCowsResult		bcRUserGuess;

	initBullsCows();
	bcComputerNextGuess = RandomlyGeneratedBullsCows();

	fprintf(stdout, "Beginning of the Bulls and Cows game :)\n\n");
	fprintf(stdout, "Please think of a secret %d-digit number with all digits different.\n\n", NUM_OF_GUESSING_DIGITS);
	
	do
	{
		fprintf(stdout, "I think your secret number is: ");
		PrintBullsCows(bcComputerNextGuess);
		fprintf(stdout, "How many A's and B's did I get? ");

		// Keeping reading Bulls and Cows results until the user's input is valid.
		while (!ParseBullsCowsResult(&bcRUserGuess))
		{
			fprintf(stdout, "Sorry I don't understand your answer.\nPlease input your Bulls and Cows answer again: ");
		}

		// Eliminating invalid BullsCows combinations, and then generate bcComputerNextGuess.  Check if bcComputerNextGuess is
		// the only valid candidate left.
		eliminateInvalidBullsCowsNumbers(bcComputerNextGuess, bcRUserGuess);
	} while (!generateNextGuess(&bcComputerNextGuess));

	fprintf(stdout, "\nI win!  Your secret number must be ");
	PrintBullsCows(bcComputerNextGuess);

	return 0;
}
