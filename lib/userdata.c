/*
 * userdata.c -- Get data on users (from /etc/passwd).
 * Copyright (c) 1999 Akim Demaille, Miguel Santana
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

#include "system.h"
#include "xalloc.h"
#include "userdata.h"

char *stpcpy PARAMS ((char *dest, const char *src));

#if !HAVE_PWD_H
struct passwd
{
  const char *pw_name;
  const char *pw_gecos;
  const char *pw_dir;
};
#endif

/* Free the memory hold by UDATA, but not UDATA itself. */
void
userdata_free (struct userdata *udata)
{
  XFREE (udata->login);
  XFREE (udata->name);
  XFREE (udata->comments);
  XFREE (udata->home);
}

void
userdata_get (struct userdata *udata)
{
  struct passwd *passwd = NULL;
  const char *home, *login;
  char *comments = NULL, *name = NULL, *cp;

#if HAVE_GETPWUID
  passwd = getpwuid (getuid ());
#endif

  /* Home dir. */
  if ((cp = getenv ("HOME")))
    home = cp;
  else if (passwd && passwd->pw_dir)
    home = passwd->pw_dir;
  else
    home = "/home";

  /* Login. */
  if ((cp = getenv ("LOGNAME")))
    login = cp;
  else if ((cp = getenv ("USERNAME")))
    login = cp;
  else if (passwd && passwd->pw_name)
    login = passwd->pw_name;
  else
    login = NULL;

  /* The field `pw_gecos' contains the full name and comments, such as
     phone number etc. */
#ifdef HAVE_STRUCT_PASSWD_PW_GECOS
  if (passwd && passwd->pw_gecos)
    {
      char *gecos = NULL;
      if (login)
	{
	  /* Occurences of `&' in the gecos should be substituted with
	     the login (first letter in uppercase). */
	  size_t amps = 0;	/* Number of ampersands. */
	  char *cp2;

	  for (cp = passwd->pw_gecos; *cp; cp++)
	    if (*cp == '&')
	      amps++;
	  gecos = ALLOCA (char,
			  (strlen (passwd->pw_gecos)
			   + amps * (strlen (login) - 1) + 1));

	  cp2 = gecos;

	  for (cp = passwd->pw_gecos; *cp; cp++)
	    if (*cp == '&')
	      {
		*cp2++ = toupper (*login);
		cp2 = stpcpy (cp2, login + 1);
	      }
	    else
	      *cp2++ = *cp;
	  *cp2 = '\0';
	}
      else
	{
	  gecos = ALLOCA (char, strlen (passwd->pw_gecos) + 1);
	  stpcpy (gecos, passwd->pw_gecos);
	}

      /* The gecos may have subfields seperated by `,'.  Consider the
	 first field to be the user name, and the rest to be
	 comments. */
      name = gecos;
      if ((comments = strchr (gecos, ',')))
	{
	  *comments = '\0';
	  comments++;
	}
    }
#endif

  udata->login = xstrdup (login ? login : _("user"));
  udata->name = xstrdup (name ? name : _("Unknown User"));
  udata->comments = comments ? xstrdup (comments) : NULL;
  udata->home = xstrdup (home ? home : "/home");
}
