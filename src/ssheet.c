/*
 * ssheet.c -- definition of the languages style sheets
 *
 * Copyright (c) 1988-93 Miguel Santana
 * Copyright (c) 1995-99 Akim Demaille, Miguel Santana
 *
 */

/*
 * This file is part of a2ps.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "a2ps.h"
#include "routines.h"
#include "ssheet.h"
#include "pathwalk.h"
#include "jobs.h"
#include "message.h"
#include "filtdir.h"
#include "versions.h"
#include "document.h"
#include "metaseq.h"
#include "title.h"
#include "quotearg.h"

/* Suffix of the style sheet files */
#define SSH_SUFFIX ".ssh"

/* Separator of ancestors in mixed style sheets. */
#define MIXED_SHEET_SEP ",;+"

extern a2ps_job * job;
extern struct hash_table_s * style_sheets;

/* Comes from regex */
extern char * re_syntax_table;

/* This function is defined in sshparser.y, but I don't know
 * how to make it appear in a .h file */
struct style_sheet * parse_style_sheet PARAMS ((const char * filename));

/* Computes the sum of the versions and the highest requirement
 * of the sheet with key in KEYS */
static inline void style_sheets_versions PARAMS ((struct darray * keys,
						  int sum [4],
						  int requirement [4]));

/* return true iff OK */
static bool style_sheet_check PARAMS ((struct style_sheet * sheet));

/* We drop in the alphabets[0] an invalid value.  It allows to see if
 * the alphabets were defined or not, so that the default alphabets
 * can be set when finalizing the sheet.  Note that we cannot set the
 * default alphabets a priori, because we need to be able to inherit
 * from ancestors. */
#define ALPHABET_UNDEFINED_TAG	127
#define ALPHABET_IS_UNDEFINED(_alpha_)	\
	((_alpha_)[0] == ALPHABET_UNDEFINED_TAG)
#define ALPHABET_UNDEFINE(_alpha_)	\
	((_alpha_)[0] = ALPHABET_UNDEFINED_TAG)

#define DEFAULT_ALPHA1 ((const uchar *) \
	"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_")
#define DEFAULT_ALPHA2 ((const uchar *) \
	"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789")

static void
alphabet_self_print (char *a, FILE *s)
{
  if (ALPHABET_IS_UNDEFINED (a))
    fputs ("<undefined>\n", s);
  else
    {
      int c;

      fputc ('\"', s);
      for (c = 0 ; c < 256 ; c++)
	if (a [c])
	  fputc (c, s);
      fputs ("\"\n", s);
    }
}

#define DEFAULT_SENSITIVENESS case_insensitive

struct pattern *
new_pattern (char * pattern, size_t len)
{
  struct pattern * res = XMALLOC (struct pattern, 1);
  res->pattern = pattern;
  res->len = len;
  return res;
}

/*
 * The faced_string
 */
struct faced_string *
faced_string_new (uchar * string, int reg_ref, struct fface_s face)
{
  struct faced_string * res = XMALLOC (struct faced_string, 1);
  res->string = string;
  res->reg_ref = reg_ref;
  res->face = face;
  return res;
}

/*
 * Report a faced_string
 */
static void
faced_string_self_print (struct faced_string * faced_string, FILE * stream)
{
  if (faced_string->string)
    fprintf (stream, "\"%s\": ", faced_string->string);
  else
    fprintf (stream, "\\%d: ", faced_string->reg_ref);

  fface_self_print (faced_string->face, stream);
}

inline static void
faced_string_free (struct faced_string * faced_string)
{
  XFREE (faced_string->string);
  free (faced_string);
}

/*
 * Replace all the No_fface's by FACE
 */
inline static void
faced_string_set_no_face (struct faced_string * str, struct fface_s face)
{
  if (fface_squ (str->face, No_fface))
    str->face = face;
}

/************************************************************************/
/*	The version handling						*/
/************************************************************************/
void
style_sheet_set_version (struct style_sheet * sheet,
			 const char * version_string)
{
  string_to_version (version_string, sheet->version);
}

/*
 * Set the version of a2ps required for the processing of SHEET.
 * Return false if the requirement is higher than VERSION.
 */
int
style_sheet_set_requirement (struct style_sheet * sheet,
			     const char * version_string)
{
  int a2ps_version [4];

  string_to_version (version_string, sheet->requirement);
  string_to_version (VERSION, a2ps_version);

  if (version_cmp (sheet->requirement, a2ps_version) > 0)
    return false;

  return true;
}

/************************************************************************/
/*	Rhs darray						*/
/************************************************************************/
/*
 * New rhs array
 */
struct darray *
rhs_new (void)
{
  struct darray * res;
  res = da_new ("Rhs", 2, da_geometrical, 2,
		(da_print_func_t) faced_string_self_print, NULL);
  return res;
}

/*
 * New rhs array with a single argument
 */
struct darray *
rhs_new_single (uchar * string, int reg_ref, struct fface_s face)
{
  struct darray * res;
  res = da_new ("Rhs", 2, da_geometrical, 2,
		(da_print_func_t) faced_string_self_print, NULL);
  da_append (res, faced_string_new (string, reg_ref, face));
  return res;
}

/*
 * Replace all the No_fface's by FACE
 */
inline static void
rhs_set_no_face (struct darray * dest, struct fface_s face)
{
  size_t i;

  for (i = 0 ; i < dest->len ; i++)
    if (fface_squ (((struct faced_string *) dest->content [i])->face,
		   No_fface))
      ((struct faced_string *) dest->content [i])->face = face;
}

/*
 * Add a rule in the rhs array,
 * Don't forget to scan the string, because it may have actually
 * regex references in it, which must be resolved
 */
void
rhs_add (struct darray * dest, struct faced_string * str)
{
  da_append (dest, str);
}

/*
 * Report a rhs array
 */
void
rhs_self_print (struct darray * rhs, FILE * stream)
{
  size_t i;

  putc ('(', stream);
  for (i = 0 ; i < rhs->len ; i++)
    faced_string_self_print ((struct faced_string *) rhs->content [i], stream);
  putc (')', stream);
}

/************************************************************************/
/*	P-rules								*/
/************************************************************************/
/*
 * Dealing with the rules and operators
 */
inline static struct rule *
rule_new_internal_word (uchar * word,
			struct darray * rhs)
{
  struct rule * res = XMALLOC (struct rule, 1);

  res->word = word;
  res->regex = NULL;
  res->rhs = rhs;
  return res;
}

/*
 * Dealing with the rules and operators
 */
inline static struct rule *
rule_new_internal_regexp (struct pattern *pattern,
			  char *regexp, int regexp_len,
			  struct darray *rhs,
			  const char *filename, size_t line)
{
  struct rule *res = XMALLOC (struct rule, 1);
  const char *error_msg;

  /* This is a regular expression.  We want to keep the original
   * pattern to ease the debugging of a style sheet */
  res->word = (uchar *) pattern->pattern;

  /* Build the regex structure, and compile the pattern */
  res->regex = XMALLOC (struct re_pattern_buffer, 1);
  res->regex->translate = NULL;
  res->regex->fastmap = NULL;
  res->regex->buffer = NULL;
  res->regex->allocated = 0;

  error_msg = re_compile_pattern (regexp, regexp_len, res->regex);
  /* The pattern is nul terminated, no fear to have */
  if (error_msg)
    error_at_line (1, 0, filename, line,
		   _("cannot compile regular expression `%s': %s"),
		   regexp, error_msg);
  free (pattern);
  res->rhs = rhs;
  return res;
}

/*
 * Dealing with the rules and operators
 */
struct rule *
rule_new (uchar * word, struct pattern * pattern,
	  struct darray * rhs,
	  const char *filename, size_t line)
{
  if (pattern)
    return rule_new_internal_regexp (pattern,
				     pattern->pattern, pattern->len,
				     rhs,
				     filename, line);
  else
    return rule_new_internal_word (word,
				   rhs);
}


/*-------------------------------------------------------------------.
| Dealing with the rules and operators.  This is pretty much the     |
| same as the previous case, but the difference is that the regexp   |
| must be built appending a `\\b' at the end, to make sure that, for |
| instance, as a keyword matcher `/c*f/' does not match `ccfe': the  |
| keyword is not separated.                                          |
`-------------------------------------------------------------------*/

struct rule *
keyword_rule_new (uchar * word, struct pattern * pattern,
		  struct darray * rhs,
		  const char *filename, size_t line)
{
  int pattern_len = 0;
  char * pattern_to_compile = NULL;

  if (pattern)
    {
      pattern_len = pattern->len + strlen ("\\b()\\b");
      pattern_to_compile = ALLOCA (char, pattern_len + 1);
      sprintf (pattern_to_compile, "\\b(%s)\\b", pattern->pattern);
      return rule_new_internal_regexp (pattern,
				       pattern_to_compile, pattern_len,
				       rhs,
				       filename, line);
    }
  else
    return rule_new_internal_word (word,
				   rhs);
}


/*--------------.
| Free a rule.  |
`--------------*/

static void
free_rule (struct rule * rule)
{
  XFREE (rule->word);
  if (rule->regex)
    {
      regfree (rule->regex);
      free (rule->regex);
    }
  da_free (rule->rhs, (da_map_func_t) faced_string_free);
  free (rule);
}

/*
 * Compare two rules.
 * Note: they should be string-rules, not regex-rule
 */
static int
rule_cmp (struct rule * k1, struct rule * k2)
{
  return ustrcmp (k1->word, k2->word);
}

static void
rule_self_print (struct rule * rule, FILE * stream)
{
  if (rule->regex)
    fprintf (stream, "/%s/ -> ", rule->word);
  else
    fprintf (stream, "\"%s\" -> ", rule->word);
  rhs_self_print (rule->rhs, stream);
}

/*
 * Replace all the No_fface's that may be in the array of
 * rules, to FACE
 */
static inline void
rule_set_no_face (struct rule * rule, struct fface_s face)
{
  rhs_set_no_face (rule->rhs, face);
}

static void
rules_set_no_faces (struct darray * arr, struct fface_s face)
{
  size_t i;

  for (i = 0 ; i < arr->len ; i++)
    rule_set_no_face (((struct rule *) arr->content[i]), face);
}


/*
 * `words' are a specialization of darray, to capture the fact
 * that it is faster to look for a prefix of a string taking
 * advantage to know that it can only be in the interval
 * of candidates that start with the same first letter
 *
 */
struct words *
words_new (/* Regular darray parameters */
	      const char * name_strings, const char * name_regexps,
	      size_t size, size_t increment)
{
  int c;
  struct words * res = XMALLOC (struct words, 1);

  /* Initialize the underlying darray of strings */
  res->strings = da_new (name_strings, size,
			 da_linear, increment,
			 (da_print_func_t) rule_self_print,
			 (da_cmp_func_t) rule_cmp);

  /* Initialize the underlying darray of regexps */
  res->regexps = da_new (name_regexps, size,
			 da_linear, increment,
			 (da_print_func_t) rule_self_print,
			 (da_cmp_func_t) NULL);

  /* Set min and max intervals to void values */
  for (c = 0 ; c < 256 ; c ++)
    res->min[c] = res->max[c] = NULL;

  return res;
}

static inline void
words_erase (struct words * words)
{
  /* Do not free the items, there may be pointers onto them */
  da_erase (words->strings);
  da_erase (words->regexps);

  free (words);
}

void
words_free (struct words * words)
{
  da_free (words->strings, (da_map_func_t) free_rule);
  da_free (words->regexps, (da_map_func_t) free_rule);
  free (words);
}

static void
words_self_print (struct words * words, FILE * stream)
{
  if (words->regexps->len)
    da_self_print (words->regexps, stream);
  if (words->strings->len)
    da_self_print (words->strings, stream);
}

void
words_add_string (struct words * words, struct rule * rule)
{
  da_append (words->strings, rule);
}

void
words_add_regex (struct words * words, struct rule * rule)
{
  da_append (words->regexps, rule);
}

void
words_set_no_face (struct words * words, struct fface_s face)
{
  rules_set_no_faces (words->strings, face);
  rules_set_no_faces (words->regexps, face);
}

/*
 * Add the content of NEW in WORDS.
 * If in WORDS there is yet a rule with the same string matcher
 *    than in NEW, then the one of NEW is the winner
 * No special care is made for the regex matcher
 * NEW is destroyed.
 */
void
words_merge_rules_unique (struct words * words, struct words * new)
{
  /* Select those that are regexps */
  da_concat (words->regexps, new->regexps);

  /* Include the strings */
  da_qsort (new->strings);
  da_merge (words->strings, new->strings,
	    (da_map_func_t) free_rule, da_2_wins);

  /* Empty the structure, but don't free, since words->regexps
   * keeps a reference to them */
  words_erase (new);
}

/*
 * Finish setting up the keywords recognition mechanism
 */
static void
words_finalize (struct words * list)
{
  size_t i;
  struct rule ** content;

  /* Sort in alphabetical order */
  da_qsort (list->strings);

  /* Build two arrays which contains for each letter, its first
   * and last occurence index in (*DEST) array */
  content = (struct rule **) list->strings->content;
  for (i = 0 ; i < list->strings->len ; i++) {
    if (list->min [content[i]->word[0]] == NULL)
      list->min [content[i]->word[0]] = content + i;
    list->max [content[i]->word[0]] = content + i;
  }
}

/************************************************************************/
/*		Handling the ancestors of a style sheet 		*/
/************************************************************************/
static struct darray *
ancestors_new (void)
{
  return da_new ("Ancestors", 2,
		 da_linear, 2,
		 (da_print_func_t) da_str_print, NULL);
}

static void
ancestors_finalize (struct style_sheet * sheet)
{
  int i;
  struct style_sheet * ancestor = NULL;

  if (sheet->ancestors->len == 0)
    return;	/* No inheritance */

  /* The string defined rules must be read in order, so that the last
   * definition overwrites the first (and not the converse) */
  for (i = 0 ; i < (int) sheet->ancestors->len ; i++)
    {
      ancestor = get_style_sheet ((char *) sheet->ancestors->content [i]);
      /* It is possible that the ancestors is not available */
      if (!ancestor)
	continue;

      /* Inherit from their string keywords.  The keywords we already
	 have are the oldest, hence the one to keep.  This is why
	 da_1_wins.  Do not free the item, coz' your being killing
	 another style sheet! */
      da_merge (sheet->keywords->strings, ancestor->keywords->strings,
		NULL, da_1_wins);
      /* Inherit from their string operators */
      da_merge (sheet->operators->strings, ancestor->operators->strings,
		NULL, da_1_wins);
    }

  /* We want to inherit from the _last_ alphabets if it has not been
     defined in this sheet.  The problem is when there are several
     ancestors, and some are missing.  To maximize the chance to get
     something defined last, we drop the tests for alphabets and
     sensitiveness in the backward loop.  The cost is low (in general
     there are few ancestors), and ensures that the last existing (not
     the last required) ancestor defines the syntactic definitions */
  for (i = (int) sheet->ancestors->len - 1; i >= 0 ; i--)
    {
      ancestor = get_style_sheet ((char *) sheet->ancestors->content [i]);
      if (!ancestor)
	continue;

      /* Inherit of the _last_ alphabets if it has not been defined in
       * this sheet.  */
      if (ALPHABET_IS_UNDEFINED (sheet->alpha1))
	memcpy (sheet->alpha1, ancestor->alpha1, sizeof (uchar) * 256);
      if (ALPHABET_IS_UNDEFINED (sheet->alpha2))
	memcpy (sheet->alpha2, ancestor->alpha2, sizeof (uchar) * 256);

      /* Inherit of the _last_ case sensitivity */
      if (sheet->sensitiveness == case_undefined)
	sheet->sensitiveness = ancestor->sensitiveness;

      /* Inherit from their regex keywords */
      da_prefix (sheet->keywords->regexps,
		 ancestor->keywords->regexps);

      /* Inherit from their regex operators */
      da_prefix (sheet->operators->regexps,
		 ancestor->operators->regexps);

      /* Inherit from their sequences */
      /* They must be read in reverse order */
      da_prefix (sheet->sequences, ancestor->sequences);
    }
}

/************************************************************************/
/*		Creating, loading a style sheet				*/
/************************************************************************/
/*
 * Create a sequence
 */
struct sequence *
sequence_new (struct rule * Open,
	      struct fface_s in_face,
	      struct words * Close,
	      struct words * exceptions)
{
  struct sequence * res = XMALLOC (struct sequence, 1);

  /* Make sure to set the face of those with No_fface */
  rule_set_no_face (Open, in_face);
  res->open = Open;

  res->face = in_face;

  words_set_no_face (exceptions, in_face);
  words_finalize (exceptions);
  res->exceptions = exceptions;

  words_set_no_face (Close, in_face);
  words_finalize (Close);
  res->close = Close;
  return res;
}

/*
 * Release a sequence.
 */
void
free_sequence (struct sequence * sequence)
{
  free_rule (sequence->open);
  words_free (sequence->close);
  words_free (sequence->exceptions);
  free (sequence);
}

/*
 * Build the usual C's string and char with their escapes
 */
#define C_add_exception(_excep_)					\
  words_add_string (res,						\
	     rule_new (xustrdup (_excep_), NULL,			\
		       rhs_new_single (xustrdup (_excep_), 0, String_fface),\
                       __FILE__, __LINE__));

static struct words *
new_C_exceptions (void)
{
  struct words * res;

  res = words_new ("C usual exceptions: strings",
		   "C usual exceptions: regexps",
		   3, 3);
  C_add_exception ("\\\'");
  C_add_exception ("\\\\");
  C_add_exception ("\\\"");

  words_finalize (res);
  return res;
}

struct sequence *
new_C_string_sequence (const char * delimitor)
{
  struct sequence * res = XMALLOC (struct sequence, 1);
  res->open = rule_new (xustrdup (delimitor), NULL,
			rhs_new_single (NULL, 0, Plain_fface),
			__FILE__, __LINE__);
  res->face = String_fface;
  res->exceptions = new_C_exceptions ();
  res->close = words_new ("C Close: strings", "C Close: regex", 5, 5);
  words_add_string (res->close,
		    rule_new (xustrdup (delimitor), NULL,
			      rhs_new_single (NULL, 0, Plain_fface),
			      __FILE__, __LINE__));
  words_finalize (res->close);
  return res;
}

void
sequence_self_print (struct sequence * tmp, FILE * stream)
{
  rule_self_print (tmp->open, stream);
  fputs (" ->", stream);
  fface_self_print (tmp->face, stream);
  fputs (":\n", stream);
  words_self_print (tmp->exceptions, stream);
  words_self_print (tmp->close, stream);
}

/************************************************************************/
/*		Creating, loading a style sheet				*/
/************************************************************************/
/*
 * Dealing with the style sheets
 */
struct style_sheet *
new_style_sheet (const uchar * name)
{
  NEW (struct style_sheet, res);

  res->name = name;
  res->author = UNULL;
  version_set_to_null (res->version);
  res->documentation = UNULL;
  version_set_to_null (res->requirement);
  res->ancestors = ancestors_new ();
  res->sensitiveness = case_undefined;

  ALPHABET_UNDEFINE (res->alpha1);
  ALPHABET_UNDEFINE (res->alpha2);

  res->keywords =  words_new ("Keywords: Strings", "Keywords: Regexps",
			      100, 100);
  res->operators =  words_new ("Operators: Strings", "Operators: Regexps",
			       100, 100);

  res->sequences = da_new ("Sequences", 100,
			   da_linear, 100,
			   (da_print_func_t) sequence_self_print, NULL);

  return res;
}


/*---------------------------------------------------------.
| Build a style sheet which is only defined by ancestors.  |
`---------------------------------------------------------*/

static char *
style_sheet_mixed_new (const uchar * ancestors)
{
  struct style_sheet *sheet, *ancestor;
  char *ancestor_key, *key, *cp;
  char *ancestors_keys;
  struct darray *ancestors_array;

  /* It cannot be longer than ancestors_keys. */
  key = ALLOCA (char, ustrlen (ancestors) + 1);
  astrcpy (ancestors_keys, ancestors);

  /* Create the darray of ancestors keys, and build the final key. */
  ancestors_array = ancestors_new ();
  ancestor_key = strtok (ancestors_keys, MIXED_SHEET_SEP);
  da_append (ancestors_array, ancestor_key);
  ancestor = get_style_sheet (ancestor_key);
  cp = stpcpy (key, ancestor->key);

  while ((ancestor_key = strtok (NULL, MIXED_SHEET_SEP)))
    {
      da_append (ancestors_array, ancestor_key);
      ancestor = get_style_sheet (ancestor_key);
      *cp++ =',';
      cp = stpcpy (cp, ancestor->key);
    }
  *cp = '\0';
  key = xstrdup (key);

  /* Create the style sheet. */
  message (msg_sheet,
	   (stderr, "Creating a mixed style sheet \"%s\"\n", key));
  /* Its name is its key. */
  sheet = new_style_sheet ((uchar *) key);
  sheet->key = strdup (key);
  da_concat (sheet->ancestors, ancestors_array);
  da_erase (ancestors_array);
  style_sheet_finalize (sheet);

  return key;
}

/*-----------------------------------------------------------------.
| Once a style sheet has been read, make it usable (sorts etc.).   |
|                                                                  |
| NOTE: It must _not_ be called from the parser, since it may call |
| another time the parser, in case one of the ancestors are not    |
| known.                                                           |
`-----------------------------------------------------------------*/
void
style_sheet_finalize (struct style_sheet * sheet)
{
  message (msg_sheet,
	   (stderr, "Finalizing style sheet \"%s\" (%s)\n",
	    sheet->name, sheet->key));

  ancestors_finalize (sheet);

  /* Ensure a default alphabet */
  if (ALPHABET_IS_UNDEFINED (sheet->alpha1))
    string_to_array (sheet->alpha1, DEFAULT_ALPHA1);
  if (ALPHABET_IS_UNDEFINED (sheet->alpha2))
    string_to_array (sheet->alpha2, DEFAULT_ALPHA2);

  /* Ensure a default sensitivity */
   if (sheet->sensitiveness == case_undefined)
     sheet->sensitiveness = DEFAULT_SENSITIVENESS;

  words_finalize (sheet->keywords);
  words_finalize (sheet->operators);
  if (msg_test (msg_sheet))
    {
      fprintf (stderr, "---------- After Finalization of %s\n", sheet->key);
      style_sheet_self_print (sheet, stderr);

      message (msg_sheet,
	       (stderr, "Checking coherence of \"%s\" (%s)\n",
		sheet->name, sheet->key));
      if (style_sheet_check (sheet))
	message (msg_sheet, (stderr, "\"%s\" (%s) is sane.\n",
			     sheet->name, sheet->key));
      else
	message (msg_sheet, (stderr, "\"%s\" (%s) is corrupted.\n",
			     sheet->name, sheet->key));
      fprintf (stderr, "---------- End of Finalization of %s\n", sheet->key);
    }

  /* Put it in the sheets' table. */
  hash_insert (style_sheets, sheet);
}

/*
 * Loading a style sheet either looking for its path
 * thanks to its key, or directly from its path.
 */
static struct style_sheet *
load_style_sheet (const char * pseudo_key)
{
  struct style_sheet * res;

  /* Is the key a real key, or a path? */
  if (strsuffix (pseudo_key, SSH_SUFFIX))
    {
      /* This style sheet has to have its path as key.  This is used
	 to ease to allow specifying a style sheet by its real path,
	 instead of allowing key based file search.  It is also used
	 to to check that an ssh file is OK, from a2ps-mode in
	 emacs.*/
      res = parse_style_sheet (pseudo_key);
    }
  else
    {
      /* This is the regular process: find the file */
      char * path = pw_find_file (job->common.path, pseudo_key, SSH_SUFFIX);
      if (!path)
	{
	  char *file;
	  astrcat2 (file, pseudo_key, SSH_SUFFIX);
	  error (0, 0, _("cannot find file `%s'"), quotearg (file));
	  return NULL;
	}
      res = parse_style_sheet (path);
      free (path);
    }

  /* Now, finalize it, and store it in the hash tab */
  res->key = strdup (pseudo_key);
  style_sheet_finalize (res);

  return res;
}

/************************************************************************/
/*		Reporting information about the style sheets		*/
/************************************************************************/

/* Compare the names of two style sheets */

static int
sheet_name_cmp (const char * key1, const char * key2)
{
  return strcasecmp ((const char *) get_style_sheet (key1) -> name,
		     (const char *) get_style_sheet (key2) -> name);
}

/*
 * Report everything (used in -v5 at end of parsing)
 */
void
style_sheet_self_print (struct style_sheet * sheet, FILE * stream)
{
  fprintf (stream, "Style sheet \"%s\" (%s), version ",
	   sheet->name, sheet->key);
  version_self_print (sheet->version, stream);
  putc ('\n', stream);
  if (!IS_EMPTY (sheet->author))
    fprintf (stream, "Written by %s\n", sheet->author);
  if (sheet->ancestors->len > 0)
    {
      size_t i;
      fprintf (stream, "It inherits from: ");
      for (i = 0 ; i < sheet->ancestors->len ; i++)
	fprintf (stream, "%s%s",
		 i ? ", " : "",
		 (char *) sheet->ancestors->content [i]);
      fputs (".\n", stream);
    }

  fprintf (stream, "Case %ssensitive\n",
	   sheet->sensitiveness == case_insensitive ? "in" : "");

  /* Alphabets */
  fputs ("First alphabet is ", stream);
  alphabet_self_print (sheet->alpha1, stream);
  fputs ("Second alphabet is ", stream);
  alphabet_self_print (sheet->alpha2, stream);

  words_self_print (sheet->keywords, stream);
  words_self_print (sheet->operators, stream);
  if (!da_is_empty (sheet->sequences))
    da_self_print (sheet->sequences, stream);
}

/*
 * Print a short signature (i.e., name, author etc.)
 */
static void
style_sheet_print_signature (FILE * stream, struct style_sheet * sheet)
{
  int i, title_bar_len;

  if (!version_null_p (sheet->version))
    {
      title_bar_len = (strlen (" (.ssh version )")
		       + ustrlen (sheet->name)
		       + strlen (sheet->key)
		       + version_length (sheet->version));
      fprintf (stream, "%s (%s.ssh version ",
	       sheet->name, sheet->key);
      version_self_print (sheet->version, stream);
      fputs (")\n", stream);
    }
  else
    {
      title_bar_len = (strlen (" (.ssh)")
		       + ustrlen (sheet->name)
		       + strlen (sheet->key));
      fprintf (stream, "%s (%s.ssh)\n",
	       sheet->name, sheet->key);
    }
  for (i = 0 ; i < title_bar_len ; i++)
    putc ('-', stream);
  putc ('\n', stream);

  authors_print_plain (sheet->author, stream, "Written by ");

  documentation_print_plain (sheet->documentation, "%s\n", stream);
  fputs ("\n", stream);
}

/*
 * List the style sheet names (for option -E)
 */
void
list_style_sheets_short (FILE * stream)
{
  fputs (_("Known Style Sheets"), stream);
  putc ('\n', stream);
  pw_lister_on_suffix (stream, job->common.path, SSH_SUFFIX);
}

/*
 * List the style sheets
 */
void
list_style_sheets_long (FILE * stream)
{
  struct style_sheet * sheet;
  struct darray * entries;
  size_t i;

  entries = pw_glob_on_suffix (job->common.path, SSH_SUFFIX);

  /* Sort them by name (not key) */
  entries->cmp = (da_cmp_func_t) sheet_name_cmp;
  da_qsort (entries);

  title (stream, '=', true, _("Known Style Sheets"));
  putc ('\n', stream);

  for (i = 0 ; i < entries->len ; i++)
    {
      sheet = get_style_sheet ((char *) entries->content[i]);
      style_sheet_print_signature (stream, sheet);
    }
  da_free (entries, (da_map_func_t) free);
}

/************************************************************************/
/*		Report in HTML format					*/
/************************************************************************/
/*
 * Print a short signature (i.e., name, author etc.)
 * In html format
 */
static void
style_sheet_html_print_signature (FILE * stream, struct style_sheet * sheet)
{
  if (version_null_p (sheet->version))
    fprintf (stream,
	     "<li><p><strong><a href=\"%s.ssh\" name=\"%s\">%s</a></strong>.</p>\n",
	     sheet->key, sheet->key, sheet->name);
  else
    {
      fprintf (stream,
	       "<li><p><strong><a href=\"%s.ssh\" name=\"%s\">%s</a></strong> version ",
	       sheet->key, sheet->key, sheet->name);
      version_self_print (sheet->version, stream);
      fputs (".</p>\n", stream);
    }

  if (!IS_EMPTY(sheet->author)
      || !version_null_p(sheet->requirement)
      || (sheet->ancestors->len > 0))
    {
      fputs ("<p>", stream);
      authors_print_html (sheet->author, stream, "Written by ");
      if (!version_null_p (sheet->requirement))
	{
	  fputs ("It requires a2ps version ", stream);
	  version_self_print (sheet->requirement, stream);
	  fputs (".\n", stream);
	}
      if (sheet->ancestors->len > 0)
	{
	  size_t i;
	  fputs ("It inherits from: ", stream);
	  for (i = 0 ; i < sheet->ancestors->len ; i++)
	    fprintf (stream, "%s<a href=\"#%s\">%s.ssh</a>",
		     i ? ", " : "",
		     (char *) sheet->ancestors->content [i],
		     (char *) sheet->ancestors->content [i]);
	  fputs (".\n", stream);
	}
      fputs ("</p>", stream);
    }

  documentation_print_html (sheet->documentation, "<p>%s</p>\n", stream);
  fputs ("</li>\n\n", stream);
}

/*
 * List the style sheets in html format
 * I have a conscience problem wrt this.  This is made to ease
 * my task, but actually it has not much to do with a2ps itself :(
 */
void
list_style_sheets_html (FILE * stream)
{
  struct style_sheet * sheet;
  struct darray * entries;
  size_t i;
  version_t version_index, requirement;
  int rows;

  entries = pw_glob_on_suffix (job->common.path, SSH_SUFFIX);

  style_sheets_versions (entries, version_index, requirement);

  /* Sort them by name, not key. */
  entries->cmp = (da_cmp_func_t) sheet_name_cmp;
  da_qsort (entries);

  /*
   * The HTML header.  It should definitely be taken out of here.
   */
  fputs ((char *) expand_user_string (job,
				      CURRENT_FILE (job),
				      "sheets.html generation",
"<html>\n\
#{html.begin.hook}\n\
<head>\n\
   <title>#{html.title:-GNU a2ps language support}</title>\n\
</head>\n\
\n\
<h1 align=\\\"CENTER\\\">\n\
#{html.title.1:-GNU a2ps language support}\n\
</h1>\n\
#{html.hline:-<hline>}\n\
<h2>Current <a href=\"sheets.mp\">sheets.map</a></h2>\n\
<p>It is the list of rules to decide what style sheet to use.</p>\n"), stream);

  /*
   * The short list, presented as a table
   */
  fputs ((char *) expand_user_string (job,
				      CURRENT_FILE (job),
				      "sheets.html generation",
"\
#{html.hline:-<hline>}\n\
<h2>Sumary of current style sheets</h2>\n\
<p>There is a <a href=\"sheets.tar.gz\">package</a> which contains \
them all.  Its index (sum of the style sheets version number) is \
<strong>"), stream);
  version_self_print (version_index, stream);
  fputs ("</strong>, and the highest requirement is a2ps version ", stream);
  version_self_print (requirement, stream);
  fputs (".  Links point to their detailed exposure.</p>\n", stream);

  /* How many rows?  Let's make two columns */
  rows = entries->len / 3 + 1;

  fputs ("<table border=\"0\" cellpadding=\"0\"><tr>\n", stream);
  for (i = 0 ; i < entries->len ; /* Nothing */)
    {
      fputs ("<td valign=\"top\"><ul>\n", stream);
      do
	{
	  sheet = get_style_sheet ((char *) entries->content[i]);
	  fprintf (stream, "<li><a href=\"#%s\">%s <code>",
		   sheet->key, sheet->name);
	  version_self_print (sheet->version, stream);
	  fputs ("</a></code></li>\n", stream);
	}
      while ((++i % rows) && (i < entries->len));
      fputs ("</ul></td>\n", stream);
    }
  fputs ("</tr></table>\n", stream);

  /*
   * The real detail list
   */
  fputs ((char *) expand_user_string (job,
				      CURRENT_FILE (job),
				      "sheets.html generation",
"\
#{html.hline:-<hline>}\n\
<h2>Detailed list of current style sheets</h2>\n"), stream);
  fputs ("Links point to their files.</p>\n", stream);

  fputs ("<ul>\n", stream);
  for (i = 0 ; i < entries->len ; i++)
    {
      sheet = get_style_sheet ((char *) entries->content[i]);
      style_sheet_html_print_signature (stream, sheet);
    }
  fputs ("</ul>\n", stream);

  fputs ((char *) expand_user_string (job,
				      CURRENT_FILE (job),
				      "sheets.html generation",
"\
#{html.end.hook}\n\
</body>\n\
</html>\n"), stream);

  da_free (entries, (da_map_func_t) free);
}

/************************************************************************/
/*		Report in Texinfo format				*/
/************************************************************************/
/*
 * Print a short signature (i.e., name, author etc.)
 * In texinfo format
 */
static void
style_sheet_texinfo_print_signature (FILE * stream,
				     struct style_sheet * sheet)
{
  fprintf (stream,
	   "@deftp {Style Sheet} {%s} (@file{%s.ssh})\n",
	   sheet->name, sheet->key);

  /* Report details (version numbers, ancestors...) only if
   * verbose mode for sheets */
  if (!IS_EMPTY(sheet->author)
      || (msg_test (msg_sheet)
	  && (!version_null_p (sheet->requirement)
	      || (sheet->ancestors->len > 0))))
    {
      authors_print_texinfo (sheet->author, stream, "Written by ");
      if (msg_test (msg_sheet))
	{
	  if (!version_null_p (sheet->requirement))
	    {
	      fputs ("It requires @code{a2ps} version ", stream);
	      version_self_print (sheet->requirement, stream);
	      fputs (".  \n", stream);
	    }
	  if (sheet->ancestors->len > 0)
	    {
	      size_t i;
	      fputs ("It inherits from: ", stream);
	      for (i = 0 ; i < sheet->ancestors->len ; i++)
		fprintf (stream, "%s@file{%s.ssh}",
			 i ? ", " : "",
			 (char *) sheet->ancestors->content [i]);
	      fputs (".  \n", stream);
	    }
	}
    }

  documentation_print_texinfo (sheet->documentation, "%s\n", stream);

  fputs ("@end deftp\n\n", stream);
}

/*
 * List the style sheets in Texinfo format
 */
void
list_style_sheets_texinfo (FILE * stream)
{
  struct style_sheet * sheet;
  struct darray * entries;
  size_t i;
  version_t version_index, requirement;

  entries = pw_glob_on_suffix (job->common.path, SSH_SUFFIX);

  /* Sort them by name (not key) */
  entries->cmp = (da_cmp_func_t) sheet_name_cmp;
  da_qsort (entries);

  style_sheets_versions (entries, version_index, requirement);

  /* Report details (version numbers etc.) only if verbose
   * mode is set for sheets */
  if (msg_test (msg_sheet))
    {
      fputs ("\
The current index (sum of all the style sheets version number) is ", stream);
      version_self_print (version_index, stream);
      fputs (" and the highest requirement is a2ps version ", stream);
      version_self_print (requirement, stream);
      fputs (".\n", stream);

    }

  for (i = 0 ; i < entries->len ; i++) {
    sheet = get_style_sheet ((char *) entries->content[i]);
    style_sheet_texinfo_print_signature (stream, sheet);
  }

  da_free (entries, (da_map_func_t) free);
}

/************************************************************************/
/*		Perform some tests on the style sheets			*/
/************************************************************************/
/*
 * Check that a darray of rules is in lower case
 * (true for success)
 */
static bool
check_rules_lower_case (const char * name, struct darray * rules)
{
  size_t i;
  bool res = true;

  for (i = 0 ; i < rules->len ; i++)
    if (!is_strlower (((struct rule *) rules->content[i])->word)) {
      error (0, 0,
	     "%s.ssh:`%s' uses upper case characters",
	     name, ((struct rule *) rules->content[i])->word);
      res = false;
    }
  return res;
}

/*
 * Check that WORDS is all in lower case
 */
static bool
check_words_lower_case (const char * name, struct words * words)
{
  return (check_rules_lower_case (name, words->strings)
	  && check_rules_lower_case (name, words->regexps));
}

/*
 * Check that SEQUENCE is all in lower case
 */
static bool
check_sequence_lower_case (const char * name, struct sequence * sequence)
{
  bool res = true;

  if (!is_strlower (sequence->open->word)) {
    error (0, 0,
	   "%s.ssh:`%s' uses upper case characters",
	   name, sequence->open->word);
    res = false;
  }

  res &= check_words_lower_case (name, sequence->exceptions);
  res &= check_words_lower_case (name, sequence->close);

  return res;
}

static bool
check_sequences_lower_case (const char * name, struct darray * sequences)
{
  size_t i;
  bool res = true;

  for (i = 0 ; i < sequences->len ; i++)
    res &= check_sequence_lower_case (name,
				      (struct sequence *) sequences->content [i]);

  return res;
}

/*
 * Check that keywords are in lower case when lang is
 * case insensitive (returns 0 for failure)
 */
static bool
check_sensitivity (struct style_sheet * sheet)
{
  bool res = true;

  if (sheet->sensitiveness == case_sensitive)
    return true;

  /* The language is case insensitive: there should be only
   * lower case rules.
   * I know \B is legal in the regex.  But I prefer still
   * making the test: those who know how to use \B will have
   * no problems to understand that the test is wrong, not them. */
  res &= check_words_lower_case (sheet->key, sheet->keywords);
  res &= check_words_lower_case (sheet->key, sheet->operators);
  res &= check_sequences_lower_case (sheet->key, sheet->sequences);

  return res;
}


static bool
check_rules_doubles (const char * name, struct darray * rules)
{
  size_t i;
  bool res = true;

  for (i = 1 ; i < rules->len ; i++)
    if (!ustrcmp (((struct rule *) rules->content [i - 1])->word,
		  ((struct rule *) rules->content [i])->word))
      {
	res = false;
	error (0, 0, "%s.ssh:`%s' is defined twice",
	       name, ((struct rule *) rules->content [i])->word);
      }
  return res;
}


static bool
check_words_doubles (const char * name, struct words * words)
{
  return (check_rules_doubles (name, words->strings)
	  && check_rules_doubles (name, words->regexps));
}

/*
 * Check that there are no doublons in the style sheets
 */
static bool
check_doubles (struct style_sheet * sheet)
{
  bool res = true;

  res &= check_words_doubles (sheet->key, sheet->keywords);
  res &= check_words_doubles (sheet->key, sheet->operators);

  return res;
}

/*
 * Return a correctness result upon SHEET
 * (0: failure, 1: correct)
 */
static bool
style_sheet_check (struct style_sheet * sheet)
{
  return check_doubles (sheet) && check_sensitivity (sheet);
}

/************************************************************************/
/*			Handling of the hash table of			*/
/*				the style sheets			*/
/************************************************************************/
/*
 * We want to store the yet read style sheets in an hash table
 */
static unsigned long
sheet_hash_1 (void const *key)
{
  return_STRING_HASH_1 (((const struct style_sheet *)key)->key);
}

static unsigned long
sheet_hash_2 (void const *key)
{
  return_STRING_HASH_2 (((const struct style_sheet *)key)->key);
}

static int
sheet_hash_cmp (void const *x, void const *y)
{
  return_STRING_COMPARE (((const struct style_sheet *)x)->key,
			 ((const struct style_sheet *)y)->key);
}

/*
 * Create the hash table
 */
struct hash_table_s *
new_style_sheets (void)
{
  struct hash_table_s * res;
  res = XMALLOC (hash_table, 1);
  hash_init (res, 8,
             sheet_hash_1, sheet_hash_2, sheet_hash_cmp);
  return res;
}

/*
 * Make sure the style sheet SHEET is in the hash table
 */
static void
require_style_sheet (const char * key)
{
  struct style_sheet * item;
  struct style_sheet token;

  if (strequ (key, "plain"))
    return;

  token.key = (char *) key;
  item = (struct style_sheet *) hash_find_item (style_sheets, &token);

  if (item == NULL)
    {
      /* The style has never been seen before, then load it through a
	 correct call to yyparse, which in turn is in charge of
	 putting the newly read style in the hash table.  Hence, look
	 for it again.  An error at this point is really an error. */
      item = load_style_sheet (key);

      if (item == NULL)
	error (0, 0,
	       _("cannot find style sheet `%s': using plain style"), key);
    }

  return;
}

/*
 * return the style sheet bearing this name
 * autoload it if not yet read.
 * If the sheet cannot be found, return NULL
 */
struct style_sheet *
get_style_sheet (const char * key)
{
  struct style_sheet * item;
  struct style_sheet token;

  if (strequ (key, "plain"))
    return NULL;

  /* There is some specials magic to build on the fly mixed style
     sheets, for instance `-Email,sh' build a new style sheet which is
     only defined by:
     ancestors are
        mail, sh
     end ancestors.
     In this case the key become `mail,sh'.
  */
  if (strpbrk (key, MIXED_SHEET_SEP))
    key = style_sheet_mixed_new (key);
  else
    /* Make sure the hash table contains the corresponding style sheet */
    require_style_sheet (key);

  token.key = (char *) key;
  item = (struct style_sheet *) hash_find_item (style_sheets, &token);

  if (!item)
    return NULL;

  /* To me, it looks like an horror: the syntax table is not
   * stored in the regex buffer.  As a consequence, each time
   * we change sheet, make sure to update the re_syntax_table */
  re_syntax_table = (char *) item->alpha1;

  return item;
}

/*
 * Compute the total SUM of the all the versions of the
 * sheets which key is in KEYS
 */
static inline void
style_sheets_versions (struct darray * keys,
		       int sum [4], int requirement [4])
{
  size_t i;
  struct style_sheet * sheet;

  version_set_to_null (sum);
  version_set_to_null (requirement);
  for (i = 0 ; i < keys->len ; i++)
    {
      sheet = get_style_sheet ((char *) keys->content[i]);
      version_add (sum, sheet->version);
      if (version_cmp (sheet->requirement, requirement) > 0)
	version_cpy (requirement, sheet->requirement);
    }
}
