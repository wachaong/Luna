package com.alimama.display.algo.luna.extract;

import java.io.IOException;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;


public class MergeFeatureMap {
	static enum Counters{
		  RECORD_TOTAL_CNT,
		  CLICK,
		  NONCLICK,
		  NULL_FEATURE_RECORD,
		  USER_TAG_
	  }
	
	public static class Mapper
	  	extends org.apache.hadoop.mapreduce.Mapper
	  	<Text, NullWritable, Text, NullWritable> {
	    
		
	    @Override
	    protected void setup(Context context)
	      throws IOException, InterruptedException {
	      }

	    @Override
	      protected void cleanup(Context context) 
	      throws IOException, InterruptedException {
	      }
		
	    @Override
	    protected void map(Text value, NullWritable nu, Context context)
	        throws IOException, InterruptedException {
	    		context.getCounter(Counters.RECORD_TOTAL_CNT).increment(1);
	    		context.write(value, NullWritable.get());
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
