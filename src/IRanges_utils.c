/****************************************************************************
 *                          Fast IRanges utilities                          *
 *                           Author: Herve Pages                            *
 ****************************************************************************/
#include "Biostrings.h"

static int debug = 0;

SEXP debug_IRanges_utils()
{
#ifdef DEBUG_BIOSTRINGS
	debug = !debug;
	Rprintf("Debug mode turned %s in 'IRanges_utils.c'\n",
	        debug ? "on" : "off");
#else
	Rprintf("Debug mode not available in 'IRanges_utils.c'\n");
#endif
	return R_NilValue;
}

typedef struct startend {
	int start;
	int end;
} StartEnd;

/*
 * Check and simplify user-specified values 'start', 'end', 'width'.
 */
static StartEnd uSEW_to_StartEnd(int start, int end, int width)
{
	StartEnd startend;

	if (start == 0)
		error("'start' must be a single >= 1, <= -1 or NA integer");
	if (end == 0)
		error("'end' must be a single >= 1, <= -1 or NA integer");
	if (width == NA_INTEGER) {
		if (start == NA_INTEGER)
			start = 1;
		if (end == NA_INTEGER)
			end = -1;
		if ((end > 0 || start < 0) && end < start)
			error("invalid ('start','end') combination");
	} else if (width < 0) {
		error("'width' must be a single >= 0 or NA integer");
	} else if ((start == NA_INTEGER) == (end == NA_INTEGER)) {
		error("either 'start' or 'end' (but not both) must be NA when 'width' is not NA");
	} else if (start == NA_INTEGER) {
		// 'end' is not NA
		if (0 < end && end < width)
			error("invalid ('end','width') combination");
		start = end - width + 1; // will be 0 iff 'end' = -1 and 'width' = 0
	} else {
		// 'end' is NA
		if (start < 0 && -start < width)
			error("invalid ('start','width') combination");
		end = start + width - 1; // will be 0 iff 'start' = 1 and 'width' = 0
	}
	// 'start' and 'end' cannot be NA anymore!
	startend.start = start;
	startend.end = end;
	return startend;
}

/*
 * --- .Call ENTRY POINT ---
 */
SEXP narrow_IRanges(SEXP x, SEXP start, SEXP end, SEXP width)
{
	StartEnd startend;
	const int *old_start, *old_width;
	int x_length, i, *new_start, *new_width, shift1, shift2;
	SEXP ans_start, ans_width, ans, ans_names;

	startend = uSEW_to_StartEnd(INTEGER(start)[0], INTEGER(end)[0], INTEGER(width)[0]);
	x_length = _get_IRanges_length(x);
	PROTECT(ans_start = NEW_INTEGER(x_length));
	PROTECT(ans_width = NEW_INTEGER(x_length));
	for (i = 0, old_start = _get_IRanges_start0(x),
		    old_width = _get_IRanges_width0(x),
		    new_start = INTEGER(ans_start),
		    new_width = INTEGER(ans_width);
	     i < x_length;
	     i++, old_start++, old_width++, new_start++, new_width++)
	{
		if (startend.start > 0)
			shift1 = startend.start - 1;
		else
			shift1 = startend.start + *old_width;
		if (startend.end < 0)
			shift2 = startend.end + 1;
		else
			shift2 = startend.end - *old_width;
		*new_width = *old_width - shift1 + shift2;
		if (shift1 < 0 || shift2 > 0 || *new_width < 0) {
			UNPROTECT(2);
			error("width of range %d is too small (%d) for this narrowing",
			      i + 1, *old_width);
		}
		*new_start = *old_start + shift1;
	}
	PROTECT(ans = NEW_LIST(2));
	PROTECT(ans_names = NEW_CHARACTER(2));
	SET_STRING_ELT(ans_names, 0, mkChar("start"));
	SET_STRING_ELT(ans_names, 1, mkChar("width"));
	SET_NAMES(ans, ans_names);
	SET_ELEMENT(ans, 0, ans_start);
	SET_ELEMENT(ans, 1, ans_width);
	UNPROTECT(4);
	return ans;
}

/*
 * --- .Call ENTRY POINT ---
 */
SEXP int_to_adjacent_ranges(SEXP x)
{
	SEXP ans;
	int x_length, i, *x_elt, *ans_elt0, *ans_elt1;

	x_length = LENGTH(x);
	PROTECT(ans = NEW_INTEGER(x_length));
	if (x_length >= 1)
		INTEGER(ans)[0] = 1;
	if (x_length >= 2)
		for (i = 1, x_elt = INTEGER(x),
			    ans_elt0 = INTEGER(ans),
			    ans_elt1 = INTEGER(ans)+1;
		     i < x_length;
		     i++, x_elt++, ans_elt0++, ans_elt1++) {
			*ans_elt1 = *ans_elt0 + *x_elt;
		}
	UNPROTECT(1);
	return ans;
}


/****************************************************************************
 * Reduction (aka extracting the frame)
 */

static RangeBuf reduced_ranges;
static int max_end, inframe_offset;

static void add_to_reduced_ranges(int start, int width)
{
	int buf_length, end, gap;

	buf_length = reduced_ranges.start.nelt;
	end = start + width - 1;
	if (buf_length == 0 || (gap = start - max_end - 1) > 0) {
		_RangeBuf_insert_at(&reduced_ranges, buf_length, start, width);
		if (buf_length == 0)
			inframe_offset = start - 1;
		else
			inframe_offset += gap;
		max_end = end;
		return;
	}
	if (end <= max_end)
		return;
	reduced_ranges.width.elts[buf_length - 1] += end - max_end;
	max_end = end;
	return;
}

static void reduce_ranges(int length, const int *start, const int *width, int *inframe_start)
{
	int i, j;
	IntBuf start_order;

	start_order = _new_IntBuf(length, 0, 0);
	get_intorder(length, start, start_order.elts);
	reduced_ranges = _new_RangeBuf(0, 0);
	for (i = 0; i < length; i++) {
		j = start_order.elts[i];
		add_to_reduced_ranges(start[j], width[j]);
		if (inframe_start != NULL)
			inframe_start[j] = start[j] - inframe_offset;
	}
	return;
}

/*
 * --- .Call ENTRY POINT ---
 */
SEXP reduce_IRanges(SEXP x, SEXP with_inframe_start)
{
	int x_length;
	const int *x_start, *x_width;
	SEXP ans, ans_names, ans_inframe_start;
	int *inframe_start;

	x_length = _get_IRanges_length(x);
	x_start = _get_IRanges_start0(x);
	x_width = _get_IRanges_width0(x);
	if (LOGICAL(with_inframe_start)[0]) {
		PROTECT(ans_inframe_start = NEW_INTEGER(x_length));
		inframe_start = INTEGER(ans_inframe_start);
	} else {
		inframe_start = NULL;
	}
	reduce_ranges(x_length, x_start, x_width, inframe_start);

	PROTECT(ans = NEW_LIST(3));
	PROTECT(ans_names = NEW_CHARACTER(3));
	SET_STRING_ELT(ans_names, 0, mkChar("start"));
	SET_STRING_ELT(ans_names, 1, mkChar("width"));
	SET_STRING_ELT(ans_names, 2, mkChar("inframe.start"));
	SET_NAMES(ans, ans_names);
	UNPROTECT(1);
	SET_ELEMENT(ans, 0, _IntBuf_asINTEGER(&(reduced_ranges.start)));
	SET_ELEMENT(ans, 1, _IntBuf_asINTEGER(&(reduced_ranges.width)));
	if (inframe_start != NULL) {
		SET_ELEMENT(ans, 2, ans_inframe_start);
		UNPROTECT(1);
	}
	UNPROTECT(1);
	return ans;
}

