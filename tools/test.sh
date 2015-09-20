#!/bin/zsh 
set -e

cd $( dirname $0:A )

for i in {1..10};
    do time ( ./master_install/bin/gcm3d --task tasks/tests/ticket-379/p-wave-propagation.xml &> /dev/null ) ;
done ;
echo
for i in {1..10};
    do time ( ./openmp_install/bin/gcm3d --task tasks/tests/ticket-379/p-wave-propagation.xml &> /dev/null ) ;
done ;
echo
for i in {1..10};
    do time ( ./master_install/bin/gcm3d --task tasks/tests/ticket-379/p-wave-propagation-2.xml &> /dev/null ) ;
done ;
echo
for i in {1..10};
    do time ( ./openmp_install/bin/gcm3d --task tasks/tests/ticket-379/p-wave-propagation-2.xml &> /dev/null ) ;
done ;
