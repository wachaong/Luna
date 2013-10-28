package com.alimama.display.algo.luna.cf;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;

import com.alimama.display.algo.luna.message.Luna.Display;
import com.alimama.display.algo.luna.message.Luna.User;

import display.algo.common.Constants;

public class GetNewUsers {
	static enum Counters{
		  RECORD_TOTAL_CNT,
		  New_USER_TOTAL_CNT,
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
	    		
	    		if(u.getAcookie() == null || u.getAcookie().equals("")){
	    			context.getCounter(Counters.New_USER_TOTAL_CNT).increment(1);
	    			
	    			String nickname = u.getNickname();
		    		String outkeyStr = nickname;
		    		
		    		for(int i = 0; i < u.getLabelsCount(); i++){
		    			outkeyStr += Constants.CTRL_A + u.getLabels(i).getTagsCount();
		    		}
		    		Long click = value.getClick();
		    		Text outkey = new Text();
		    		LongWritable outvalue = new LongWritable();
		    		
		    		outkey.set(outkeyStr);
		    		outvalue.set(click);
		    		context.write(outkey, outvalue);
	    		}
	    		
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

		    	String outkey = "";
		    	outkey += key.toString() + Constants.CTRL_B + pv + Constants.CTRL_A + click;
		    	Text outText = new Text();
		    	outText.set(outkey);
		    	context.write(outText, NullWritable.get());
		    }
	}
}
