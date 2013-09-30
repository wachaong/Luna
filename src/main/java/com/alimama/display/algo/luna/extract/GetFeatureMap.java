package com.alimama.display.algo.luna.extract;

import java.io.IOException;


import java.util.ArrayList;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;

import com.alimama.display.algo.luna.message.Luna.Display;


public class GetFeatureMap {
	static enum Counters{
		  RECORD_TOTAL_CNT,
		  CLICK,
		  NONCLICK,
		  NULL_FEATURE_RECORD
	  }
	
	public static class Mapper
	  	extends org.apache.hadoop.mapreduce.Mapper
	  	<LongWritable, Display, Text, NullWritable> {
	    
	    @Override
	    protected void setup(Context context)
	      throws IOException, InterruptedException {
	      }

	    @Override
	      protected void cleanup(Context context) 
	      throws IOException, InterruptedException {
	      }
		
	    @Override
	    protected void map(LongWritable key, Display value, Context context)
	        throws IOException, InterruptedException {
	    	context.getCounter(Counters.RECORD_TOTAL_CNT).increment(1);
	    	
	    		ArrayList<String> allFeatures = FeatureGenerator.getAllFeatures(value);
	    		if(allFeatures == null){
	    			context.getCounter(Counters.NULL_FEATURE_RECORD).increment(1);
	    			return;
	    		}
	    		Text outkey = new Text();
	    		
	    		for(int i = 0; i < allFeatures.size(); i++){
	    			outkey.set(allFeatures.get(i));
	    			context.write(outkey, NullWritable.get());
	    		}
				
		}
	}
	
	public static class Reducer
	  	extends org.apache.hadoop.mapreduce.Reducer
	  	<Text, NullWritable, Text, NullWritable> {
	
		    @Override
		    protected void reduce(Text key, Iterable<NullWritable> values, Context context)
		        throws IOException, InterruptedException {
		    	  context.write(key, NullWritable.get());
		    }
	}
}
