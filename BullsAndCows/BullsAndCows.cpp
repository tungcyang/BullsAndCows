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

// isThisNumberValid() checks whether the given input uiNumber corresponds to a valid Bulls and Cows digit pattern.  it returns true if
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
void	initBullsCows(BullsCowsResult *pbcUserSecretNumResult, BullsCowsResult *pbcCompSecretNumResult)
{
	unsigned int	i, j;
	unsigned int	uiNumCombinations;
	BullsCowsDigits	bcCurrentNumber;

	// Setting random number seed.  This should be really random (unpredictable).
	// Using 0 as the random seed during tests.
	// srand(0);
	srand(GetTickCount() & 0x0000ffff);

	// Initializing bcUserSecretNumResult and bcCompSecretNumResult.  Both the computer and the user start with 0A0B.
	pbcUserSecretNumResult->A = 0;
	pbcUserSecretNumResult->B = 0;
	pbcCompSecretNumResult->A = 0;
	pbcCompSecretNumResult->B = 0;

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


// PrintBullsCowsResult() prints a Bulls and Cows result.
void	PrintBullsCowsResult(BullsCowsResult bcResult)
{
	fprintf(stdout, "%dA%dB\n", bcResult.A, bcResult.B);
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


// ParseBullsCowsGuess() parses (reads from standard input) the user's guess on the computer's secret number, for instance,
// "1234" or "6537".  If it returns false, it means the parsing failed (with invalid inputs, like containing non-digit
// characters, or duplicated digits).
bool	ParseBullsCowsGuess(BullsCowsDigits *pbcResult)
{
	bool	bResult = true;
	unsigned int	i;
	int		iDigit;
	char	str[5];		// length of "****" + 1
	unsigned int	uiGuess;

	uiGuess = 0;

	// We only read the characters filling str[] (extra characters are ignored for security consideration).
	scanf_s("%9s", str, (unsigned)_countof(str));
	// We have only one valid case: strlen(str) being 4, NUM_OF_GUESSING_DIGITS)
	assert(strlen(str) == NUM_OF_GUESSING_DIGITS);

	if (strlen(str) == NUM_OF_GUESSING_DIGITS)
	{
		for (i = 0; i < NUM_OF_GUESSING_DIGITS; i++)
		{
			iDigit = (int) str[i] - '0';
			if ((iDigit < 0) || (iDigit > 9))
			{
				// The input data leads to a digit outside the valid range.
				return false;
			}
			else
				uiGuess = uiGuess*10 + iDigit;
		}
	}
	else
	{
		// This should not happen.
		bResult = false;
	}

	// Verifying that the digits do not repeat.
	bResult &= isThisNumberValid(pbcResult, uiGuess);

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

	// uiNumPossibleAnswers must be positive.  If it is 0, there is something wrong -- there are no valid candidates left.
	// Either there is a bug, or the user provided wrong clues.
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


// BullsAndCowsTest() essentially tests the guessing algorithm adopted in this game to make sure it does not lead to an
// infinite loop, and likely collect some statistics for the algorithm.  It essentially follows the main program except:
// (1) there are no user inputs/outputs involved;
// (2) only the computer attempts to guess the user's secret number with the implemented algorithm.
void	BullsAndCowsTest()
{
	unsigned int	uiUserSecretNum, uiCompGuessNum;
	BullsCowsDigits	bcUserSecretNum, bcCompGuessNum;
	BullsCowsResult		bcUserSecretNumResult, bcCompSecretNumResult;
	unsigned int	uiNumValidTests, uiCurrNumGuesses, uiMaxNumGuesses, uiTotalNumGuesses;

	// Initialization
	uiNumValidTests = 0;
	uiMaxNumGuesses = 0;
	uiTotalNumGuesses = 0;

	for (uiUserSecretNum = 0; uiUserSecretNum < 10000; uiUserSecretNum++)
	{
		// Moving to the next number if this one is not valid.
		if (!isThisNumberValid(&bcUserSecretNum, uiUserSecretNum))
			continue;

		// Printing current status (uiUserSecretNum) while trying all combinations of uiCompGuessNum so the tester knows
		// where we are during the test.
		fprintf(stderr, "Currently testing uiUserSecretNum %d and all possible uiCompGuessNum\n", uiUserSecretNum);

		for (uiCompGuessNum = 0; uiCompGuessNum < 10000; uiCompGuessNum++)
		{
			// Moving to the next number if this one is not valid.
			if (!isThisNumberValid(&bcCompGuessNum, uiCompGuessNum))
				continue;

			// Updating status
			uiNumValidTests++;
			uiCurrNumGuesses = 1;

			// Now we have both bcUserSecretNum and bcCompGuessNum being valid and exhaustive.  Continue the guessing.
			initBullsCows(&bcUserSecretNumResult, &bcCompSecretNumResult);

			while ((bcUserSecretNumResult.A < GAMEOVER_NUM_A) && (uiCurrNumGuesses < MAX_GUESSING_ALLOWED))
			{
				// Use bcUserSecretNum and bcCompGuessNum to derive bcUserSecretNumResult
				bcUserSecretNumResult = guessingResponse(bcUserSecretNum, bcCompGuessNum);

				// Check if the computer wins already.
				if (bcUserSecretNumResult.A == GAMEOVER_NUM_A)
					continue;

				eliminateInvalidBullsCowsNumbers(bcCompGuessNum, bcUserSecretNumResult);
				generateNextGuess(&bcCompGuessNum);
				uiCurrNumGuesses++;
			}

			if (uiCurrNumGuesses == MAX_GUESSING_ALLOWED)
			{
				// Runaway guess.  Report it and exit.
				fprintf(stdout, "We have a runaway guess under current implementation!\n\n");
				return;
			}

			// We are done and reach a success.  Record the statistics
			if (uiMaxNumGuesses < uiCurrNumGuesses)
				uiMaxNumGuesses = uiCurrNumGuesses;
			uiTotalNumGuesses += uiCurrNumGuesses;
		}
	}

	// We have iterated over all possible combinations.  Reporting the statistics we have collected.

	fprintf(stdout, "We have attempted %ld different valid combinations.\n", uiNumValidTests);
	fprintf(stdout, "The longest one takes %ld guesses.\n", uiMaxNumGuesses);
	fprintf(stdout, "On average each attempt takes %f guesses.\n", (float)uiTotalNumGuesses / uiNumValidTests);

	// We have attempted 25401600 different valid combinations (25401600 = (10*9*8*7)^2), 5040 combinations for
	// uiUserSecretNum and 5040 for uiCompGuessNum.
	// The longest game takes 9 guesses.
	// On average each attempt takes 5.455535 guesses for the computer to successfully guess the user's secret number.
	//
	// The longest session will look like the followings.  Note that the digits in uiUserSecretNum and those
	// in bcUserSecretNum are in reversed order as implemented.
	//
	// We hit 9 guesses when uiUserSecretNum is 0138 (bcUserSecretNum: 0x08030100) and uiCompGuessNum starts with 3209:
	// uiCompGuessNum --> 3209 : 0A2B
	//                    4120 : 1A1B
	//                    5310 : 0A3B
	//                    4053 : 0A2B
	//                    2135 : 2A0B
	//                    0195 : 2A0B
	//                    0136 : 3A0B
	//                    0137 : 3A0B
	//                    0138 : 4A0B
}
