/* userdata.c - get data on users (from /etc/passwd)
   Copyright 1999-2017 Free Software Foundation, Inc.

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

#include <string.h>
#include <pwd.h>

#include "system.h"
#include "xalloc.h"
#include "userdata.h"
#include "routines.h"

/* Free the memory hold by UDATA, but not UDATA itself. */
void
userdata_free (struct userdata *udata)
{
  free (udata->login);
  free (udata->name);
  free (udata->comments);
  free (udata->home);
}

void
userdata_get (struct userdata *udata)
{
  struct passwd *passwd = NULL;
  const char *home, *login;
  char *comments = NULL, *name = NULL, *cp;

  passwd = getpwuid (getuid ());

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

  udata->login = xstrdup (login ? login : _("user"));
  udata->name = xstrdup (name ? name : _("Unknown User"));
  udata->comments = comments ? xstrdup (comments) : NULL;
  udata->home = xstrdup (home ? home : "/home");
}
