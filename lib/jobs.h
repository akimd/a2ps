/* jobs.h - recording information about the print jobs
   Copyright 1995-2017 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA
   02110-1301, USA.  */

#ifndef _JOBS_H_
#define _JOBS_H_

#include <time.h>

#include "encoding.h"
#include "output.h"
#include "confg.h"
#include "media.h"
#include "darray.h"
#include "gen.h"
#include "psstat.h"
#include "caret.h"
#include "common.h"
#include "printers.h"
#include "backupfile.h"
#include "madir.h"
#include "filalign.h"

struct stream;

/* All the information on the current print job */
typedef struct a2ps_job
{
  char ** argv;			/* The command line			*/
  size_t argc;

  struct a2ps_common_s common;	/* Common data to several modules */

  char * file_command;		/* Command to use to call file(1)	*/

  /* Global information that the library needs */
  hash_table * media;		/* Definition of the media		*/
  struct pair_htable * user_options; /* Short cuts defined by the user	*/

  /* Relative to the whole process */
  struct tm run_tm; 		/* Time when this program is run	*/
  enum backup_type backup_type;	/* When to backup output files		*/
  int sheets;			/* BEWARE: current PAGE number		*/
  int pages;			/* BEWARE: current VIRTUAL number	*/
  int lines_folded;             /* Number of lines that were folded     */
  int total_files;		/* id but files				*/
  ORIENTATION orientation;	/* landscape mode ?			*/
  enum duplex_e duplex;		/* Recto verso printing ? 		*/
  int columns;			/* number of columns/page		*/
  int rows;			/* number of rows/page			*/
  madir_t madir;		/* firt rows, or columns?
				 * (not major, because of a SunOS macro)*/
  int virtual;			/* number of the virtual page on that sheet */
  int copies;			/* number of copies			*/
  int margin;			/* margin to leave for binding		*/

  struct pair_htable * encodings_map;/* Content of the encoding.map file*/
  struct output * ps_encodings; /* PS definition of the encodings used	*/
  int page_prefeed;		/* Page prefeed mecanism		*/
  /* How to get the physical font (eg Courier), from the logical
   * (eg, Keyword) */
  char * face_eo_font [NB_FACES];
  unsigned char * stdin_filename;	/* Filename given to stdin		*/

  /*
   * Output related
   */
  struct a2ps_printers_s * printers;/* The printers module		*/
  enum output_format output_format; /* ps, eps etc.			*/
  struct stream * output_stream;/* Where the result will be dumped	*/

  bool folding;		/* Line folding option 			*/
  int numbering;		/* Line numbering option 		*/
  /* Replace non printable char */
  enum unprintable_format unprintable_format;
  int interpret;		/* Interpret TAB, FF and BS chars option */
  int print_binaries;		/* Force printing for binary files 	*/
  file_align_t file_align;	/* Policy of sharing medium b/t files.	*/
  int border;			/* print the surrounding border ?	*/
  bool debug;			/* Are we in debug mode?		*/
  char * prolog;		/* postscript header file 		*/
  struct medium * medium;	/* Medium to use 			*/
  char *medium_request;		/* The name of the medium choosen	*/
  int tabsize;			/* length of tabulations		*/
  int lines_requested;		/* Number of line per page 		*/
  int columns_requested;	/* Number of columns per page		*/
  float fontsize;		/* Size of a char for body font 	*/
  struct encoding * encoding;/* What is the current char set ?	*/
  char * requested_encoding_name;/* Because encoding.map has not been
				  * read when reading the conf files,
				  * we cannot store the result of -X<enc>
				  * as an encoding */
  struct encoding * requested_encoding;/* The main encoding (given by option)	*/
  struct encoding * saved_encoding; 	/* Given by the options			*/
  struct hash_table_s * encodings;/* The hash table of encodings yet read */

  struct pair_htable * fonts_map;/* Aliases (to cope with MS_DOS) */
  struct hash_table_s * font_infos;	/* The HT of the afm for each file */

  /* Headers and footers */
  unsigned char * title;		/* Job's title (eg. in %%Title: )	*/
  unsigned char * header;		/* Allow different header text	 	*/
  unsigned char * center_title;		/* Allow different header text	 	*/
  unsigned char * left_title;		/* Allow different header text	 	*/
  unsigned char * right_title;		/* Allow different header text	 	*/
  unsigned char * left_footer;		/* Allow different header text	 	*/
  unsigned char * footer;		/* Allow different header text	 	*/
  unsigned char * right_footer;		/* Allow different footer text	 	*/
  unsigned char * water;		/* Water marks text			*/

  /* Used to grab headers etc. from the file */
  unsigned char tag1[256], tag2[256], tag3[256], tag4[256];

  /* Definition of the macro meta sequences	*/
  struct pair_htable * macro_meta_sequences;

  /* Private information on the PS generation engine */
  struct ps_status * status;

  /* Where the output is diverted */
  struct output * divertion;

  /* 10 temporary files */
  char * tmp_filenames [10];

  /* Interval of pages to print */
  struct page_range * page_range;

  /* List of the jobs */
  struct darray * jobs;
} a2ps_job;

/* liba2ps.h:begin */
/* Return a newly allocated output session storage */
struct a2ps_job * a2ps_job_new (void);

/* Finalize it */
void a2ps_job_finalize (struct a2ps_job * job);

/* Free the memory used by JOB */
void a2ps_job_free (struct a2ps_job * job);
/* liba2ps.h:end */

#define CURRENT_FILE(j)				\
  ((struct file_job *) (j->jobs->content[j->jobs->len - 1]))
#define FIRST_FILE(j)				\
  ((struct file_job *) (j->jobs->content[0]))

/* Unlink everything that could have been used */
void a2ps_job_unlink_tmpfiles (struct a2ps_job * job);

#endif
