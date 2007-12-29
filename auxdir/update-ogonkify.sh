#! /bin/sh

if test $# != 2; then
  echo "Usage: $0 SOURCE DESTINATION"
  echo "Update the directory DESTINATION with the content of SOURCE."
  echo "SOURCE is destroyed."
  exit 1
fi

srcdir=$1
dstdir=$2

if test ! -d $srcdir || test ! -d $dstdir; then
  "$srcdir or $dstdir does not exist"
  exit 1
fi

# Some of the files should just be ignored
(
  cd $srcdir
  rm INSTALL clean config instogonki distribution test.html test makecomp
  cd doc
  rm clean makedoc ogonkify.man ogonkify.info
)

# Update all the AFM files
for i in `cd $srcdir && echo *.afm`
do
  if test ! -r $dstdir/$i; then
    echo "The file $i is new"
  fi
  mv $srcdir/$i $dstdir/$i
done

# Install auxiliary files
for i in compose.ps allchars.ps fontname.dat helper.ps README VERSION
do
  if test ! -r $dstdir/$i; then
    echo "The file $i is new"
  fi
  mv $srcdir/$i $dstdir/$i
done

# composeglyphs needs PERL
cat $srcdir/composeglyphs |
 sed -e 's|/usr/local/bin/perl|@PERL@|' > $dstdir/composeglyphs.in
rm $srcdir/composeglyphs

# Juliusz does not seem to want to change IBM fonts access in ogonkify
cat $srcdir/ogonkify.in |
 sed -e 's|$IBMFONTDIR="@IBMFONTDIR@"|$IBMFONTS="@IBMFONTS@"|' \
     -e 's|<$IBMFONTDIR/@IBMFONTS@>|<$IBMFONTS>|' > $dstdir/ogonkify.in.in
rm $srcdir/ogonkify.in

# The documentation
for i in `cd $srcdir && echo doc/*`
do
  if test ! -r $dstdir/$i; then
    echo "The file $i is new"
  fi
  mv $srcdir/$i $dstdir/$i
done

# At this point $srcdir should be empty.
rmdir $srcdir/doc
rmdir $srcdir

echo "$dstdir updated."
exit 0
