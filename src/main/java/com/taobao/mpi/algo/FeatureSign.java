
/*
 * modified by yanling.yl
 * 2013/11/5
 * hashcode length modified to 32bits, e.g. #ffffffff
 * 
 * Add AdFeature, UserFeature, OtherFeature sign in front of all hash codes;
 * AdFeature: a 		feature_type: 0
 * UserFeature: u 		feature_type: 1 
 * OtherFeature: o		feature_type: 2
 */

package com.taobao.mpi.algo;

import java.io.IOException;
import java.util.Iterator;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

import com.alimama.display.algo.luna.message.Luna;
import com.alimama.display.algo.luna.util.LunaConstants;
import com.alimama.loganalyzer.common.*;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.JobConf;
import org.apache.hadoop.mapred.MapReduceBase;
import org.apache.hadoop.mapred.Mapper;
import org.apache.hadoop.mapred.OutputCollector;
import org.apache.hadoop.mapred.Reducer;
import org.apache.hadoop.mapred.Reporter;


public class FeatureSign extends AbstractProcessor {

    enum Stat {
        Ins, Fea, ValidFea, feaFilter
    }

    enum Error {
        err1, err2, err3, err4, err5, err6, err7, err8, err9;
    }
    
    
    static public String md5sum(String plainText) {
        StringBuffer buf = new StringBuffer("");
        try {
            MessageDigest md = MessageDigest.getInstance("MD5");
            byte b[] = md.digest(plainText.getBytes());
            int i = 0;
            //for (int offset = 0; offset < b.length; offset++) {
            for (int offset = 0; offset < 4; offset++) {
                i = b[offset];
                if (i < 0) {
                    i += 256;
                }
                if (i < 16) {
                    buf.append("0");
                }
                buf.append(Integer.toHexString(i));
            }
        } catch (NoSuchAlgorithmException e) {
            e.printStackTrace();
            return "";
        }
        return buf.toString();
   }
   
   /*
    * Get feature Type
    * AdFeature: a 		return 0
    * UserFeature: u 	return 1 
    * OtherFeature: o 	return 2
    */
   public static int getFeatureType(String featureName){
	   //Ad Targeting Feature
	   if(featureName.startsWith(LunaConstants.AD_CROWDPOWER_PREFIX) 
			   || featureName.startsWith(LunaConstants.AD_INTEREST_PREFIX)
			   || featureName.startsWith(LunaConstants.AD_MAINCATETARGETING_PREFIX)){
		   return 0;
	   }
	   
	   //User Targeting Feature
	   if(featureName.startsWith(LunaConstants.USER_CROWDPOWER_PREFIX)
			   || featureName.startsWith(LunaConstants.USER_INTEREST_PREFIX)
			   || featureName.startsWith(LunaConstants.USER_MAINCATETARGETING_PREFIX)){
		   return 1;
	   }
	   
	   //Other type Feature
	   else{
		   return 2;
	   }
   }
    
   public static String getFeatureSign(String featureName){
	   StringBuffer result = new StringBuffer("");
	   int type = getFeatureType(featureName);
	   if(type == 0){
		   result.append("a");
	   }
	   else if(type == 1){
		   result.append("u");
	   }
	   else if(type == 2){
		   result.append("o");
	   }
	   result.append(md5sum(featureName));
	   return result.toString();
   }

   
   public static class Map extends MapReduceBase implements
       Mapper<LongWritable, Text, Text, Text> {

           HadoopLog m_log = HadoopLog.getInstance();
           public Text outKey = new Text("");
           public Text outValue = new Text("");

           public void map(LongWritable key, Text value,
                   OutputCollector<Text, Text> output, Reporter reporter)
               throws IOException {
               String line = value.toString();
               if (line == null || line.isEmpty()) {
                   reporter.incrCounter(Error.err1, 1);
                   return;
               }
               String[] groupIns = line.split(Common.CTRL_C, -1);
               String ins = groupIns[0];
               String group = "";
               if (groupIns.length==2) {
                 group = groupIns[1];
               }

               String[] rec = ins.split(Common.CTRL_A, -1);
               if (rec.length <=2){
                   reporter.incrCounter(Error.err2, 1);
                   return;
               }

               long p = 0;
               long c = 0;

               try {
                  p = Long.parseLong(rec[0]);
                  c = Long.parseLong(rec[1]);
               } catch (Exception e) {
                   reporter.incrCounter(Error.err8, 1);
                   return;
               }

               if (p<=0 || c<0) {
                   reporter.incrCounter(Error.err6, 1);
                   return;
               }

               if (c>0 && p<c) {
                   p=c;
               }

               String nonclk = String.valueOf(p-c);
               String clk = String.valueOf(c);

               String feature = "";
               boolean bstart = true;

               for (int i = 2; i < rec.length; i++) {

                   if (Common.isEmpty(rec[i])) {
                       reporter.incrCounter(Error.err9, 1);
                       continue;
                   }
                   String[] fea = rec[i].split(Common.CTRL_B, -1);

                   if (Common.isEmpty(fea[0]) 
                           || Common.isEmpty(fea[0].trim())
                           || (fea.length == 2 && Common.isEmpty(fea[1]) ) 
                           || fea.length >2) {
                       reporter.incrCounter(Error.err9, 1);
                       continue;
                   }

                   if (fea.length == 2) {
                       try {
                           double v = Double.parseDouble(fea[1]);
                       } catch (Exception e) {
                           reporter.incrCounter(Error.err9, 1);
                           continue;
                       }
                   }

                   // output each feature
                   outKey.set(fea[0].trim());
                   outValue.set(nonclk + Common.CTRL_A + clk + Common.CTRL_A + "1");
                   output.collect(outKey, outValue);

                   if (!bstart) {
                       feature += Common.CTRL_A;
                   }
                   bstart = false;

                   String feaname = fea[0].trim().replace('\t', '');
                   if (fea.length == 2) {
                       feature +=  getFeatureSign(feaname) + Common.CTRL_B + fea[1];
                   }else {
                       feature +=  getFeatureSign(feaname);
                   }
               }
               feature +=  Common.CTRL_C + group;

               // output ins
               outKey.set(feature);
               outValue.set(nonclk + Common.CTRL_A + clk);
               output.collect(outKey, outValue);
           }
       }

   public static class Combiner extends MapReduceBase implements
       Reducer<Text, Text, Text, Text> {

           public Text outValue = new Text("");

           public void reduce(Text key, Iterator<Text> values,
                   OutputCollector<Text, Text> output, Reporter reporter)
               throws IOException {

               boolean isIns = false;
               boolean isFea = false;
               long insNonClk = 0;
               long insClk = 0;
               long feaNonClk = 0;
               long feaClk = 0;
               long feaCnt = 0;
               while (values.hasNext()) {
                   String value = values.next().toString();
                   String[] rec = value.split(Common.CTRL_A, -1);

                   if (rec.length == 2) {
                       isIns=true;
                       insNonClk += Long.parseLong(rec[0]);
                       insClk += Long.parseLong(rec[1]);

                   } else if (rec.length == 3) {
                       isFea=true;
                       feaNonClk += Long.parseLong(rec[0]);
                       feaClk += Long.parseLong(rec[1]);
                       feaCnt += Long.parseLong(rec[2]);
                   }
               }

               if (isIns) {
                   outValue.set(insNonClk + Common.CTRL_A + insClk);
                   output.collect(key, outValue);
               } 

               if (isFea) {
                   outValue.set(feaNonClk + Common.CTRL_A + feaClk + Common.CTRL_A + feaCnt);
                   output.collect(key, outValue);
               }
           }
       }

   public static class Reduce extends MapReduceBase implements
       Reducer<Text, Text, Text, Text> {

           HadoopLog m_log = HadoopLog.getInstance();
           public int pvThreshold = 0;
           public int isFeaFilter = 0;
           public Text outKey = new Text("");

           public void configure(JobConf job) {
               try {
                   pvThreshold  = Integer.parseInt(job.get("pv"));
                   isFeaFilter = Integer.parseInt(job.get("isFeaFilter"));
               } catch(NumberFormatException e) {
                   throw  new RuntimeException(e.toString());
               }
           }

           public void reduce(Text key, Iterator<Text> values,
                   OutputCollector<Text, Text> output, Reporter reporter)
               throws IOException {
               boolean isIns = false;
               boolean isFea = false;
               long insNonClk = 0;
               long insClk = 0;
               long feaNonClk = 0;
               long feaClk = 0;
               long feaCnt = 0;
               while (values.hasNext()) {
                   String value = values.next().toString();
                   String[] rec = value.split(Common.CTRL_A, -1);

                   if (rec.length == 2) {
                       isIns=true;
                       try {
                           insNonClk += Long.parseLong(rec[0]);
                           insClk += Long.parseLong(rec[1]);
                       } catch(NumberFormatException e) {  
                           reporter.incrCounter(Error.err3, 1);
                           continue;
                       }

                   } else if (rec.length == 3) {
                       isFea=true;
                       try {
                           feaNonClk += Long.parseLong(rec[0]);
                           feaClk += Long.parseLong(rec[1]);
                           feaCnt += Long.parseLong(rec[2]);
                       } catch(NumberFormatException e) {  
                           reporter.incrCounter(Error.err3, 1);
                           continue;
                       }
                   }
               }

               if (isIns) {
                   outKey.set(insNonClk + Common.CTRL_A + insClk + Common.CTRL_A + key);
                   output.collect(outKey, null);
                   reporter.incrCounter(Stat.Ins, 1);
               } 

               if (isFea) {
                   reporter.incrCounter(Stat.Fea, 1);
                   if (isFeaFilter >0 && feaNonClk+feaClk <= pvThreshold) {
                       reporter.incrCounter(Stat.feaFilter, 1);
                       return;
                   }

                   String feaname = key.toString().trim().replace('\t', '');
                   outKey.set(feaNonClk + "\t" + feaClk + "\t" + feaCnt + "\t" + getFeatureSign(feaname) + "\t" + feaname);
                   output.collect(outKey, null);
                   reporter.incrCounter(Stat.ValidFea, 1);
               }
           }
       }

   protected void configJob(JobConf conf) {
       conf.setCombinerClass(Combiner.class);
       conf.setMapOutputKeyClass(Text.class);
       conf.setMapOutputValueClass(Text.class);

       conf.setOutputKeyClass(Text.class);
       conf.setOutputValueClass(Text.class);
       conf.setOutputFormat(ReportOutFormat.class);
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
