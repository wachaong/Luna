package com.alimama.display.algo.luna.feedback;

import java.io.IOException;

import org.apache.hadoop.io.BytesWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.MapReduceBase;
import org.apache.hadoop.mapred.Mapper;
import org.apache.hadoop.mapred.OutputCollector;
import org.apache.hadoop.mapred.Reporter;

public class CombMapper extends MapReduceBase implements
	Mapper<BytesWritable, BytesWritable, Text, BytesWritable>{

	@Override
	public void map(BytesWritable arg0, BytesWritable arg1,
			OutputCollector<Text, BytesWritable> arg2, Reporter arg3)
			throws IOException {
		// TODO Auto-generated method stub
		
	}

}
