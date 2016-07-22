// BullsAndCowsTestApp.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "BullsAndCows.h"

// This game is based on the description in Wikipedia page "Bulls and Cows" in https://en.wikipedia.org/wiki/Bulls_and_Cows.

void	GamePreamble()
{
	fprintf(stdout, "Beginning of the Bulls and Cows game :)\n\n");
	fprintf(stdout, "Please think of a secret %d-digit number with different digits.\n\n", NUM_OF_GUESSING_DIGITS);
	fprintf(stdout, "You can start guessing my secret %d-digit number.\n", NUM_OF_GUESSING_DIGITS);
	fprintf(stdout, "Anything else means you like me to guess first.\n\n");
}


int main(int argc, char* argv[])
{
	BullsCowsDigits		bcComputerNextGuess, bcUserNextGuess, bcComputerSecretNum;
	BullsCowsResult		bcUserSecretNumResult, bcCompSecretNumResult;

	// Engaging the self-test when needed.
	// BullsAndCowsTest();

	initBullsCows(&bcUserSecretNumResult, &bcCompSecretNumResult);
	bcComputerSecretNum = RandomlyGeneratedBullsCows();		// 6481 under testing condition with srand(0).
	bcComputerNextGuess = RandomlyGeneratedBullsCows();		// 0743 under testing condition with srand(0).

	GamePreamble();

	// Allowing the user to take a guess first.  It is separated from the main loop because it is okay for
	// the user to enter some bad data (to indicate she likes the computer to guess first).
	if (ParseBullsCowsGuess(&bcUserNextGuess))
	{
		// The user wants to guess first.
		// Print user's A/B result and terminate the game if the user is really lucky.
		bcCompSecretNumResult = guessingResponse(bcComputerSecretNum, bcUserNextGuess);
		if (bcCompSecretNumResult.A < GAMEOVER_NUM_A)
		{
			fprintf(stdout, "Okay, your guess leads to ");
			PrintBullsCowsResult(bcCompSecretNumResult);
		}
	}

	while ((bcCompSecretNumResult.A < GAMEOVER_NUM_A) && (bcUserSecretNumResult.A < GAMEOVER_NUM_A))
	{
		fprintf(stdout, "\nI think your secret number is: ");
		PrintBullsCows(bcComputerNextGuess);
		fprintf(stdout, "How many A's and B's did I get? ");

		// Keeping reading Bulls and Cows results until the user's input is valid.
		while (!ParseBullsCowsResult(&bcUserSecretNumResult))
		{
			fprintf(stdout, "Sorry I don't understand your answer.\nPlease input your Bulls and Cows answer again: ");
		}

		// Check if the computer wins already.
		if (bcUserSecretNumResult.A == GAMEOVER_NUM_A)
			continue;

		// Eliminating invalid BullsCows combinations, and then generate bcComputerNextGuess.  Check if bcComputerNextGuess is
		// the only valid candidate left.
		eliminateInvalidBullsCowsNumbers(bcComputerNextGuess, bcUserSecretNumResult);

		fprintf(stdout, "\nWhat is your guess of my secret number? ");
		// Keeping reading Bulls and Cows guesses until the user's input is valid.
		while (!ParseBullsCowsGuess(&bcUserNextGuess))
		{
			fprintf(stdout, "Sorry I don't understand your answer.\nPlease input your Bulls and Cows guess again: ");
		}

		// Print user's A/B result and terminate the game if the user is really lucky.
		bcCompSecretNumResult = guessingResponse(bcComputerSecretNum, bcUserNextGuess);

		// Check if the user wins already.
		if (bcCompSecretNumResult.A == GAMEOVER_NUM_A)
			continue;

		fprintf(stdout, "Okay, your guess leads to ");
		PrintBullsCowsResult(bcCompSecretNumResult);

		generateNextGuess(&bcComputerNextGuess);
	}

	if (bcUserSecretNumResult.A == GAMEOVER_NUM_A)
	{
		fprintf(stdout, "\nI win and I successfully guessed your secret number!\n");
	}
	else if (bcCompSecretNumResult.A == GAMEOVER_NUM_A)
	{
		fprintf(stdout, "\nYou win and you successfully guessed my secret number!\n");
	}
	else
	{
		// This should not happen.
		fprintf(stderr, "Something is wrong!\n");
	}

	return 0;
}
