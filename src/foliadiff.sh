# /bin/sh -x
if [ "$exe" = "" ]
then
    if which folialint >/dev/null 2>/dev/null
    then
	exe=$(which folialint)
    fi
fi

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
		if [ ! -f $exe ]
		then
		    exe=""
		fi
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

run_lint(){
    $exe --strip --canonical "$1" --output $2
    if [ $? -ne 0 ]
    then
	echo "foliadiff.sh: $1 is INVALID FoLiA"
	exit 1
    fi
}

t1=/tmp/${1##*/}.s1
t2=/tmp/${2##*/}.s2

run_lint $1 $t1 &
run_lint $2 $t2 &
wait
diff $t1 $t2
if [ $? -ne 0 ]
then
    echo "foliadiff.sh: differences in: $1 $2"
    exit 1
fi

exit 0
