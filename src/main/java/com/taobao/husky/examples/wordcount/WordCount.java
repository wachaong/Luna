// Husky Library 
// Copyright 2013, Taobao.com. All rights reserved.
//
// Author : zhaoyin.zy@taobao.com (Zhao Yin)

package com.taobao.husky.examples.wordcount;

import java.io.IOException;
import java.util.StringTokenizer;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;

/**
 * This is an example Hadoop Map/Reduce application.
 * It reads the text input files, breaks each line into words
 * and counts them. The output is a locally sorted list of words and the 
 * count of how often they occurred.
 *
 */
public class WordCount {
  
  static enum Counters{
	  WORD_TOTAL_CNT,
	  WORD_UNIQ_CNT,
  }
	
  /**
   * Counts the words in each line.
   * For each line of input, break the line into words and emit them as
   * (<b>word</b>, <b>1</b>).
   */
  public static class Mapper
    extends org.apache.hadoop.mapreduce.Mapper
    <LongWritable, Text, Text, IntWritable> {
    
    private Text word = new Text();
    private IntWritable one = new IntWritable(1);

    @Override
    protected void map(LongWritable key, Text value, Context context)
        throws IOException, InterruptedException {
      String line = value.toString();
      StringTokenizer itr = new StringTokenizer(line);
      while (itr.hasMoreTokens()) {
        word.set(itr.nextToken());
        context.write(word, one);
        context.getCounter(Counters.WORD_TOTAL_CNT).increment(1);
      }
    }
  }
  
  /**
   * A reducer class that just emits the sum of the input values.
   */
  public static class Reducer
    extends org.apache.hadoop.mapreduce.Reducer
    <Text, IntWritable, Text, IntWritable> {

    @Override
    protected void reduce(Text key, Iterable<IntWritable> values, Context context)
        throws IOException, InterruptedException {
      int sum = 0;
      for (IntWritable value: values)
        sum += value.get();
      context.write(key, new IntWritable(sum));
      context.getCounter(Counters.WORD_UNIQ_CNT).increment(1);
    }
  }
}
