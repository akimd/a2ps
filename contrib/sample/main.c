/* This is just to know if your system support stdlib.h */
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#ifdef HAVE_STDLIB_H
#  include <stdlib.h>
#endif
#if HAVE_STDBOOL_H
# include <stdbool.h>
#else
typedef enum {false = 0, true = 1} bool;
#endif

#include "liba2ps.h"

#define _(V) sprintf(buf, "%.4d",V),a2ps_print_string(job,buf, String)
int a=10000, b, c=8400, d, e, f[8401], g; char buf[10];
void
foo (struct a2ps_job * job)
{ for (;b-c;) f[b++]=a/5; for (;d=0,g=c*2;c-=14,_(e+d/a), e=d%a) for
(b=c;d+=f[b]*a,f[b]=d%--g, d/=g--,--b;d*=b); }


/* It is mandatory to declare and set the value of this variable.
 * It is used when printing error messages. */
char *program_name = "Sample";
char *program_invocation_name = "Sample";

int
main (void)
{
  enum face_e i;
  int j;
  struct a2ps_job * job;

  /* Create the structure, and fill it according the master
   * system configuration file (typically /etc/a2ps.cfg) */
  job = a2ps_job_new ();
  a2_read_sys_config (job);

  /* Once the configuration files are read, finalize the structure */
  a2ps_job_finalize (job);

  /* Open an output session */
  a2ps_open_output_session (job);

  /* We open a new input session, which title is Sample Input */
  a2ps_open_input_session (job, "Sample Input");

  for (j = 0 ; j < 5 ; j++)
    for (i = First_face ; i <= Last_face ; i++)
      a2ps_print_string (job, "Hello world\n", i);

  a2ps_print_string (job, "This is an Error", Error);
  a2ps_close_input_session (job);

  /* Just another page, with nothing really important in it */
  a2ps_open_input_session (job, "Surprise!");
  a2ps_print_char (job, '\160', Symbol);
  a2ps_print_string (job, " = ", Plain);
  foo (job);
  a2ps_close_input_session (job);

  /* This send the output to the current default printer */
  a2ps_close_output_session (job);

  /* Release the memory used */
  a2ps_job_free (job);

  exit (0);
}
