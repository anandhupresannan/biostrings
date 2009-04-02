#include "Biostrings_interface.h"

#define DEFINE_CCALLABLE_STUB(retT, stubname, Targs, args) \
typedef retT(*__ ## stubname ## _funtype__)Targs; \
retT stubname Targs \
{ \
	static __ ## stubname ## _funtype__ fun = NULL; \
	if (fun == NULL) \
		fun = (__ ## stubname ## _funtype__) R_GetCCallable("Biostrings", "_" #stubname); \
	return fun args; \
}

/*
 * Using the above macro when retT (the returned type) is void will make Sun
 * Studio 12 C compiler unhappy. So we need to use the following macro to
 * handle that case.
 */
#define DEFINE_NOVALUE_CCALLABLE_STUB(stubname, Targs, args) \
typedef void(*__ ## stubname ## _funtype__)Targs; \
void stubname Targs \
{ \
	static __ ## stubname ## _funtype__ fun = NULL; \
	if (fun == NULL) \
		fun = (__ ## stubname ## _funtype__) R_GetCCallable("Biostrings", "_" #stubname); \
	fun args; \
	return; \
}


/*
 * Stubs for callables defined in RoSeq_utils.c
 */

DEFINE_CCALLABLE_STUB(SEXP, new_STRSXP_from_RoSeqs,
	(const RoSeqs *seqs, SEXP lkup),
	(              seqs,      lkup)
);

DEFINE_CCALLABLE_STUB(RoSeqs, new_RoSeqs_from_CharAEAE,
	(const CharAEAE *char_aeae),
	(                char_aeae)
);

DEFINE_CCALLABLE_STUB(SEXP, new_IRanges_from_RoSeqs,
	(const char *classname, const RoSeqs *seqs),
	(            classname,               seqs)
);


/*
 * Stubs for callables defined in XString_class.c
 */

DEFINE_CCALLABLE_STUB(char, DNAencode,
	(char c),
	(     c)
)

DEFINE_CCALLABLE_STUB(char, DNAdecode,
	(char code),
	(     code)
)

DEFINE_CCALLABLE_STUB(char, RNAencode,
	(char c),
	(     c)
)

DEFINE_CCALLABLE_STUB(char, RNAdecode,
	(char code),
	(     code)
)

DEFINE_CCALLABLE_STUB(RoSeq, get_XString_asRoSeq,
	(SEXP x),
	(     x)
)


/*
 * Stubs for callables defined in XStringSet_class.c
 */

DEFINE_CCALLABLE_STUB(const char *, get_XStringSet_baseClass,
	(SEXP x),
	(     x)
)

DEFINE_CCALLABLE_STUB(int, get_XStringSet_length,
	(SEXP x),
	(     x)
)

DEFINE_CCALLABLE_STUB(CachedXStringSet, new_CachedXStringSet,
	(SEXP x),
	(     x)
)

DEFINE_CCALLABLE_STUB(RoSeq, get_CachedXStringSet_elt_asRoSeq,
	(CachedXStringSet *x, int i),
	(                  x,     i)
)

DEFINE_CCALLABLE_STUB(RoSeq, get_XStringSet_elt_asRoSeq,
	(SEXP x, int i),
	(     x,     i)
)

DEFINE_CCALLABLE_STUB(SEXP, new_XStringSet_from_RoSeqs,
	(const char *baseClass, const RoSeqs *seqs),
	(            baseClass,               seqs)
)

DEFINE_NOVALUE_CCALLABLE_STUB(set_XStringSet_names,
	(SEXP x, SEXP names),
	(     x,      names)
)

DEFINE_CCALLABLE_STUB(SEXP, alloc_XStringSet,
	(const char *baseClass, int length, int super_length),
	(            baseClass,     length,     super_length)
)

DEFINE_NOVALUE_CCALLABLE_STUB(write_RoSeq_to_CachedXStringSet_elt,
	(CachedXStringSet *x, int i, const RoSeq *seq, int encode),
	(                  x,     i,              seq,     encode)
)

DEFINE_NOVALUE_CCALLABLE_STUB(write_RoSeq_to_XStringSet_elt,
	(SEXP x, int i, const RoSeq *seq, int encode),
	(     x,     i,              seq,     encode)
)


/*
 * Stubs for callables defined in match_reporting.c
 */

DEFINE_NOVALUE_CCALLABLE_STUB(init_match_reporting,
	(SEXP mode),
	(     mode)
)

DEFINE_NOVALUE_CCALLABLE_STUB(drop_reported_matches,
	(),
	()
)

DEFINE_NOVALUE_CCALLABLE_STUB(shift_match_on_reporting,
	(int shift),
	(    shift)
)

DEFINE_NOVALUE_CCALLABLE_STUB(report_match,
	(int start, int width),
	(    start,     width)
)

DEFINE_CCALLABLE_STUB(SEXP, reported_matches_asSEXP,
	(),
	()
)

