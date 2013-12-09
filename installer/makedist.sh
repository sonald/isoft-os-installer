#!/bin/sh

PACKAGE=rfinstaller
VERSION=0.1.0
DIR=$PACKAGE-$VERSION

URL=$(svn info --xml|grep url)
URL=${URL#<url>}
URL=${URL%</url>}

[ -e $DIR ] && rm -rf $DIR
svn checkout -q $URL $DIR
find $DIR -name .svn |xargs rm -rf

tar -jcf $DIR.tar.bz2 $DIR

rm -rf $DIR
