#include "Biostrings.h"
#include <S.h> /* for Srealloc() */


/* ==========================================================================
 * Helper functions used for storing views in a temporary buffer like the
 * matches found by the matching algos.
 * --------------------------------------------------------------------------
 */

static int *views_startbuf, *views_endbuf;
static char **views_descbuf;
static int views_bufsize, views_count; /* views_bufsize >= views_count */

/* Reset views buffer */
void _Biostrings_reset_views_buffer()
{
	/* No memory leak here, because we use transient storage allocation */
	views_startbuf = views_endbuf = NULL;
	views_descbuf = NULL;
	views_bufsize = views_count = 0;
	return;
}

int *_Biostrings_get_views_start()
{
	return views_startbuf;
}

int *_Biostrings_get_views_end()
{
	return views_endbuf;
}

char **_Biostrings_get_views_desc()
{
	return views_descbuf;
}

/* Return the new number of views */
int _Biostrings_report_view(int start, int end, const char *desc)
{
	long new_size;
	size_t desc_size;

	if (views_count >= views_bufsize) {
		/* Buffer is full */
		if (views_bufsize == 0)
			new_size = 1024;
		else
			new_size = 2 * views_bufsize;
		views_startbuf = Srealloc((char *) views_startbuf, new_size,
						(long) views_bufsize, int);
		views_endbuf = Srealloc((char *) views_endbuf, new_size,
						(long) views_bufsize, int);
		views_descbuf = Srealloc((char *) views_descbuf, new_size,
						(long) views_bufsize, char *);
		views_bufsize = new_size;
	}
	views_startbuf[views_count] = start;
	views_endbuf[views_count] = end;
	desc_size = strlen(desc) + 1; /* + 1 for the terminating '\0' character */
	views_descbuf[views_count] = Salloc((long) desc_size, char);
	memcpy(views_descbuf[views_count], desc, desc_size);
	return ++views_count;
}

/* Return the new number of views (== number of matches) */
int _Biostrings_report_match(int Lpos, int Rpos)
{
	return _Biostrings_report_view(++Lpos, ++Rpos, "");
}

SEXP _Biostrings_get_views_LIST()
{
	SEXP ans, ans_names, ans_elt;

	PROTECT(ans = NEW_LIST(2));
	/* set the names */
	PROTECT(ans_names = allocVector(STRSXP, 2));
	SET_STRING_ELT(ans_names, 0, mkChar("start"));
	SET_STRING_ELT(ans_names, 1, mkChar("end"));
	SET_NAMES(ans, ans_names);
	UNPROTECT(1);
	/* set the "start" element */
	PROTECT(ans_elt = allocVector(INTSXP, views_count));
	memcpy(INTEGER(ans_elt), _Biostrings_get_views_start(), sizeof(int) * views_count);
	SET_ELEMENT(ans, 0, ans_elt);
	UNPROTECT(1);
	/* set the "end" element */
	PROTECT(ans_elt = allocVector(INTSXP, views_count));
	memcpy(INTEGER(ans_elt), _Biostrings_get_views_end(), sizeof(int) * views_count);
	SET_ELEMENT(ans, 1, ans_elt);
	UNPROTECT(1);
	/* ans is ready */
	UNPROTECT(1);
	return ans;
}

