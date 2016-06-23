// BullsAndCows.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <Windows.h>
#include "BullsAndCows.h"

BullsCowsDigits		bcBuffer0[NUM_OF_GUESSING_TARGETS], bcBuffer1[NUM_OF_GUESSING_TARGETS];
BullsCowsDigits		*pbcPossibleNumbers = bcBuffer0, *pbcPossibleCandidates = bcBuffer1;
unsigned int		uiNumPossibleAnswers = 0, uiNumPossibleCandidates = 0;

// isThisNumberValid() checks whether the given input iNumber corresponds to a valid Bulls and Cows digit pattern.  it returns true if
// the number corresponds to a valid Bulls and Cows digit pattern (while at the same time puts the Bulls and Cows digits in *bcResult,
// and it returns false otherwise.  *bcResult is undefined if isThisNumberValid() returns false.
bool	isThisNumberValid(BullsCowsDigits *pbcResult, unsigned int uiNumber)
{
	unsigned int	i;
	bool	bResult = true;
	unsigned int	uiDigit;
	bool	bDigitUsed[NUM_OF_DIGITS];

	// Initialization.
	*pbcResult = 0;
	for (i = 0; i < NUM_OF_DIGITS; i++)
		bDigitUsed[i] = false;

	for (i = 0; i < NUM_OF_GUESSING_DIGITS; i++)
	{
		// Deriving individual digits.
		uiDigit = uiNumber % 10;
		uiNumber = uiNumber / 10;
		*pbcResult = (*pbcResult << BITS_PER_BAC_DIGIT) | uiDigit;
		if (bDigitUsed[uiDigit])
		{
			// The digit we derived is already in use, and there is no need to continue;
			bResult = false;
			break;
		}
		else
		{
			bDigitUsed[uiDigit] = true;
		}
	}

	return bResult;
}


// initBullsCows() carries out the required initialization of the Bulls and Cows game, including populating
// the array pbcPossibleNumbers[] holding all the possible combinations.
void	initBullsCows()
{
	unsigned int	i, j;
	unsigned int	uiNumCombinations;
	BullsCowsDigits	bcCurrentNumber;

	// Setting random number seed.  This should be really random (unpredictable).
	// srand(0);
	srand(GetTickCount() & 0x0000ffff);

	assert(NUM_OF_GUESSING_DIGITS < NUM_OF_DIGITS);		// We want to avoid the "modulo by 0" case below.
	// It does not seem straightforward to derive NUM_OF_GUESSING_DIGITS non-repeating digits to fill bcPossibleNumbers.
	// Here we simply iterate over all possible 10^NUM_OF_GUESSING_DIGITS combinations, and then skip over the invalid ones
	// (with duplicated digits, for example).
	uiNumPossibleAnswers = 0;
	uiNumCombinations = 1;
	for (j = 0; j < NUM_OF_GUESSING_DIGITS; j++)
		uiNumCombinations *= 10;

	for (i = 0; i < uiNumCombinations; i++)
	{
		if (isThisNumberValid(&bcCurrentNumber, i))
		{
			// i leads to a valid Bulls and Cows combination.  Save it.
			pbcPossibleNumbers[uiNumPossibleAnswers] = bcCurrentNumber;
			uiNumPossibleAnswers++;
		}
	}
	// This should be true.
	assert(uiNumPossibleAnswers == NUM_OF_GUESSING_TARGETS);
}


// RandomlyGeneratedBullsCows() returns a randomly generated Bulls and Cows digit pattern.
BullsCowsDigits	RandomlyGeneratedBullsCows()
{
	// fRandMapping evaluates the float mapping from rand() to a random number between 0 and 9999 (inclusive).
	const float		fRandMapping = (float) pow(10.0, NUM_OF_GUESSING_DIGITS)/(RAND_MAX + 1);
	unsigned int	iRandNum;
	BullsCowsDigits	bc4Digits;

	do
	{
		// Generating a random integer between 0 and 9999 (inclusive on both ends).
		iRandNum = (unsigned int)(rand() * fRandMapping);
	} while (!isThisNumberValid(&bc4Digits, iRandNum));

	return bc4Digits;
}


// PrintBullsCows() prints a Bulls and Cows digit pattern.
void	PrintBullsCows(BullsCowsDigits bcData)
{
	int		i;

	for (i = 0; i < NUM_OF_GUESSING_DIGITS; i++)
		fprintf(stdout, "%1d", (bcData >> (BITS_PER_BAC_DIGIT*(NUM_OF_GUESSING_DIGITS - 1 - i))) & 0x000000ff);
	fprintf(stdout, "\n");
}


// ParseBullsCowsResult() parses (reads from standard input) the user's answer on matching results, for instance,
// "1A2B" or "0A1B".  If it returns false, it means the parsing failed (with invalid inputs).
bool	ParseBullsCowsResult(BullsCowsResult *pbcResult)
{
	bool	bResult = true;
	char	str[5];		// length of "*A*B" + 1

	pbcResult->A = pbcResult->B = 0;

	// We only read the characters filling str[] (extra characters are ignored for security consideration).
	scanf_s("%9s", str, (unsigned) _countof(str));
	// We have two cases: strlen(str) being 4, or strlen(str) being 2 (when the user only responded, say, "2A")
	assert((strlen(str) == 2) || (strlen(str) == 4));

	if (strlen(str) == 4)
	{
		// Verifying str[1] is 'a' or 'A'.
		bResult &= ((str[1] == 'a') || (str[1] == 'A'));
		// Verifying str[3] is 'b' or 'B'.
		bResult &= ((str[3] == 'b') || (str[3] == 'B'));
		// We are processing a fixed-sized string.  Using atoi() seems awkward.
		pbcResult->A = (unsigned int)(str[0] - '0');
		pbcResult->B = (unsigned int)(str[2] - '0');
	}
	else if (strlen(str) == 2)
	{
		// The user is lazy to reply only "2A" or "3B".
		if ((str[1] == 'a') || (str[1] == 'A'))
			pbcResult->A = (unsigned int)(str[0] - '0');
		else if ((str[1] == 'b') || (str[1] == 'B'))
			pbcResult->B = (unsigned int)(str[0] - '0');
		else
			// str[1] is not one of 'a', 'A', 'b' or 'B'.
			bResult = false;
	}
	else
	{
		// This should not happen.
		bResult = false;
	}

	// Verifying the A and B answers are valid.
	bResult &= (pbcResult->A <= NUM_OF_GUESSING_DIGITS);
	bResult &= (pbcResult->B <= NUM_OF_GUESSING_DIGITS);
	bResult &= ((pbcResult->A + pbcResult->B) <= NUM_OF_GUESSING_DIGITS);

	return bResult;
}


// guessingResponse() returns the Bulls and Cows response on the inputs bcFirst and bcSecond.  For instance, when bcFirst corresponds to
// "0123" and bcSecond corresponds to "0326", guessingResponse() returns 2A1B.
BullsCowsResult		guessingResponse(BullsCowsDigits bcFirst, BullsCowsDigits bcSecond)
{
	unsigned int	i;
	unsigned int	uiNumAs, uiNumBs;
	unsigned int	uiTemp;
	BullsCowsResult bcResult;
	unsigned int	bDigitsUsedFirst[NUM_OF_DIGITS], bDigitsUsedSecond[NUM_OF_DIGITS];

	// Checking A's.  We take an EXOR of bcFirst and bcSecond, then count the number of 0x0000's.
	uiNumAs = 0;
	uiTemp = bcFirst ^ bcSecond;
	for (i = 0; i < NUM_OF_GUESSING_DIGITS; i++)
	{
		if (!(uiTemp & 0x000000ff))
			uiNumAs++;
		uiTemp >>= BITS_PER_BAC_DIGIT;
	}
	bcResult.A = uiNumAs;

	// Checking B's.  Note that bcFirst and bcSecond will be altered here.
	for (i = 0; i < NUM_OF_DIGITS; i++)
		bDigitsUsedFirst[i] = bDigitsUsedSecond[i] = 0;
	for (i = 0; i < NUM_OF_GUESSING_DIGITS; i++)
	{
		bDigitsUsedFirst[bcFirst & 0x000000ff]++;
		bDigitsUsedSecond[bcSecond & 0x000000ff]++;
		bcFirst >>= BITS_PER_BAC_DIGIT;
		bcSecond >>= BITS_PER_BAC_DIGIT;
	}
	// We accumulate the smaller of bDigitsUsedFirst[] and bDigitsUsedSecond[] -- it should be the sume of number of A's
	// and the number of B's, no matter if we allow duplicated digits or not.
	uiNumBs = 0;
	for (i = 0; i < NUM_OF_DIGITS; i++)
		if (bDigitsUsedFirst[i] < bDigitsUsedSecond[i])
			uiNumBs += bDigitsUsedFirst[i];
		else
			uiNumBs += bDigitsUsedSecond[i];
	uiNumBs -= uiNumAs;
	bcResult.B = uiNumBs;

	return bcResult;
}


// eliminateInvalidBullsCowsNumbers() goes through the array bcPossibleNumbers[] of size uiNumPossibleAnswers
// and remove the entries inconsistent of bcComputerNextGuess and bcRUserGuess.  bcPossibleNumbers[] and
// uiNumPossibleAnswers will be updated upon return.
void	eliminateInvalidBullsCowsNumbers(BullsCowsDigits bcComputerNextGuess, BullsCowsResult bcRUserGuess)
{
	unsigned int	i;
	BullsCowsDigits		*pbcTemp = NULL;
	BullsCowsResult		bcResult;

	// We use pbcPossibleCandidates[] and uiNumPossibleCandidates as scratch variables.
	uiNumPossibleCandidates = 0;
	for (i = 0; i < uiNumPossibleAnswers; i++)
	{
		bcResult = guessingResponse(bcComputerNextGuess, pbcPossibleNumbers[i]);
		if ((bcRUserGuess.A == bcResult.A) && (bcRUserGuess.B == bcResult.B))
		{
			pbcPossibleCandidates[uiNumPossibleCandidates] = pbcPossibleNumbers[i];
			uiNumPossibleCandidates++;
		}
	}

	// Swapping the pointers to the BullsCowsDigits arrays.
	pbcTemp = pbcPossibleNumbers;
	pbcPossibleNumbers = pbcPossibleCandidates;
	pbcPossibleCandidates = pbcTemp;
	uiNumPossibleAnswers = uiNumPossibleCandidates;

	// uiNumPossibleAnswers must be positive.  If it is 0, there is something wrong.
	assert(uiNumPossibleAnswers);
}


// generateNextGuess() scans over the remaining valid bcPossibleNumbers[] and returns a guess for the computer in *pbcNextGuess.
// It returns true if there is only one candidate (the generated guess must be the answer).
bool	generateNextGuess(BullsCowsDigits *pbcNextGuess)
{
	unsigned int		i, j;
	unsigned int		uiDigitsStatistics[NUM_OF_DIGITS];
	unsigned int		uiMostLikelyDigit, uiMostLikelyDigitFreq;
	BullsCowsDigits		bcCandidates[NUM_OF_GUESSING_TARGETS];
	BullsCowsDigits		*pbcCandidates;
	unsigned int		uiNumCandidates;

	// Initialization.
	uiNumCandidates = uiNumPossibleAnswers;
	pbcCandidates = pbcPossibleNumbers;

	// Iterating to find digits for bcResult.
	for (i = 0; i < NUM_OF_GUESSING_DIGITS; i++)
	{
		// Find out the most frequently occurring digit.
		for (j = 0; j < NUM_OF_DIGITS; j++)
			uiDigitsStatistics[j] = 0;
		for (j = 0; j < uiNumCandidates; j++)
		{
			uiDigitsStatistics[(pbcCandidates[j] >> (BITS_PER_BAC_DIGIT*(NUM_OF_GUESSING_DIGITS - 1 - i))) & 0x000000ff] ++;
		}
		uiMostLikelyDigit = INVALID_DIGIT;
		uiMostLikelyDigitFreq = 0;
		for (j = 0; j < NUM_OF_DIGITS; j++)
			if (uiDigitsStatistics[j] > uiMostLikelyDigitFreq)
			{
				uiMostLikelyDigit = j;
				uiMostLikelyDigitFreq = uiDigitsStatistics[j];
			}
		// Now uiMostLikelyDigit points to the most likely digit for bcResult.
		*pbcNextGuess = (*pbcNextGuess << BITS_PER_BAC_DIGIT) | uiMostLikelyDigit;

		// Having determined uiMostLikelyDigit and combining it into *pbcNextGuess, we want to copy those pbcCandidates[]
		// with chosen digit (uiMostLikelyDigit) into the scratch buffer and continuing finding the next digit.
		uiNumPossibleCandidates = 0;
		for (j = 0; j < uiNumCandidates; j++)
		{
			if (((pbcCandidates[j] >> (BITS_PER_BAC_DIGIT*(NUM_OF_GUESSING_DIGITS - 1 - i))) & 0x000000ff) == uiMostLikelyDigit)
			{
				pbcPossibleCandidates[uiNumPossibleCandidates] = pbcCandidates[j];
				uiNumPossibleCandidates++;
			}
		}
		// Pointing *pbcCandidates to local buffer and then copy the data back to *pbcCandidates to get ready for the
		// next iteration.
		pbcCandidates = bcCandidates;
		for (j = 0; j < uiNumPossibleCandidates; j++)
			pbcCandidates[j] = pbcPossibleCandidates[j];
		uiNumCandidates = uiNumPossibleCandidates;
	}

	return (uiNumPossibleAnswers == 1);
}