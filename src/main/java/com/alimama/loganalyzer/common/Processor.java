package com.alimama.loganalyzer.common;
import java.util.*;

public interface Processor 
{
	public Class getMapper();
	public Class getReducer();
	public boolean run(String inputPath,String outputPath,int numMap,int numReduce,
					boolean isInputSequenceFile,Map<String,String> properties) 
	throws Exception;
}

