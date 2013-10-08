package com.alimama.display.algo.luna.extract;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.net.URISyntaxException;
import java.util.HashMap;
import java.util.Map;

import org.apache.hadoop.conf.Configuration;


public class InstanceGenerator {
	private Map<String, Long> FeatureName2ID = new  HashMap<String, Long>();
	private InstanceGenerator() {
	}
	
	public static InstanceGenerator newInstance(Configuration conf) throws IOException, URISyntaxException {		

		InstanceGenerator generator = new InstanceGenerator();
		generator.init(conf);		
		return generator;
	}
	
	private void init(Configuration conf) throws IOException, URISyntaxException {
		
		System.out.println("Instance Generator init...");
		readFeatureName2FearureId(conf);
		System.out.println("Instance Generator init Success!");

	}

	private void readFeatureName2FearureId(Configuration conf) {
		System.out.println("read FeatureName2FearureId...");
		String file = conf.get("feature_map");
		try {
			BufferedReader br = new BufferedReader(new FileReader(file));
			String line = null;
			long no = 1;
			while ((line = br.readLine()) != null) {
				line = line.trim();
				FeatureName2ID.put(line,no++);
			}
			br.close();
		}catch (Exception e) {
			throw new RuntimeException(e);
		}
		System.out.println("AdvertiserId2MainCate.size()="+FeatureName2ID.size());
	}
}
