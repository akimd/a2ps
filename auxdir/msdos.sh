#! /bin/sh

#
# Take a configure'd package.tar.gz, and make it better for msdos.
#
test $# = 1 || exit 1

thisdir=`pwd`
tarball=$thisdir/$1
package=`echo $1 | sed 's/\.tar\.gz//g;s#.*/##g'`
msdos=`echo $package | sed -e 's/[-.]//g'`

cd /tmp
rm -rf $package
gunzip -cd $tarball | tar xvf - || exit 1
cd /tmp/$package || exit 1


#################### We are in the package.  Patch what we need to patch
# 1. config.h.in is two dots long
perl -pi -e 's/config\.h/config.h:config.hin/g' configure.in || exit 1
mv config.h.in config.hin

# 2. po/Makefile.in.in has 2 dots
sed -e 's/Makefile\.in\.in/Makefile.in2/g' \
    -e 's|$(subdir)/$@.in|$(subdir)/$@.in:$(subdir)/$@.in2|g' \
    po/Makefile.in.in > po/Makefile.in2
perl -pi -e 's#po/Makefile\.in#po/Makefile.in:po/Makefile.in2#' configure.in


################################################ Reconfigure the package
automake || exit 1
autoconf || exit 1

# Rebuild the package
cd /tmp
mv $package $msdos
tar cvf $thisdir/$msdos.tar $msdos || exit 1
gzip $thisdir/$msdos.tar || exit 1
mv $thisdir/$msdos.tar.gz $thisdir/$msdos.tgz || exit 1
