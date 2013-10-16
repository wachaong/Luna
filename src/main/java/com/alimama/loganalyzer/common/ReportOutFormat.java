package com.alimama.loganalyzer.common;

import java.io.IOException;

import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.io.Writable;
import org.apache.hadoop.io.WritableComparable;
import org.apache.hadoop.mapred.JobConf;
import org.apache.hadoop.mapred.RecordWriter;
import org.apache.hadoop.mapred.TextOutputFormat;
import org.apache.hadoop.mapred.lib.MultipleOutputFormat;
import org.apache.hadoop.util.Progressable;

public class ReportOutFormat<K extends WritableComparable<?>, V extends Writable>
    extends MultipleOutputFormat<K, V> {

    private TextOutputFormat<K, V> theTextOutputFormat = null;

    @Override
        protected RecordWriter<K, V> getBaseRecordWriter(FileSystem fs, 
                JobConf job, String name, Progressable arg3) throws IOException {
            if (theTextOutputFormat == null) {
                theTextOutputFormat = new TextOutputFormat<K, V>();
            }   
            return theTextOutputFormat.getRecordWriter(fs, job, name, arg3);
        }   

    @Override
        protected String generateFileNameForKeyValue(K key, V value, String name) {
            String[] rec = key.toString().split("\t", -1);
            String filename = "";
            if (rec.length ==5) {
                // feaname feaid nonclk clk cnt
                filename = "fea-";
            } else {
                filename = "ins-";
            }
            return filename + name;
        }
}   
