#!/bin/awk -f


{
    if(NR==FNR) {
        model[$1]=$2; 
    } else { 
        if (model[FNR-1] !=0) {
            print $1 "\t" model[FNR-1]
        }
    }
}
