package com.alimama.display.algo.luna.extract;

import java.io.IOException;
import java.net.URISyntaxException;
import java.util.ArrayList;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;

import com.alimama.display.algo.luna.message.Luna.Display;

import display.algo.common.Constants;

public class GetInstance {
	static enum Counters{
		  RECORD_TOTAL_CNT,
		  INSTANCE_TOTAL_CNT,
		  NULL_FEATURE_RECORD
	  }
	
	public static class Mapper
	  	extends org.apache.hadoop.mapreduce.Mapper
	  	<Display, NullWritable, Text, LongWritable> {
	    	InstanceGenerator ig;
		
	    @Override
	    protected void setup(Context context)
	      throws IOException, InterruptedException {
	    	try {
				ig = InstanceGenerator.newInstance(context.getConfiguration());
			} catch (URISyntaxException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
	      }

	    @Override
	      protected void cleanup(Context context) 
	      throws IOException, InterruptedException {
	      }
		
	    @Override
	    protected void map(Display value, NullWritable n, Context context)
	        throws IOException, InterruptedException {
	    		context.getCounter(Counters.RECORD_TOTAL_CNT).increment(1);
	    	
	    		ArrayList<String> allFeatures = FeatureGenerator.getAllFeatures(value);
	    		if(allFeatures == null){
	    			context.getCounter(Counters.NULL_FEATURE_RECORD).increment(1);
	    			return;
	    		}
	    		Text outkey = new Text();
	    		Long click = value.getClick();
	    		LongWritable outvalue = new LongWritable();
	    		
	    		outkey.set(ig.getInstance(allFeatures));
	    		outvalue.set(click);
	    		context.write(outkey, outvalue);
		}
	}
	
	public static class Reducer
	  	extends org.apache.hadoop.mapreduce.Reducer
	  	<Text, LongWritable, Text, NullWritable> {
	
		    @Override
		    protected void reduce(Text key, Iterable<LongWritable> values, Context context)
		        throws IOException, InterruptedException {
		    	int pv = 0;
		    	int click = 0;
		    	for(LongWritable value: values){
		    		if(value.get() == 1){
		    			click++;
		    		}
		    		pv++;
		    	}
		    	float beta = 1;
		    	String outkey = "";
		    	outkey += pv + Constants.CTRL_A + click + Constants.CTRL_A + beta + Constants.CTRL_A;
		    	outkey += key.toString();
		    	Text outText = new Text();
		    	outText.set(outkey);
		    	context.write(outText, NullWritable.get());
		    }
	}

}
