package com.alimama.display.algo.luna.util;

import static com.alimama.display.algo.luna.util.LunaConstants.*;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.Set;

import org.apache.hadoop.conf.Configuration;

import com.alimama.display.algo.luna.message.Luna.Ad;

import display.algo.logs.proto.MiddataMessage.DiamondMidData;

public class Filter {
	public static enum FilterBy {
    	PID_ERROR,
    	FORMAT_ERR,
    	FILTED_BY_PID,
    	FILTED_BY_TYPE,
    }
	private Set<String> filted_Pid;
	public void init(Configuration conf) throws NumberFormatException, IOException{
		readFiltedPid(conf);
	}
	private void readFiltedPid(Configuration conf) throws NumberFormatException, IOException{
		String filePath = conf.get(FILTED_PID_FILE);
		if(filePath==null || filePath.length()==0){
			System.err.print("filted_pid not indicated!!!");
		}else{
				BufferedReader br = new BufferedReader(new FileReader(filePath));
				String line = null;
				while((line=br.readLine())!=null){
					line=line.trim();
					if(line.length()==0 || line.startsWith("#")){
						continue;
					}
					filted_Pid.add(line);
				}
				br.close();
		}
		System.out.println("filted_pid.size="+filted_Pid.size());
	}
	
	public boolean valid(DiamondMidData displayLog) {
		if(filted_Pid.contains(displayLog.getPublish().getPid())){
			return true;
		}
		return false;
	}
	public FilterBy Valid(Ad ad) {
		if(ad==null){
			return  FilterBy.FILTED_BY_TYPE;
		}
		return null;
	}
}
