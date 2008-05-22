#include <float.h>
#include "Biostrings.h"

#define MAX(x, y) (x > y ? x : y)

#define NEGATIVE_INFINITY (- FLT_MAX)

#define  GLOBAL_ALIGNMENT 1
#define   LOCAL_ALIGNMENT 2
#define OVERLAP_ALIGNMENT 3

#define SUBSTITUTION 'S'
#define DELETION     'D'
#define INSERTION    'I'
#define TERMINATION  'T'

#define F_MATRIX(i, j) (fMatrix[(nCharString2 + 1) * i + j])
#define H_MATRIX(i, j) (hMatrix[(nCharString2 + 1) * i + j])
#define V_MATRIX(i, j) (vMatrix[(nCharString2 + 1) * i + j])
#define S_TRACE_MATRIX(i, j) (sTraceMatrix[nCharString2 * i + j])
#define D_TRACE_MATRIX(i, j) (dTraceMatrix[nCharString2 * i + j])
#define I_TRACE_MATRIX(i, j) (iTraceMatrix[nCharString2 * i + j])

#define SAFE_SUM(x, y) (x == NEGATIVE_INFINITY ? x : (x + y))

#define SET_LOOKUP_VALUE(lookupTable, length, key) \
{ \
	unsigned char lookupKey = (unsigned char) (key); \
	if (lookupKey >= (length) || (lookupValue = (lookupTable)[lookupKey]) == NA_INTEGER) { \
		error("key %d not in lookup table", (int) lookupKey); \
	} \
}

/* Global Variables */
static int nCharAligned = 0;
static char *align1, *align2;


/* Returns the score of the optimal pairwise alignment */
static float pairwiseAlignment(
		RoSeq stringElements1,
		RoSeq stringElements2,
		RoSeq qualityElements1,
		RoSeq qualityElements2,
		char gapCode,
		int typeCode,
		int scoreOnly,
		float gapOpening,
		float gapExtension,
		const int *qualityLookupTable,
		int qualityLookupTableLength,
		const double *qualityMatchMatrix,
		const double *qualityMismatchMatrix,
		const int *qualityMatrixDim,
		const int *constantLookupTable,
		int constantLookupTableLength,
		const double *constantMatrix,
		const int *constantMatrixDim)
{
	int i, j, iMinus1, jMinus1;

	/* Step 1:  Get information on input XString objects */
	int nCharString1 = stringElements1.nelt;
	int nCharString2 = stringElements2.nelt;
	int nQuality1 = qualityElements1.nelt;
	int nQuality2 = qualityElements2.nelt;

	/* Step 2:  Create objects for scores and traceback values */
	float *fMatrix = (float *) R_alloc((long) (nCharString1 + 1) * (nCharString2 + 1), sizeof(float));
	float *hMatrix = (float *) R_alloc((long) (nCharString1 + 1) * (nCharString2 + 1), sizeof(float));
	float *vMatrix = (float *) R_alloc((long) (nCharString1 + 1) * (nCharString2 + 1), sizeof(float));		
	char *sTraceMatrix = (char *) R_alloc((long) nCharString1 * nCharString2, sizeof(char));
	char *iTraceMatrix = (char *) R_alloc((long) nCharString1 * nCharString2, sizeof(char));
	char *dTraceMatrix = (char *) R_alloc((long) nCharString1 * nCharString2, sizeof(char));
	if (typeCode == GLOBAL_ALIGNMENT) {
		for (i = 0; i <= nCharString1; i++)
			H_MATRIX(i, 0) = gapOpening + i * gapExtension;
		for (j = 0; j <= nCharString2; j++)
			V_MATRIX(0, j) = gapOpening + j * gapExtension;
	} else {
		for (i = 0; i <= nCharString1; i++)
			H_MATRIX(i, 0) = 0.0;
		for (j = 0; j <= nCharString2; j++)
			V_MATRIX(0, j) = 0.0;
	}
	F_MATRIX(0, 0) = 0;
	for (i = 1; i <= nCharString1; i++) {
		F_MATRIX(i, 0) = NEGATIVE_INFINITY;
		V_MATRIX(i, 0) = NEGATIVE_INFINITY;
	}
	for (j = 1; j <= nCharString2; j++) {
		F_MATRIX(0, j) = NEGATIVE_INFINITY;
		H_MATRIX(0, j) = NEGATIVE_INFINITY;
	}

	/* Step 3:  Generate scores and traceback values */
	RoSeq sequence1, sequence2;
	int scalar1, scalar2;
	const int *lookupTable;
	int lookupTableLength;
	const double *matchMatrix, *mismatchMatrix;
	const int *matrixDim;
	if (nQuality1 == 0) {
		sequence1 = stringElements1;
		sequence2 = stringElements2;
		scalar1 = (nCharString1 == 1);
		scalar2 = (nCharString2 == 1);
		lookupTable = constantLookupTable;
		lookupTableLength = constantLookupTableLength;
		matchMatrix = constantMatrix;
		mismatchMatrix = constantMatrix;
		matrixDim = constantMatrixDim;
	} else {
		sequence1 = qualityElements1;
		sequence2 = qualityElements2;
		scalar1 = (nQuality1 == 1);
		scalar2 = (nQuality2 == 1);
		lookupTable = qualityLookupTable;
		lookupTableLength = qualityLookupTableLength;
		matchMatrix = qualityMatchMatrix;
		mismatchMatrix = qualityMismatchMatrix;
		matrixDim = qualityMatrixDim;
	}
	int startRow = -1, startCol = -1;
	int lookupValue, element1, element2;
	float substitutionValue, score, startScore = NEGATIVE_INFINITY;
	float gapOpeningPlusExtension = gapOpening + gapExtension;
	for (i = 1, iMinus1 = 0; i <= nCharString1; i++, iMinus1++) {
		SET_LOOKUP_VALUE(lookupTable, lookupTableLength, sequence1.elts[scalar1 ? 0 : iMinus1]);
		element1 = lookupValue;
		for (j = 1, jMinus1 = 0; j <= nCharString2; j++, jMinus1++) {
			SET_LOOKUP_VALUE(lookupTable, lookupTableLength, sequence2.elts[scalar2 ? 0 : jMinus1]);
			element2 = lookupValue;
			if (stringElements1.elts[iMinus1] == stringElements2.elts[jMinus1])
				substitutionValue = (float) matchMatrix[matrixDim[0] * element1 + element2];
			else
				substitutionValue = (float) mismatchMatrix[matrixDim[0] * element1 + element2];

			if (F_MATRIX(iMinus1, jMinus1) >= MAX(H_MATRIX(iMinus1, jMinus1), V_MATRIX(iMinus1, jMinus1))) {
				S_TRACE_MATRIX(iMinus1, jMinus1) = SUBSTITUTION;
				F_MATRIX(i, j) = SAFE_SUM(F_MATRIX(iMinus1, jMinus1), substitutionValue);
			} else if (V_MATRIX(iMinus1, jMinus1) >= H_MATRIX(iMinus1, jMinus1)) {
				S_TRACE_MATRIX(iMinus1, jMinus1) = INSERTION;
				F_MATRIX(i, j) = SAFE_SUM(V_MATRIX(iMinus1, jMinus1), substitutionValue);
			} else {
				S_TRACE_MATRIX(iMinus1, jMinus1) = DELETION;
				F_MATRIX(i, j) = SAFE_SUM(H_MATRIX(iMinus1, jMinus1), substitutionValue);
			}
			if (SAFE_SUM(F_MATRIX(iMinus1, j), gapOpening) >= H_MATRIX(iMinus1, j)) {
				D_TRACE_MATRIX(iMinus1, jMinus1) = SUBSTITUTION;
				H_MATRIX(i, j) = SAFE_SUM(F_MATRIX(iMinus1, j), gapOpeningPlusExtension);
			} else {
				D_TRACE_MATRIX(iMinus1, jMinus1) = DELETION;
				H_MATRIX(i, j) = SAFE_SUM(H_MATRIX(iMinus1, j), gapExtension);
			}
			if (SAFE_SUM(F_MATRIX(i, jMinus1), gapOpening) >= V_MATRIX(i, jMinus1)) {
				I_TRACE_MATRIX(iMinus1, jMinus1) = SUBSTITUTION;
				V_MATRIX(i, j) = SAFE_SUM(F_MATRIX(i, jMinus1), gapOpeningPlusExtension);
			} else {
				I_TRACE_MATRIX(iMinus1, jMinus1) = INSERTION;
				V_MATRIX(i, j) = SAFE_SUM(V_MATRIX(i, jMinus1), gapExtension);
			}

			if (typeCode == LOCAL_ALIGNMENT) {
				F_MATRIX(i, j) = MAX(0.0, F_MATRIX(i, j));
				if (F_MATRIX(i, j) == 0.0) {
					S_TRACE_MATRIX(iMinus1, jMinus1) = TERMINATION;
					D_TRACE_MATRIX(iMinus1, jMinus1) = TERMINATION;
					I_TRACE_MATRIX(iMinus1, jMinus1) = TERMINATION;
				}
				if (F_MATRIX(i, j) > startScore) {
					startRow = i;
					startCol = j;
					startScore = F_MATRIX(i, j);
				}
			}
		}
	}
	if (typeCode == GLOBAL_ALIGNMENT) {
		startRow = nCharString1;
		startCol = nCharString2;
		startScore =
			MAX(F_MATRIX(nCharString1, nCharString2),
			MAX(H_MATRIX(nCharString1, nCharString2),
			    V_MATRIX(nCharString1, nCharString2)));
	} else if (typeCode == OVERLAP_ALIGNMENT) {
		for (i = 1; i <= nCharString1; i++) {
			score =
				MAX(F_MATRIX(i, nCharString2),
				MAX(H_MATRIX(i, nCharString2),
				    V_MATRIX(i, nCharString2)));
			if (score > startScore) {
				startRow = i;
				startCol = nCharString2;
				startScore = score;
			}
	    }
		for (j = 1; j <= nCharString2; j++) {
			score =
				MAX(F_MATRIX(nCharString1, j),
				MAX(H_MATRIX(nCharString1, j),
				    V_MATRIX(nCharString1, j)));
			if (score > startScore) {
				startRow = nCharString1;
				startCol = j;
				startScore = score;
			}
	    }
	}

	nCharAligned = 0;
	if (scoreOnly == 0) {
		/* Step 4:  Get a starting location for the traceback */
		int alignmentBufferSize = nCharString1 + nCharString2;
		char *align1Buffer = (char *) R_alloc((long) alignmentBufferSize, sizeof(char));
		char *align2Buffer = (char *) R_alloc((long) alignmentBufferSize, sizeof(char));
		align1 = align1Buffer + alignmentBufferSize;
		align2 = align2Buffer + alignmentBufferSize;
		char traceValue = SUBSTITUTION;
		if (typeCode == LOCAL_ALIGNMENT && startScore <= 0.0) {
			traceValue = TERMINATION;
		} else if (typeCode == GLOBAL_ALIGNMENT) {
			if (F_MATRIX(nCharString1, nCharString2) >=
					   MAX(H_MATRIX(nCharString1, nCharString2),
						   V_MATRIX(nCharString1, nCharString2))) {
				traceValue = SUBSTITUTION;
			} else if (V_MATRIX(nCharString1, nCharString2) >= H_MATRIX(nCharString1, nCharString2)) {
				traceValue = INSERTION;
			} else {
				traceValue = DELETION;
			}
		} else if (typeCode == OVERLAP_ALIGNMENT && (startRow < nCharString1 || startCol < nCharString2)) {
			if (startRow == nCharString1) {
				nCharAligned += nCharString2 - startCol;
				for (j = 1; j <= nCharString2 - startCol; j++) {
					align1--;
					align2--;
					*align1 = gapCode;
					*align2 = stringElements2.elts[nCharString2 - j];
				}
			} else {
				nCharAligned += nCharString1 - startRow;
				for (i = 1; i <= nCharString1 - startRow; i++) {
					align1--;
					align2--;
					*align1 = stringElements1.elts[nCharString1 - i];
					*align2 = gapCode;
				}
			}
		}

		/* Step 5:  Traceback through the score matrix */
		i = startRow - 1;
		j = startCol - 1;
		while (traceValue != TERMINATION && (i >= 0 || j >= 0)) {
			if (j < 0)
				traceValue = DELETION;
			else if (i < 0)
				traceValue = INSERTION;
			switch (traceValue) {
		    	case DELETION:
					traceValue = D_TRACE_MATRIX(i, j);
					if (traceValue != TERMINATION) {
						align1--;
						align2--;
						nCharAligned++;
						*align1 = stringElements1.elts[i];
			    		*align2 = gapCode;
			    		i--;
					}
		    		break;
		    	case INSERTION:
					traceValue = I_TRACE_MATRIX(i, j);
					if (traceValue != TERMINATION) {
						align1--;
						align2--;
						nCharAligned++;
			    		*align1 = gapCode;
			    		*align2 = stringElements2.elts[j];
			    		j--;
					}
		    		break;
		    	case SUBSTITUTION:
					traceValue = S_TRACE_MATRIX(i, j);
					if (traceValue != TERMINATION) {
						align1--;
						align2--;
						nCharAligned++;
			    		*align1 = stringElements1.elts[i];
			    		*align2 = stringElements2.elts[j];
			    		i--;
			    		j--;
					}
		    		break;
		    	default:
		    		error("unknown traceback code %d", traceValue);
		    		break;
			}
		}
	}

	return startScore;
}

/*
 * INPUTS
 * 'string1', 'string2':     left and right XString objects for reads
 * 'quality1', 'quality2':   left and right BString objects for quality scores
 * 'gapCode':                encoded value of the '-' letter
 *                           (raw vector of length 1)
 * 'typeCode':               type of pairwise alignment
 *                           (integer vector of length 1;
 *                            1 = 'global', 2 = 'local', 3 = 'overlap')
 * 'scoreOnly':              denotes whether or not to only return the scores
 *                           of the optimal pairwise alignment
 *                           (logical vector of length 1)
 * 'gapOpening':             gap opening cost or penalty
 *                           (double vector of length 1)
 * 'gapExtension':           gap extension cost or penalty
 *                           (double vector of length 1)
 * 'qualityLookupTable':     lookup table for translating BString bytes to
 *                           quality-based scoring matrix indices
 *                           (integer vector)
 * 'qualityMatchMatrix':     quality-based substitution matrix for matches
 * 'qualityMismatchMatrix':  quality-based substitution matrix for matches
 * 'qualityMatrixDim':       dimension of 'qualityMatchMatrix' and
 *                           'qualityMismatchMatrix'
 *                           (integer vector of lenth 2)
 * 'constantLookupTable':    lookup table for translating XString bytes to scoring
 *                           matrix indices
 *                           (integer vector)
 * 'constantMatrix':         constant substitution matrix for matches/mismatches
 *                           (double matrix)
 * 'constantMatrixDim':      dimension of 'constantMatrix'
 *                           (integer vector of length 2)
 * 
 * OUTPUT
 * Return a named list with 3 elements: 2 "externalptr" objects describing
 * the alignments and 1 integer vector containing the alignment score.
 * Note that the 2 XString objects to align should contain no gaps.
 */
SEXP align_pairwiseAlignment(
		SEXP string1,
		SEXP string2,
		SEXP quality1,
		SEXP quality2,
		SEXP gapCode,
		SEXP typeCode,
		SEXP scoreOnly,
		SEXP gapOpening,
		SEXP gapExtension,
		SEXP qualityLookupTable,
		SEXP qualityMatchMatrix,
		SEXP qualityMismatchMatrix,
		SEXP qualityMatrixDim,
		SEXP constantLookupTable,
		SEXP constantMatrix,
		SEXP constantMatrixDim)
{
	RoSeq stringElements1 = _get_XString_asRoSeq(string1);
	RoSeq stringElements2 = _get_XString_asRoSeq(string2);
	RoSeq qualityElements1 = _get_XString_asRoSeq(quality1);
	RoSeq qualityElements2 = _get_XString_asRoSeq(quality2);
	float score = pairwiseAlignment(
			stringElements1,
			stringElements2,
			qualityElements1,
			qualityElements2,
			(char) RAW(gapCode)[0],
			INTEGER(typeCode)[0],
			LOGICAL(scoreOnly)[0],
			(float) REAL(gapOpening)[0],
			(float) REAL(gapExtension)[0],
			INTEGER(qualityLookupTable),
			LENGTH(qualityLookupTable),
			REAL(qualityMatchMatrix),
			REAL(qualityMismatchMatrix),
			INTEGER(qualityMatrixDim),
			INTEGER(constantLookupTable),
			LENGTH(constantLookupTable),
			REAL(constantMatrix),
			INTEGER(constantMatrixDim));

	SEXP answer, answerNames, answerElements, tag;
	PROTECT(answer = NEW_LIST(3));
	/* set the names */
	PROTECT(answerNames = NEW_CHARACTER(3));
	SET_STRING_ELT(answerNames, 0, mkChar("align1"));
	SET_STRING_ELT(answerNames, 1, mkChar("align2"));
	SET_STRING_ELT(answerNames, 2, mkChar("score"));
	SET_NAMES(answer, answerNames);
	UNPROTECT(1);
	/* set the "align1" element */
	PROTECT(tag = NEW_RAW(nCharAligned));
	memcpy((char *) RAW(tag), align1, nCharAligned * sizeof(char));
	PROTECT(answerElements = _new_XRaw(tag));
	SET_ELEMENT(answer, 0, answerElements);
	UNPROTECT(2);
	/* set the "align2" element */
	PROTECT(tag = NEW_RAW(nCharAligned));
	memcpy((char *) RAW(tag), align2, nCharAligned * sizeof(char));
	PROTECT(answerElements = _new_XRaw(tag));
	SET_ELEMENT(answer, 1, answerElements);
	UNPROTECT(2);
	/* set the "score" element */
	PROTECT(answerElements = NEW_NUMERIC(1));
	REAL(answerElements)[0] = score;
	SET_ELEMENT(answer, 2, answerElements);
	UNPROTECT(1);
	/* answer is ready */
	UNPROTECT(1);
	return answer;
}
