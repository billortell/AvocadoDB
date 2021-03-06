#!/bin/bash
INFO="$1"
FILE="$2"

if test ! -f "$FILE";  then
  version=`fgrep PACKAGE_VERSION Makefile | awk '{print $3}'`
  echo "#define TRIAGENS_VERSION \"$version\"" > ${FILE}
fi

DIR=`dirname $INFO`

if test -d ${DIR}/.svn;  then
  revision=`(cd $DIR && svnversion)`
else
  if test ! -f "$INFO";  then
    revision="exported"
  else
    revision=`grep 'Revision:' $INFO | awk '{print $2}'`
  fi
fi

if test -z "$revision";  then
  echo "$0: cannot read revision from file $INFO"
  exit 1
fi

version=`sed -e 's:.*"\([^[]*[^ []\).*":\1:' $FILE`

if test -z "$version";  then
  echo "$0: cannot read vision from file $FILE"
  exit 1
fi

echo "#define TRIAGENS_VERSION \"$version [$revision]\"" > ${FILE}.tmp

if cmp -s $FILE ${FILE}.tmp;  then
  rm ${FILE}.tmp
else
  mv ${FILE}.tmp $FILE
fi
