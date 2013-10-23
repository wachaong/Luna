package com.alimama.display.algo.luna.extract;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;

import display.algo.common.Constants;

public class MergeInstance {
	static enum Counters{
		  RECORD_TOTAL_CNT,
		  CLICK,
		  NONCLICK,
		  NULL_FEATURE_RECORD,
	  }
	
	public static class Mapper
	  	extends org.apache.hadoop.mapreduce.Mapper
	  	<LongWritable, Text, Text, Text> {
	    
		
	    @Override
	    protected void setup(Context context)
	      throws IOException, InterruptedException {
	      }

	    @Override
	      protected void cleanup(Context context) 
	      throws IOException, InterruptedException {
	      }
		
	    @Override
	    protected void map(LongWritable key, Text value, Context context)
	        throws IOException, InterruptedException {
	    		context.getCounter(Counters.RECORD_TOTAL_CNT).increment(1);
	    		String str = value.toString();
	    		String[] temp = str.split(Constants.CTRL_C);
	    		Text outKey = new Text();
	    		Text outValue = new Text();
	    		outKey.set(temp[1]);
	    		outValue.set(temp[0]);
	    		context.write(outKey, outValue);
		}
	}
	
	public static class Reducer
	  	extends org.apache.hadoop.mapreduce.Reducer
	  	<Text, Text, Text, NullWritable> {
	
		    @Override
		    protected void reduce(Text key, Iterable<Text> values, Context context)
		        throws IOException, InterruptedException {
		    	Long pv = (long) 0;
		    	Long click = (long) 0;
		    	for(Text value: values){
		    		String[] temp = value.toString().split(Constants.CTRL_A);
		    		pv += Long.parseLong(temp[0]);
		    		click += Long.parseLong(temp[1]);
		    	}
		    	String outkey = "";
		    	outkey += pv + Constants.CTRL_A + click + Constants.CTRL_A + key.toString();
		    	Text outText = new Text();
		    	outText.set(outkey);
		    	context.write(outText, NullWritable.get());
		    }
	}

}
