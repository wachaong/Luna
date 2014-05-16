package com.test;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Arrays;

public class CJQuali4 {
	public static void main(String[] args) throws IOException{
		BufferedReader br = new BufferedReader(new FileReader("D-large.in"));
		BufferedWriter bw = new BufferedWriter(new FileWriter("ans.txt"));
		int T = Integer.parseInt(br.readLine());
		for(int i = 0; i < T; i++){
			int N = Integer.parseInt(br.readLine());
			double[] Naomi = new double[N];
			double[] Ken = new double[N];
			boolean[] usedWar = new boolean[N];
			boolean[] usedDef = new boolean[N];
			String[] tmp = br.readLine().split(" ");
			for(int j = 0; j < N; j++) Naomi[j] = Double.parseDouble(tmp[j]);
			tmp = br.readLine().split(" ");
			for(int j = 0; j < N; j++) {
				Ken[j] = Double.parseDouble(tmp[j]);
				usedWar[j] = false;
				usedDef[j] = false;
			}
			Arrays.sort(Naomi);
			Arrays.sort(Ken);
			int scoreWar = 0;
			int scoreDef = 0;
			
			for(int j = 0; j < N; j++){
				int k = 0;
				for(; k < N; k++){
					if(!usedWar[k] && Ken[k] > Naomi[j]){
						usedWar[k] = true;
						break;
					}
				}
				if(k == N){
					for(k = 0; k < N; k++){
						if(!usedWar[k]){
							usedWar[k] = true;
							if(Ken[k] < Naomi[j])
								scoreWar++;
						}
					}
				}
			}
			int k = 0;
			int max = N-1;
			for(int j = 0; j < N; j++){
				if(!usedDef[k] && Ken[k] >= Naomi[j]){
					usedDef[max--] = true;
				}
				else{
					usedDef[k] = true;
					if(Ken[k] < Naomi[j])
						scoreDef++;
					k++;
				}
			}
			
			bw.write("Case #" + (i+1) + ": " +scoreDef +" "+scoreWar + "\n");
			
		}
		br.close();
		bw.close();
	}

}
