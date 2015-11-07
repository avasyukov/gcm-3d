#!/bin/bash
for i in `ls NDT/steel-pics`;
do
	echo ${i}
	cp -v NDT/steel-pics/${i}/*.vts .
	./tools/pv_render.py --task tasks/tests/ndt/steel/${i}.xml --snap-list ${i}.xml.snapshots --output-dir . --verbose render-all
	cp -v *.png NDT/steel-pics/${i}/
	rm -rf *.vts
	rm -rf *.png
done
