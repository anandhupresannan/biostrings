#include "Biostrings.h"
#include "IRanges_interface.h"

/*
 * --- .Call ENTRY POINT ---
 * Return an XString object.
 */
SEXP inject_code(SEXP x, SEXP start, SEXP width, SEXP code)
{
	const char *x_classname;
	RoSeq x_seq;
	int nranges, i, s, w;
	const int *s_p, *w_p;
	SEXP tag, xdata, ans;

	x_classname = get_classname(x);
	x_seq = _get_XString_asRoSeq(x);
	nranges = LENGTH(start); /* must be == LENGTH(width) */
	PROTECT(tag = NEW_RAW(x_seq.nelt));
	memcpy(RAW(tag), x_seq.elts, x_seq.nelt);
	for (i = 0, s_p = INTEGER(start),  w_p = INTEGER(width);
	     i < nranges;
	     i++, s_p++, w_p++)
	{
		s = *s_p;
		w = *w_p;
		if (s == NA_INTEGER || w == NA_INTEGER)
			error("Biostrings internal error in inject_code():"
			      "NAs in 'start' or 'width' are not supported");
		s--; // 0-based start (offset)
		if (s < 0 || w < 0 || s + w > x_seq.nelt)
			error("Biostrings internal error in inject_code():"
			      "invalid start/width values");
		memset(RAW(tag) + s, INTEGER(code)[0], w);
	}
	PROTECT(xdata = new_SequencePtr("RawPtr", tag));
	PROTECT(ans = new_XSequence(x_classname, xdata, 0, LENGTH(tag)));
	UNPROTECT(3);
	return ans;
}

