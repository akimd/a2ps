/* atexit.c -- Do nothing, but to return a success

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3, or (at your option)
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software Foundation,
    Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#ifdef HAVE_ON_EXIT
/* The function used by on_exit has two args:
     The routine named is called as
          (*procp)(status, arg);
     where status is the argument with which exit()  was  called,
     or  zero  if main returns.  Typically, arg is the address of
     an argument vector to (*procp), but may be an integer value.

  Use this space to store the address to call the atexit'ed function,
  so that there are no mismatches between arities of call procedures.     
*/

static void
on_exit_wrapper (status, func)
  int status;
  void (* func) ();
{
  func ();
}

int
atexit (func)
    void (*func) ();
{
  return on_exit (on_exit_wrapper, func);
}

#else /* !defined(HAVE_ON_EXIT) */
/* Define the registering function
 * and a replacement exiting function */

#define EXIT_STACK_SIZE 32

typedef void (*exit_fn_t) ();
static exit_fn_t exit_stack[EXIT_STACK_SIZE];
static int exit_stack_len = 0;

int
atexit (func)
  exit_fn_t func;
{
  if (exit_stack_len >= EXIT_STACK_SIZE)
    return 1;

  exit_stack [exit_stack_len ++] = func;
  return 0;
}

void
exit (status)
   int status;
{
  while (exit_stack_len > 0)
    exit_stack [--exit_stack_len] ();
#undef exit
  exit (status);
}

#endif
