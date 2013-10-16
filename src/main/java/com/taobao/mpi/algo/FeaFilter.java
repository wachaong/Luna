/** 
 * @copyright (c) 2012 Taobao.com, Inc. All Rights Reserved
 * @author : xiaowen.zl
 * @fax    : +86.10.815.72428
 * @e-mail : xiaowen.zl@taobao.com
 * @date   : 2013-01-08 - 16:12
 * @version: 1.0.0.1
 * 
 * @file   : FeaFilter.java
 * @brief  :
 */

package com.taobao.mpi.algo;

import java.io.IOException;
import java.util.Iterator;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

import com.alimama.loganalyzer.common.*;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.JobConf;
import org.apache.hadoop.mapred.MapReduceBase;
import org.apache.hadoop.mapred.Mapper;
import org.apache.hadoop.mapred.OutputCollector;
import org.apache.hadoop.mapred.Reducer;
import org.apache.hadoop.mapred.Reporter;


public class FeaFilter extends AbstractProcessor {


  public static class Map extends MapReduceBase implements
      Mapper<LongWritable, Text, Text, Text> {

          enum Stat {
              TotalFea, ValidFea
          }

          HadoopLog m_log = HadoopLog.getInstance();
          public int pvThreshold = 0;
          public Text outKey = new Text("");

          public void configure(JobConf job) {
              try {
                  pvThreshold  = Integer.parseInt(job.get("pv"));
              } catch(NumberFormatException e) {
                  throw  new RuntimeException(e.toString());
              }
          }

          public void map(LongWritable key, Text value,
                  OutputCollector<Text, Text> output, Reporter reporter)
              throws IOException {
              String line = value.toString();
              if (line == null || line.isEmpty()) {
                  return;
              }
              reporter.incrCounter(Stat.TotalFea, 1);

              //outKey.set(feaNonClk + "\t" + feaClk + "\t" + feaCnt + "\t" + md5sum(key.toString()) + "\t" + key.toString());
              String[] rec = line.split(Common.SP_TAB, -1);
              if (rec.length != 5){
                  return;
              }

              long feaNonClk = 0;
              long feaClk = 0;
              try {
                  feaNonClk = Long.parseLong(rec[0]);
                  feaClk = Long.parseLong(rec[1]);
              } catch(NumberFormatException e) { 
                  return;
              }

              if (feaNonClk + feaClk <= pvThreshold) {
                  return;
              }

              outKey.set(rec[3]+"\t"+rec[4]);
              output.collect(outKey, new Text(""));
              reporter.incrCounter(Stat.ValidFea, 1);
          }
      }


  public static class Reduce extends MapReduceBase implements
      Reducer<Text, Text, Text, Text> {
          public void reduce(Text key, Iterator<Text> values,
                  OutputCollector<Text, Text> output, Reporter reporter)
              throws IOException {
              output.collect(key, null);
          }
      }

  protected void configJob(JobConf conf) {
      conf.setMapOutputKeyClass(Text.class);
      conf.setMapOutputValueClass(Text.class);

      conf.setOutputKeyClass(Text.class);
      conf.setOutputValueClass(Text.class);
  }

  @Override
      public Class getMapper() {
          return Map.class;
      }

  @Override
      public Class getReducer() {
          return Reduce.class;
      }

}
