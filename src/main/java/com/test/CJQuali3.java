package com.test;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;

public class CJQuali3 {
	public static void main(String[] args) throws IOException{
		BufferedReader br = new BufferedReader(new FileReader("a.txt"));
		BufferedWriter bw = new BufferedWriter(new FileWriter("ans.txt"));
		int T = Integer.parseInt(br.readLine());
		for(int i = 0; i < T; i++){
			String[] tmp = br.readLine().split(" ");
			int R = Integer.parseInt(tmp[0]);
			int C = Integer.parseInt(tmp[1]);
			int M = Integer.parseInt(tmp[2]);
			
			bw.write("Case #" + (i+1) + ": " + "\n");
			
		}
		br.close();
		bw.close();
	}

}
