sort $1 > $1-sorted
sort $2 > $2-sorted
diff $1-sorted $2-sorted > /dev/null 2>&1
res=$?
if [ $res -eq 0 ]
then
	echo "IGUAIS"
else
	echo "DIFERENTES"
fi
rm -f $1-sorted $2-sorted
