package com.alimama.display.algo.luna.extract;

import java.io.IOException;
import java.net.URISyntaxException;
import java.util.Collection;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.io.BytesWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;




import com.alimama.display.algo.luna.message.Luna.Display;
import com.alimama.display.algo.luna.util.DataTransform;
import com.alimama.display.algo.luna.util.Filter;
import com.alimama.display.algo.luna.util.Filter.FilterBy;

import display.algo.logs.proto.MiddataMessage.DiamondMidData;

public class FilterData {
	static enum Counters{
		  RECORD_TOTAL_CNT,
		  RECORD_FILTERED_CNT,
		  PV_ALL,
		  PV_FILTED,
		  PV_OK,
		  CLICK,
		  NONCLICK
	  }
	
	static Filter filter = new Filter();
	
	public static class Mapper
    	extends org.apache.hadoop.mapreduce.Mapper
    	<BytesWritable, BytesWritable, Text, Display> {
	    
		
		private DataTransform dt = new DataTransform();
		DiamondMidData.Builder dmdb = DiamondMidData.newBuilder();
		
	    @Override
	    protected void setup(Context context)
	      throws IOException, InterruptedException {
	        Configuration conf = context.getConfiguration();
	        
	        filter.init(conf);
	        try {
				dt.init(conf);
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
	    protected void map(BytesWritable key, BytesWritable value, Context context)
	        throws IOException, InterruptedException {
	    	context.getCounter(Counters.RECORD_TOTAL_CNT).increment(1);
	    	DiamondMidData dmd = dmdb.clear()
					.mergeFrom(value.getBytes(), 0, value.getLength()).build();
	      
	    	if (!filter.valid(dmd)) {
	    		context.getCounter(Counters.RECORD_FILTERED_CNT).increment(1);
	    		return;
	    	}
	      
	    	Collection<Display> displays = dt.getDisplays(dmd);
	    	//context.getCounter("FRAME_CNT", String.valueOf(displays.size())).increment(1);
	    	
	    	
			for(Display display:displays){
				context.getCounter(Counters.PV_ALL).increment(1);
				FilterBy a = filter.Valid(display.getAd());
				if(a!=null){
					context.getCounter(a).increment(1);
					context.getCounter(Counters.PV_FILTED).increment(1);
					continue;
				}
				Text outKey = new Text();
				String out = "";
				
				out += "AD:" + FeatureGenerator.GetAdFeaturesStr(display.getAd()) + " | ";
				out += "USER:" + FeatureGenerator.GetUserFeaturesStr(display.getUser()) + " | ";
				out += "CONTEXT:" + FeatureGenerator.GetContextFeaturesStr(display.getContext()) + " | ";
				//out += display.getSessionid()+"_";
				//out += display.getAd().getTransId()+"_";
				//out += display.getAd().getAdboardId()+"_";
				//out += display.getAd().getCustomerId()+"_";
				//out += display.getContext().getPid()+"_";
				//out += display.getContext().getUrl()+"_";
				
				//String nickName = display.getUser().getNickname();
				//if(nickName == null || nickName.equals("")) nickName = "UNKNOWN";
				//out += nickName + "_";
				
				out += "CLICK:" + display.getClick();
				outKey.set(out);
				if(display.getClick() == 1){
					context.getCounter(Counters.CLICK).increment(1);
				}
				else
					context.getCounter(Counters.NONCLICK).increment(1);
				//outKey.set(display.getClickid());
				context.write(outKey, display);
				context.getCounter(Counters.PV_OK).increment(1);
			}
	    }
	}
	
	public static class Reducer
    	extends org.apache.hadoop.mapreduce.Reducer
    	<Text, Display, Text, NullWritable> {

	    @Override
	    protected void reduce(Text key, Iterable<Display> values, Context context)
	        throws IOException, InterruptedException {
	      for (Display value: values)
	    	  context.write(key, NullWritable.get());
	    }
	}
}
