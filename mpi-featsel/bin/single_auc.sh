#!/bin/sh 

awk '{
    pctr = $1;
    pv = $2+$3;
    clk = $3;
    if(pctr != last_pctr){ 
        if(pv_sum > 0) {
            printf "%d\t%d\n", pv_sum, clk_sum;    
        }
        last_pctr = pctr;    
        pv_sum = pv;    
        clk_sum = clk;
    }else{    
        pv_sum += pv;    
        clk_sum += clk;
    }
   
}
END{
    if(pctr == last_pctr) {
        printf "%d\t%d\n",pv_sum, clk_sum;    
    }
}' | awk '{
    clk=$2;
    noclk=$1-$2;
    noclksum += noclk;
    oldclksum = clksum;
    clksum += clk;   
    auc += (clksum + oldclksum)*noclk/2;
}
END{ 
    if (clksum*noclksum != 0) {
        aucfinal=auc/(clksum*noclksum); 
        printf "%.8f\n",  aucfinal;
    }else {
        printf "%s\t0\n", cls;
    }
}'

