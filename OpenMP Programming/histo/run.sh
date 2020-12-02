#! /bin/bash 

GREEN="\e[1;32m"
RED="\e[0;31m"
NC="\e[m"

imgFile=$1
if [[ $imgFile == "" ]]
then
    imgFile="uiuc.pgm"
fi


if [[ $imgFile == "uiuc.pgm" ]]
then
    validate="validation.out"
else
    validate="validation-large.out"
fi

# clear the file
echo > plot_data.txt
outfiles=""

for program in histo histo_locks histo_atomic histo_creative
do
    summary="$program: "
    for cnt in 1 2 4 8
    do
	echo -e "$GREEN=== Running $program with $cnt threads ===$NC"
	filename="out-$program-$cnt.out"
	outfiles+=" $filename"
	# execute the program and store the output
	OMP_NUM_THREADS=$cnt ./$program $imgFile > $filename
	# grep the runtime
	cat $filename | grep "Runtime"
	# store only the number
	summary+="$(cat $filename | perl -ne '/^Runtime =[\s]*([\d.]*)[\s]*seconds$/ and print "$1"'), "
	# remove the last line which contains the Runtime
	sed -i '$ d' $filename
	# remove the last line which is empty
	sed -i '$ d' $filename
	# add some delay between iterations
	#sleep 1
    done
    # use ${summary%, } to remove the trailing ", "
    # use tee to print & write to file
    echo -e "$RED${summary%, } \n$NC" | tee -a plot_data.txt
done

# valid the output
for f in $outfiles
do
    echo -e "$GREEN=== Comparing $f with the standard output ===$NC"
    if diff $f $validate > /dev/null
    then
	echo "same"
	rm $f
    else
	echo -e "$RED inconsistent output $NC"
	echo "run \"diff $f $validate\" to see more detail"
    fi
done


