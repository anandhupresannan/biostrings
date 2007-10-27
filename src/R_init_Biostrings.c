#include "Biostrings.h"

static const R_CallMethodDef callMethods[] = {

/* utils.c */
	{"Biostrings_debug_utils", (DL_FUNC) &Biostrings_debug_utils, 0},

/* XRaw.c */
	{"Biostrings_debug_XRaw", (DL_FUNC) &Biostrings_debug_XRaw, 0},

	{"Biostrings_sexp_address", (DL_FUNC) &Biostrings_sexp_address, 1},
	{"Biostrings_xp_show", (DL_FUNC) &Biostrings_xp_show, 1},
	{"Biostrings_xp_new", (DL_FUNC) &Biostrings_xp_new, 0},
	{"Biostrings_safe_explode", (DL_FUNC) &Biostrings_safe_explode, 1},

	{"Biostrings_XRaw_alloc", (DL_FUNC) &Biostrings_XRaw_alloc, 2},
	{"Biostrings_XRaw_get_show_string", (DL_FUNC) &Biostrings_XRaw_get_show_string, 1},
	{"Biostrings_XRaw_length", (DL_FUNC) &Biostrings_XRaw_length, 1},
	{"Biostrings_XRaw_memcmp", (DL_FUNC) &Biostrings_XRaw_memcmp, 5},

	{"Biostrings_XRaw_copy_from_i1i2", (DL_FUNC) &Biostrings_XRaw_copy_from_i1i2, 4},
	{"Biostrings_XRaw_copy_from_subset", (DL_FUNC) &Biostrings_XRaw_copy_from_subset, 3},

	{"Biostrings_XRaw_read_chars_from_i1i2", (DL_FUNC) &Biostrings_XRaw_read_chars_from_i1i2, 3},
	{"Biostrings_XRaw_read_chars_from_subset", (DL_FUNC) &Biostrings_XRaw_read_chars_from_subset, 2},
	{"Biostrings_XRaw_write_chars_to_i1i2", (DL_FUNC) &Biostrings_XRaw_write_chars_to_i1i2, 4},
	{"Biostrings_XRaw_write_chars_to_subset", (DL_FUNC) &Biostrings_XRaw_write_chars_to_subset, 3},

	{"XRaw_read_ints_from_i1i2", (DL_FUNC) &XRaw_read_ints_from_i1i2, 3},
	{"XRaw_read_ints_from_subset", (DL_FUNC) &XRaw_read_ints_from_subset, 2},
	{"XRaw_write_ints_to_i1i2", (DL_FUNC) &XRaw_write_ints_to_i1i2, 4},
	{"XRaw_write_ints_to_subset", (DL_FUNC) &XRaw_write_ints_to_subset, 3},

	{"XRaw_read_enc_chars_from_i1i2", (DL_FUNC) &XRaw_read_enc_chars_from_i1i2, 4},
	{"XRaw_read_enc_chars_from_subset", (DL_FUNC) &XRaw_read_enc_chars_from_subset, 3},
	{"XRaw_write_enc_chars_to_i1i2", (DL_FUNC) &XRaw_write_enc_chars_to_i1i2, 5},
	{"XRaw_write_enc_chars_to_subset", (DL_FUNC) &XRaw_write_enc_chars_to_subset, 4},

	{"XRaw_read_complexes_from_i1i2", (DL_FUNC) &XRaw_read_complexes_from_i1i2, 4},
	{"XRaw_read_complexes_from_subset", (DL_FUNC) &XRaw_read_complexes_from_subset, 3},

	{"XRaw_loadFASTA", (DL_FUNC) &XRaw_loadFASTA, 4},

/* XInteger.c */
	{"XInteger_alloc", (DL_FUNC) &XInteger_alloc, 2},
	{"XInteger_show", (DL_FUNC) &XInteger_show, 1},
	{"XInteger_length", (DL_FUNC) &XInteger_length, 1},
	{"XInteger_memcmp", (DL_FUNC) &XInteger_memcmp, 5},

	{"XInteger_read_ints_from_i1i2", (DL_FUNC) &XInteger_read_ints_from_i1i2, 3},
	{"XInteger_read_ints_from_subset", (DL_FUNC) &XInteger_read_ints_from_subset, 2},
	{"XInteger_write_ints_to_i1i2", (DL_FUNC) &XInteger_write_ints_to_i1i2, 4},
	{"XInteger_write_ints_to_subset", (DL_FUNC) &XInteger_write_ints_to_subset, 3},

/* reverseComplement.c */
        {"XRaw_translate_copy_from_i1i2", (DL_FUNC) &XRaw_translate_copy_from_i1i2, 5},
        {"XRaw_translate_copy_from_subset", (DL_FUNC) &XRaw_translate_copy_from_subset, 4},
        {"XRaw_reverse_copy_from_i1i2", (DL_FUNC) &XRaw_reverse_copy_from_i1i2, 4},
        {"XRaw_reverse_translate_copy_from_i1i2", (DL_FUNC) &XRaw_reverse_translate_copy_from_i1i2, 5},

/* char_frequency.c */
	{"char_frequency", (DL_FUNC) &char_frequency, 3},

/* normalize_views.c */
	{"Biostrings_normalize_views", (DL_FUNC) &Biostrings_normalize_views, 2},

/* match_naive.c */
	{"match_naive_debug", (DL_FUNC) &match_naive_debug, 0},
	{"match_naive_exact", (DL_FUNC) &match_naive_exact, 7},
	{"match_naive_fuzzy", (DL_FUNC) &match_naive_fuzzy, 9},

/* match_boyermoore.c */
	{"match_boyermoore_debug", (DL_FUNC) &match_boyermoore_debug, 0},
	{"match_boyermoore", (DL_FUNC) &match_boyermoore, 7},

/* match_shiftor.c */
	{"match_shiftor_debug", (DL_FUNC) &match_shiftor_debug, 0},
	{"bits_per_long", (DL_FUNC) &bits_per_long, 0},
	{"match_shiftor", (DL_FUNC) &match_shiftor, 9},

/* pmatchPattern.c */
	{"lcprefix", (DL_FUNC) &lcprefix, 6},
	{"lcsuffix", (DL_FUNC) &lcsuffix, 6},

/* align_needwunsQS.c */
	{"align_needwunsQS", (DL_FUNC) &align_needwunsQS, 11},

	{NULL, NULL, 0}
};

void R_init_Biostrings(DllInfo *info)
{
	/* Lots of code around assumes that sizeof(Rbyte) == sizeof(char) */
	if (sizeof(Rbyte) != sizeof(char))
		error("sizeof(Rbyte) != sizeof(char)");
	R_registerRoutines(info, NULL, callMethods, NULL, NULL);
}

