#! /usr/local/bin/perl -w

# Produce the list of the translators for the Texinfo documentation

local ($mail, $name, $lang, $comment);

$translator_file = "translators.txt";

# List of the people who helped the translation
open (STDIN, $translator_file)
  || die "Could not open `$translator_file': $1\n";

while (<>)
  {
    chop ;
    ($mail, $name, $lang, $comment) = split ('	');

    print "\@item\n";
    if ($mail)
      {
	$mail =~ s/@/@@/g;
	print "\@email{$mail, $name} ";
      }
    else
      {
	print "$name ";
      }
    print "($lang) ";
    if ($comment)
      {
	print "$comment";
      }
    print "\n" x 2;
  }

