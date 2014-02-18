#!/bin/sh
cpus=`cat /proc/cpuinfo | grep processor | wc -l`;
if [ $# == 0 ];
then
	echo "Usage: ./render.sh {folderTemplateName}";
	exit
fi;
folders=`ls | grep $1`;
for i in ${folders};
do
	echo "Rendering \"${i}\"";
	for j in `ls ${i}`;
	do
		name=`echo ${j} | sed -e 's/snap.*cpu/cpu/'`;
		if [ ${j} != ${name} ];
		then
			mv ${i}/${j} ${i}/${name};
		fi;
	done;
	cd ${i};
	for j in `seq 0 50`;
	do
		echo -e "\tmpirun -np ${cpus} pvbatch --use-offscreen-rendering render.py snap ${j}";
		mpirun -np ${cpus} pvbatch --use-offscreen-rendering render.py snap ${j} > /dev/null;
	done;
	echo -e "\tmpirun -np ${cpus} pvbatch --use-offscreen-rendering render.py stat 50";
	mpirun -np ${cpus} pvbatch --use-offscreen-rendering render.py stat 50 > /dev/null;
	cd ..;
done

rm -f res.tar.gz;
tar czf res.tar.gz `find ${folders} -iname *png`;
echo "Results are packed into res.tar.gz";
