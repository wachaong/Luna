package com.alimama.display.algo.luna.cf;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;


import display.algo.common.Constants;

public class MergeAllAds {
	static enum Counters{
		  RECORD_TOTAL_CNT,
		  AD_TOTAL_CNT,
		  NO_TARGETING_INFO_AD,
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
	    		String[] temp = str.split(Constants.CTRL_A);
	    		Text outKey = new Text();
	    		Text outValue = new Text();
	    		outKey.set(temp[0]);
	    		outValue.set(temp[1]+Constants.CTRL_A+temp[2]);
	    		context.write(outKey, outValue);
		}
	}
	
	public static class Reducer
	  	extends org.apache.hadoop.mapreduce.Reducer
	  	<Text, Text, Text, NullWritable> {
	
		    @Override
		    protected void reduce(Text key, Iterable<Text> values, Context context)
		        throws IOException, InterruptedException {
		    	context.getCounter(Counters.AD_TOTAL_CNT).increment(1);
		    	int pv = 0;
		    	int click = 0;
		    	for(Text value: values){
		    		String str = value.toString();
		    		String[] temp = str.split(Constants.CTRL_A);
		    		pv += Integer.parseInt(temp[0]);
		    		click += Integer.parseInt(temp[1]);
		    	}

		    	String outkey = "";
		    	outkey += key.toString() + Constants.CTRL_A + pv + Constants.CTRL_A + click;
		    	Text outText = new Text();
		    	outText.set(outkey);
		    	context.write(outText, NullWritable.get());
		    }
	}

}

