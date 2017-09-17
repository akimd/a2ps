/* stream.c - open streams on pipes or files
   Copyright 1988-2017 Free Software Foundation, Inc.

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

#include <config.h>

#include "a2ps.h"
#include "stream.h"
#include "routines.h"
#include "message.h"
#include "quotearg.h"
#include <assert.h>

/* Open for reading. */

static inline struct stream *
_stream_ropen (const char * command, bool is_file)
{
  struct stream * res = XMALLOC (struct stream);

  res->is_file = is_file;
  if (!res->is_file)
    res->fp = xrpopen (command);
  else
    {
      if (!IS_EMPTY (command))
	res->fp = xrfopen (command);
      else
	res->fp  = stdin;
    }
  return res;
}

/*
 * Inline wrapper
 */
struct stream *
stream_ropen (const char * command, bool is_file)
{
  return _stream_ropen (command, is_file);
}

/*
 * Open for writing
 */
static inline struct stream *
_stream_wopen (const char * command, bool is_file,
	       enum backup_type backup_type)
{
  struct stream * res = XMALLOC (struct stream);

  res->is_file = is_file;
  if (!res->is_file)
    res->fp = xwpopen (command);
  else
    {
      if (!IS_EMPTY (command))
	res->fp = fopen_backup (command, backup_type);
      else
	res->fp = stdout;
    }
  return res;
}

/*
 * Inline wrapper
 */
struct stream *
stream_wopen (const char * command, bool is_file)
{
  return _stream_wopen (command, is_file, no_backups);
}

struct stream *
stream_wopen_backup (const char * command, bool is_file,
		     enum backup_type backup_type)
{
  return _stream_wopen (command, is_file, backup_type);
}


/* Open a r or w stream depending in PERL_COMMAND.  PERL_COMMAND can
   be:
   - `> file', open a w-stream on FILE, and backup is asked.
   - `| cmd', open a w-pipe on CMD.
   - `cmd |', open a r-pipe on CMD.
   - otherwise, PERL_COMMAND is a file to read.

   Once the decoding done, NAME points to the first char of the file
   name.
*/

struct stream *
stream_perl_open_backup (const char * perl_command,
			 enum backup_type backup,
			 const char **name)
{
  char * cp;
  int len;

  assert (perl_command);
  message (msg_file,
	   (stderr, "perl-open (%s)\n", quotearg (perl_command)));

  *name = perl_command + strspn (perl_command, "\t >|");

  switch (*perl_command)
    {
    case '|':
      return _stream_wopen (*name, false, no_backups);

    case '>':
      return _stream_wopen (*name, true, backup);

    default:
      /* Open for reading. */
      len = strlen (perl_command);
      switch (perl_command [len - 1])
	{
	case '|':
	  /* Read a pipe. */
	  cp = ALLOCA (char, len);
	  strncpy (cp, *name, len - 1);
	  return _stream_ropen (cp, false);

	default:
	  /* Read a file. */
	  return _stream_ropen (*name, true);
	}
    }
}

/*
 * Closes and frees.
 */
void
stream_close (struct stream * stream)
{
  if (stream->is_file)
    fclose (stream->fp);
  else
    pclose (stream->fp);
  free (stream);
}
