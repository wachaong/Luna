package com.alimama.display.algo.luna.cf;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;

import com.alimama.display.algo.luna.message.Luna.Display;
import com.alimama.display.algo.luna.message.Luna.User;

import display.algo.common.Constants;

public class GetAllUsers {
	static enum Counters{
		  RECORD_TOTAL_CNT,
		  USER_TOTAL_CNT,
	  }
	
	public static class Mapper
	  	extends org.apache.hadoop.mapreduce.Mapper
	  	<Display, NullWritable, Text, LongWritable> {
		
	    @Override
	    protected void setup(Context context)
	      throws IOException, InterruptedException {
	      }

	    @Override
	      protected void cleanup(Context context) 
	      throws IOException, InterruptedException {
	      }
		
	    @Override
	    protected void map(Display value, NullWritable n, Context context)
	        throws IOException, InterruptedException {
	    		context.getCounter(Counters.RECORD_TOTAL_CNT).increment(1);
	    		User u = value.getUser();
	    		Long click = value.getClick();
	    		String acookie = u.getAcookie();
	    		Text outkey = new Text();
	    		LongWritable outvalue = new LongWritable();
	    		
	    		outkey.set(acookie);
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
		    	context.getCounter(Counters.USER_TOTAL_CNT).increment(1);
		    	int pv = 0;
		    	int click = 0;
		    	for(LongWritable value: values){
		    		if(value.get() == 1){
		    			click++;
		    		}
		    		pv++;
		    	}

		    	String outkey = "";
		    	outkey += key.toString() + Constants.CTRL_A + pv + Constants.CTRL_A + click;
		    	outkey += key.toString();
		    	Text outText = new Text();
		    	outText.set(outkey);
		    	context.write(outText, NullWritable.get());
		    }
	}
}
