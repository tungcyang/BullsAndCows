// BullsAndCows.h : Types and constants defined here.
//
#pragma once

#define	NUM_OF_DIGITS						10
#define NUM_OF_GUESSING_DIGITS				4
#define NUM_OF_GUESSING_TARGETS				(10*9*8*7)				// 10!/(10 - NUM_OF_GUESSING_DIGETS)! = 5040; if we allow the digits to
																	// duplicate, then this becomes 10^NUM_OF_GUESSING_DIGITS.
#define INVALID_DIGIT						10						// Valid digits range from 0 to 9.
#define BITS_PER_BAC_DIGIT					8						// Number of bits each Bulls and Cows digit takes.

// We will use the lower bytes of BullsCowsDigits (unsigned int) to keep the (4) digits.
typedef	unsigned int				BullsCowsDigits;

typedef struct
{
	// These are the responses for a guessed 4-digit, like "1A2B" or "2B".
	unsigned int	A;
	unsigned int	B;
} BullsCowsResult;

// Function headers.
void	initBullsCows();
BullsCowsDigits	RandomlyGeneratedBullsCows();
void	PrintBullsCows(BullsCowsDigits bcData);
bool	ParseBullsCowsResult(BullsCowsResult *pbcResult);
void	eliminateInvalidBullsCowsNumbers(BullsCowsDigits bcComputerNextGuess, BullsCowsResult bcRUserGuess);
bool	generateNextGuess(BullsCowsDigits *pbcNextGuess);
