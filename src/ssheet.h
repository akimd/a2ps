/*
 * ssheet.h
 *
 * definition of the languages style sheets
 * Copyright (c) 1988, 89, 90, 91, 92, 93 Miguel Santana
 * Copyright (c) 1995, 96, 97, 98 Akim Demaille, Miguel Santana
 * $Id: ssheet.h,v 1.1.1.1.2.1 2007/12/29 01:58:36 mhatta Exp $
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

#ifndef _SSHEET_H_
#define _SSHEET_H_

#include "a2ps.h"
#include "darray.h"
#include "ffaces.h"
#include "hashtab.h"
#include "jobs.h"
#include "regex.h"

enum case_sensitiveness
{
  case_sensitive, case_insensitive, case_undefined
};

/*
 * Used by the scanner and parser to return a pattern,
 * which may enclose NUL
 */
struct pattern
{
  char * pattern;
  size_t len;
};

/*
 * Now, keywords, and operators share the same underlying
 * structure, because, though keywords don't need a SYMBOL field,
 * they are so close, that implementation should be the same.
 * This may be a sign that OO would have been appreciated :)
 */
struct faced_string
{
  uchar * string;		/* the destination string	*/
  int reg_ref;			/* there is no destination string,
				 * but the model is the REG_REF expression
				 * caught by the regexp (e.g. 1 for \1 */
  struct fface_s face;
};


/*--------------------------------------.
| A rule is composed of its lhs and rhs |
`--------------------------------------*/

struct rule
{
  uchar *word;
  struct re_pattern_buffer *regex;
  struct darray * rhs;
};

struct words
{
  /* darrays of struct rule * */
  struct darray * strings;	/* those which matchers are strings */
  struct darray * regexps;	/* those which matchers are regexps */
  struct rule ** min [256];
  struct rule ** max [256];
};

struct sequence
{
  struct rule * open;
  struct fface_s face;
  struct words * exceptions;		/* Exception, i.e. \" between ""  */
  struct words * close;			/* closing alternatives */
} ;

struct style_sheet
{
  /* index of the language (compare with command-line option) */
  const char * key;

  /* Nice looking name of the style */
  const uchar * name;

  /* Who wrote it, and when */
  const uchar * author;
  int version[4];

  /* What version of a2ps is required? */
  int requirement[4];

  /* Note describing the mode or the language */
  const uchar * documentation;

  /* Does it have ancestors (i.e., this one is an extension of
   * its ancestors).  It is a list of keys, of course */
  struct darray * ancestors;

  /* case sensitiveness for keywords and rules */
  enum case_sensitiveness sensitiveness;

  /* definition of the "words" (keywords and rules):
   * a char belonging to a first alphabet (alpha1),
   * and any number of chars belonging to the second (alpha2). */
  uchar alpha1 [256];
  uchar alpha2 [256];

  /* list of keywords for this language */
  struct words * keywords;

  /* same as keywords BUT there is no need to be preceded and followed
   * by not in alpha2. In other words, these are not "words" of the
   * alphabet, but any sequence of chars */
  struct words * operators;

  /* darray of the sequences (strings, documentations, etc.) */
  struct darray * sequences;
};

/*	Notes
 *
 * - global exceptions is made for languages such as tcl where, whereever the
 * rule appears, it must not be "understood", e.g. \" and \\ are
 * written directly.
 *
 * - local exception is made for аз§и! languages such as ada where "" is the
 * empty string, but appearing in a string, "" denotes ".
 */


/*
 * The faced_string
 */
struct faced_string *
faced_string_new PARAMS ((uchar * string, int reg_ref, struct fface_s fface));

/*
 * The version numbers
 */
void style_sheet_set_version PARAMS ((struct style_sheet * sheet,
				      const char * version_string));
int style_sheet_set_requirement PARAMS ((struct style_sheet * sheet,
					 const char * version_string));

/*
 * The destinations (part of rule)
 */
struct darray * rhs_new PARAMS ((void));
struct darray *
rhs_new_single PARAMS ((uchar * string, int reg_ref, struct fface_s fface));
void rhs_add PARAMS ((struct darray * dest, struct faced_string * str));
void rhs_self_print PARAMS ((struct darray * rhs, FILE * stream));

/*
 * Dealing with the keywords, rules and operators
 */
struct rule * rule_new PARAMS ((uchar * word,
				struct pattern * pattern,
				struct darray * destination,
				const char *filename, size_t line));
struct rule * keyword_rule_new PARAMS ((uchar * word,
					struct pattern * pattern,
					struct darray * destination,
					const char *filename, size_t line));

/*
 * Dealing with the sequences
 */
struct pattern *
new_pattern PARAMS ((char * pattern, size_t len));

struct sequence * sequence_new
   	PARAMS ((struct rule * Open,
		 struct fface_s in_face,
		 struct words * Close,
		 struct words * exceptions));
void free_sequence PARAMS ((struct sequence * sequence));
struct sequence * new_C_string_sequence PARAMS ((const char * delimitor));
void sequence_self_print PARAMS ((struct sequence * v, FILE * stream));


/*
 * Dealing with the struct words
 */
struct words *
words_new PARAMS ((const char * name_strings, const char * name_regexps,
		   size_t size, size_t increment));
void words_free PARAMS ((struct words * words));
void words_add_string PARAMS ((struct words * words, struct rule * rule));
void words_add_regex PARAMS ((struct words * words, struct rule * rule));
void words_set_no_face PARAMS ((struct words * words, struct fface_s face));
void words_merge_rules_unique PARAMS ((struct words * words,
					 struct words * new));

/*
 * Dealing with the style sheets
 */
struct style_sheet * new_style_sheet PARAMS ((const uchar * name));
void style_sheet_finalize PARAMS ((struct style_sheet * sheet));
void style_sheet_self_print PARAMS ((struct style_sheet * sheet,
				     FILE * stream));

/*
 * Dealing with the hash table for style sheets
 */
struct hash_table_s * new_style_sheets PARAMS ((void));
struct style_sheet * get_style_sheet PARAMS ((const char * name));

/************************************************************************/
/*				style selection				*/
/************************************************************************/
/*
 * List the style sheets
 */
void list_style_sheets_short PARAMS ((FILE * stream));
void list_style_sheets_long PARAMS ((FILE * strea));
void list_style_sheets_html PARAMS ((FILE * strea));
void list_style_sheets_texinfo PARAMS ((FILE * strea));

#endif /* not defined _SSHEET_H_ */
