package com.alimama.display.algo.luna.extract;

import java.io.IOException;



import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;


import display.algo.common.Constants;


public class GetMainCate {
	static enum Counters{
		  SHOP_NUM,
		  NO_MAINTYPE,
		  FORMAT_ERROR,
		  RECORD_CNT,
		  SHOP_DELETE
	  }
	
	
	public static class Mapper
  	extends org.apache.hadoop.mapreduce.Mapper
  	<LongWritable, Text, Text, NullWritable> {
	    
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
	    	context.getCounter(Counters.RECORD_CNT).increment(1);
	    	String line = value.toString();

			String[] rec = line.split("\"", -1);
			if (rec.length != 75) {
				context.getCounter(Counters.FORMAT_ERROR).increment(1);
				return;
			}
			String shopId = rec[0];
			String isTmall = rec[31];
			String mainCate = "";
			String cateid = "";
			//NOT TMALL Seller
			if (isTmall.equals("0")) {
				mainCate = rec[72];
				cateid = rec[71];
			}
			//TMALL Seller
			if (isTmall.equals("1")) {
				mainCate = rec[63];
				cateid = rec[38];
			}
			if (mainCate.equals("")) {
				context.getCounter(Counters.NO_MAINTYPE).increment(1);
				return;
			}
			// DELETED SELLER
			String title = rec[7];
			if (title.contains("ÌÔ±¦ÍøÉ¾³ý") || title.contains("d[s")
					|| title.contains("u[") || title.contains("[d")) {

				context.getCounter(Counters.SHOP_DELETE).increment(1);
				return;
			}
			context.write(new Text(shopId + Constants.CTRL_A + cateid), NullWritable.get());
			context.getCounter(Counters.SHOP_NUM).increment(1);
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
