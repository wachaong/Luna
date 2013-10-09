package com.alimama.display.algo.luna.extract;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.HashMap;
import java.util.Iterator;

import display.algo.common.Constants;

public class ShopMainCate {
	public static void main(String[] args) throws IOException{
		HashMap<String, String> hm = new HashMap<String, String>();
		String line;
		String filename = "~/Luna/data/shop2cate.txt";
		File f = new File(filename);
		if(f.exists()){
			BufferedReader br = new BufferedReader(new FileReader(filename));
			while((line = br.readLine()) != null){
				String[] temp = line.split(Constants.CTRL_A);
				if(hm.containsKey(temp[0])) continue;
				hm.put(temp[0],temp[1]);
			}
			
			f.delete();
			br.close();
		}
		for(int i = 20; i<=30; i++){
			filename = "~/Luna/data/201309"+i+"/shop2cate.txt";
			f = new File(filename);
			if(!f.exists()) continue;
			BufferedReader br = new BufferedReader(new FileReader(filename));
			while((line = br.readLine()) != null){
				String[] temp = line.split(Constants.CTRL_A);
				if(hm.containsKey(temp[0])) continue;
				hm.put(temp[0],temp[1]);
			}
			f.delete();
			br.close();
		}
		
		BufferedWriter bw = new BufferedWriter(new FileWriter("~/Luna/data/shop2cate.txt"));
		Iterator<String> iter = hm.keySet().iterator(); 
		while(iter.hasNext()){
			String key = iter.next();
			String value = hm.get(key);
			
			String item = key + Constants.CTRL_A + value;
			bw.write(item+"\n");
			
		}
		bw.close();
	}

}
