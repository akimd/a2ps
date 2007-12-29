/*
 * stream.c
 *
 * Uniform access to pipe and files
 *
 * Copyright (c) 1988-1993 Miguel Santana
 * Copyright (c) 1995-1999 Akim Demaille, Miguel Santana
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

/*
 * $Id: stream.c,v 1.1.1.1.2.1 2007/12/29 01:58:24 mhatta Exp $
 */

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
  NEW (struct stream, res);

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
  NEW (struct stream, res);

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
  return _stream_wopen (command, is_file, none);
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
      return _stream_wopen (*name, false, none);

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
