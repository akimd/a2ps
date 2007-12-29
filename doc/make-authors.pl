#! /usr/local/bin/perl -w

# Produce the list of the translators for the AUTHORS file.

local ($mail, $name, $lang, $comment, %authors);

sub title
  {
    $line = shift;
    $line =~ s/(.)/$1\b$1/g;
    print "$line\n";
  }

$translator_file = "translators.txt";

# List of the people who wrote the package
title "Authors of this package.";
print <<'EOC';
 * Miguel Santana <Miguel.Santana@st.com>
 * Akim Demaille <demaille@inf.enst.fr>
EOC

print "\n";

# List of the people who maintain the package
title "Interim Maintainer";
print <<'EOC';
 * Masayuki Hatta <mhatta@gnu.org>
EOC

print "\n";

# List of the people who helped the translation
title "Translators";
open (STDIN, $translator_file)
  || die "Could not open `$translator_file': $1\n";

while (<>)
  {
    chop ;
    ($mail, $name, $lang, $comment) = split ('	');
    if ($mail)
      {
	$mail = ' <' . $mail . '>';
      }
    else
      {
	$mail = "";
      }
    print " * $name ($lang)$mail\n";
  }

print "\n";

# List of the people who wrote style sheets
title "Style Sheets Authors";
open (STDIN, "grep 'written by' ../sheets/*.ssh|");

while (<>)
  {
    chop;
    ($file, $names) = split (':');
    $file =~ s|^.*/||;
    $file =~ s/\.ssh//;
    $names =~ s/.*"(.*)".*/$1/g;

    foreach $name (split (',', $names))
      {
	# We don't want to see demailles
	next
	  if ($name =~ /Akim Demaille/);
	# Avoid extraneous white spaces
	$name =~ s/[ \t]+/ /g;
	$name =~ s/^ //;
	$name =~ s/ $//;
	$authors{$name} .= ($authors{$name} ? ', ' : '') . $file;
      }
  }

foreach $name (sort (keys (%authors)))
  {
    $sheets = $authors{$name};
    if ($name =~ /(<[^>@]+@[^>]+>)/)
      {
	$mail = $1;
	$name =~ s/$mail//;
	# Avoid extraneous white spaces
	$name =~ s/[ \t]+/ /g;
	$name =~ s/^ //;
	$name =~ s/ $//;
	$mail = " $mail";
      }
    else
      {
	$mail = "";
      }
    print " * $name ($sheets)$mail\n";
  }

print "\n";

# People who package the package
title "Packagers / Ports";
print <<'EOC';
 * Alexander Mai (OS/2 port) <mai@migdal.ikp.physik.tu-darmstadt.de>
 * Ansgar Duelmer (DOS) <Ansgar.Duelmer@anorg.chemie.uni-giessen.de>
 * Chuck Robey (FreeBSD) <chuckr@mat.net>
 * Dirk Eddelbuettel (formerly Debian) <edd@debian.org>
 * Masayuki Hatta (Debian) <mhatta@debian.org>
 * Michael Williams (Mac OS X) <mike@dreamboatrecords.co.uk>
 * Paulo Matos (RPM) <pjsm@students.fct.unl.pt>
EOC

print "\n";

title "Various";
print <<'EOC';
 * Bruce Ingalls (a2ps-print.el) <bingalls@iconnet.com>
EOC
