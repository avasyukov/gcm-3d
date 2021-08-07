# User specific aliases and functions

adhoc_cluster_cmd() {
    srun -N1 -c1 --partition="RT_build" -l --comment="processing gcm3d results" $@;
}

progress() {
    for i in `ls $1/*out`; do
        c=`tail -n5 $i | grep "Clean up done"`
        if [ ! -z "$c" ]
        then
           echo "$i :: Done";
        else
           s=`cat $i | grep "Estimated time of calculation completion" | tail -n1 | sed -e 's/.* - //'`;
           [[ ! -z "$s" ]] && echo "$i :: $s";
        fi
    done
}

alias sl='SLURM_TIME_FORMAT="%Y/%m/%d %H:%M:%S" sacct --partition=RT -X --format="JobID,JobName%25,State,Submit%20,Elapsed,NodeList,ExitCode"'

