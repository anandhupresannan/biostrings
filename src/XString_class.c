/****************************************************************************
 *                  Basic manipulation of XString objects                   *
 *                           Author: Herve Pages                            *
 ****************************************************************************/
#include "Biostrings.h"
#include "IRanges_interface.h"

static int debug = 0;

SEXP debug_XString_class()
{
#ifdef DEBUG_BIOSTRINGS
	debug = !debug;
	Rprintf("Debug mode turned %s in file %s\n",
		debug ? "on" : "off", __FILE__);
#else
	Rprintf("Debug mode not available in file %s\n", __FILE__);
#endif
	return R_NilValue;
}


/****************************************************************************
 * Encoding/decoding XString data.
 */

static ByteTrTable DNA_enc_byte2code, DNA_dec_byte2code,
		 RNA_enc_byte2code, RNA_dec_byte2code;

const ByteTrTable *get_enc_byte2code(const char *classname)
{
	if (strcmp(classname, "DNAString") == 0)
		return (const ByteTrTable *) &DNA_enc_byte2code;
	else if (strcmp(classname, "RNAString") == 0)
		return (const ByteTrTable *) &RNA_enc_byte2code;
	return NULL;
}

const ByteTrTable *get_dec_byte2code(const char *classname)
{
	if (strcmp(classname, "DNAString") == 0)
		return (const ByteTrTable *) &DNA_dec_byte2code;
	else if (strcmp(classname, "RNAString") == 0)
		return (const ByteTrTable *) &RNA_dec_byte2code;
	return NULL;
}

/*
 * --- .Call ENTRY POINT ---
 */
SEXP init_DNAlkups(SEXP enc_lkup, SEXP dec_lkup)
{
	_init_ByteTrTable_with_lkup(DNA_enc_byte2code, enc_lkup);
	_init_ByteTrTable_with_lkup(DNA_dec_byte2code, dec_lkup);
	return R_NilValue;
}

char _DNAencode(char c)
{
	int code;

	code = DNA_enc_byte2code[(unsigned char) c];
	if (code == NA_INTEGER)
		error("_DNAencode(): key %d not in lookup table", (int) c);
	return code;
}

char _DNAdecode(char code)
{
	int c;

	c = DNA_dec_byte2code[(unsigned char) code];
	if (c == NA_INTEGER)
		error("_DNAdecode(): key %d not in lookup table", (int) code);
	return c;
}

/*
 * --- .Call ENTRY POINT ---
 */
SEXP init_RNAlkups(SEXP enc_lkup, SEXP dec_lkup)
{
	_init_ByteTrTable_with_lkup(RNA_enc_byte2code, enc_lkup);
	_init_ByteTrTable_with_lkup(RNA_dec_byte2code, dec_lkup);
	return R_NilValue;
}

char _RNAencode(char c)
{
	int code;

	code = RNA_enc_byte2code[(unsigned char) c];
	if (code == NA_INTEGER)
		error("_RNAencode(): key %d not in lookup table", (int) c);
	return code;
}

char _RNAdecode(char code)
{
	int c;

	c = RNA_dec_byte2code[(unsigned char) code];
	if (c == NA_INTEGER)
		error("_RNAdecode(): key %d not in lookup table", (int) code);
	return (char) c;
}


/****************************************************************************
 * Low-level manipulation of XString objects.
 */

/*
 * Creating a set of sequences (RoSeqs struct) from an XString object.
 */
static RoSeqs new_RoSeqs_from_XString(int nelt, SEXP x)
{
	RoSeqs seqs;
	cachedCharSeq *elt1;
	int i;

	seqs = _alloc_RoSeqs(nelt);
	for (i = 0, elt1 = seqs.elts; i < nelt; i++, elt1++)
		*elt1 = cache_XRaw(x);
	return seqs;
}

/*
 * Making an XString object from the sequences referenced by a RoSeqs struct.
 * Assume that these sequences are NOT already encoded.
 */
SEXP _new_XString_from_RoSeqs(const char *classname, const RoSeqs *seqs)
{
	const ByteTrTable *byte2code;
	SEXP lkup, shared, ans;

	byte2code = get_enc_byte2code(classname);
	PROTECT(lkup = _new_lkup_from_ByteTrTable(byte2code));
	PROTECT(shared = _new_SharedRaw_from_RoSeqs(seqs, lkup));
	PROTECT(ans = new_XVector(classname, shared, 0, get_SharedVector_length(shared)));
	UNPROTECT(3);
	return ans;
}


/****************************************************************************
 * Other stuff...
 */

void _Ocopy_cachedCharSeq_to_XString(SEXP out, int start,
		const cachedCharSeq *in, int encode)
{
	int offset;
	const ByteTrTable *enc_byte2code;

	offset = get_XVector_offset(out);
	enc_byte2code = encode ? get_enc_byte2code(get_classname(out)) : NULL;
	Ocopy_cachedCharSeq_to_SharedRaw_offset(
		get_XVector_shared(out), offset + start - 1,
		in,
		*enc_byte2code, BYTETRTABLE_LENGTH);
	return;
}

