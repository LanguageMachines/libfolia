# /bin/sh -x
if [ "$exe" = "" ]
then
    exe="$HOME/usr/local/bin/folialint"
    if [ ! -f $exe ]
    then
	exe="/exp/sloot/usr/local/bin/folialint"
	if [ ! -f $exe ]
	then
	    exe="/usr/local/bin/folialint"
	    if [ ! -f $exe ]
	    then
		exe="/usr/bin/folialint"
	    fi
	fi
    fi
fi

if [ "$exe" = "" ];
then
    "cannot locate folialint"
    exit 1
fi

if [ $# -ne 2 ]
  then
      echo "two arguments expected"
      exit 1
fi

t1=/tmp/${1##*/}.s1
t2=/tmp/${2##*/}.s2
$exe --strip --KANON "$1" --output $t1
$exe --strip --KANON "$2" --output $t2
diff $t1 $t2
if [ $? -ne 0 ]
then
    echo "$1 and $2 are different"
    exit 1
fi

exit 0
