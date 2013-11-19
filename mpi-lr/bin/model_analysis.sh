#!/bin/sh


cat FeaDict/* | awk '{print $5}' | awk -F":" '{print $1}' | sort -u >rank-00000/feaname
echo "" | awk '{ print "cnt\tmean\tstd\tmax\tmin"}' >rank-00000/model.ana
while read fea
do
    echo $fea >> rank-00000/model.ana
    grep "$fea:" rank-00000/model.name  | awk 'BEGIN{max=-100;min=100;}{mean+=$6; ave+=$6*$6; cnt++; if(max<$6) max=$6; if(min>$6) min=$6} END{ave=ave/cnt; mean=mean/cnt; print cnt "\t" mean "\t" ave-mean*mean "\t" max "\t" min}' >>rank-00000/model.ana

    grep "$fea:" rank-00000/model.name  | sort -k6 -gr | awk '{print  $6 "\t" $1 "\t" $2}' | sh single_auc.sh >>rank-00000/model.ana
    grep "$fea:" rank-00000/model.name  | awk '{print $2/($1+$2) "\t" $1 "\t" $2}' | sort -k1 -gr | awk '{print  $2+$3 "\t" $2 "\t" $3}'  | sh single_auc.sh >>rank-00000/model.ana

done < rank-00000/feaname


cat rank-00000/model.ana

exit 0;

