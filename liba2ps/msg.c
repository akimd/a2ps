/* msg.c - declaration for verbosity sensitive feedback function
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

/* This file should not be compiled, but included by message.c. */


/* Definition of the arguments/values for argmatching the verbosity
   level. */

#include <config.h>

const char *const _msg_verbosity_args[] =
{
  "sheets",
  "pathwalk", "pw",
  "encodings",
  "configuration", "options",
  "files",
  "tools",
  "fonts",
  "ppd",
  "variables", "meta-sequences",
  "parsers",
  "all",
  0
};

const int _msg_verbosity_types[] =
{
  msg_sheet,
  msg_pw, msg_pw,
  msg_enc,
  msg_opt, msg_opt,
  msg_file,
  msg_tool,
  msg_font,
  msg_ppd,
  msg_meta, msg_meta,
  msg_parse,
  msg_all
};
