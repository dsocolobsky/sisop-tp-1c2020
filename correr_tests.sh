for file in test_cases/test_*
do
	echo $file
	python3 meditions.py -f 1 -m 30 -t $file -g -s 20
done
