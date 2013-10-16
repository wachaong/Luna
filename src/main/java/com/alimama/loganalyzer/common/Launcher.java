package com.alimama.loganalyzer.common;
import java.util.*;

public class Launcher 
{

    public static void printErrorAndExit(String message) 
    {
        System.out.println(message);
        System.out.println("Usage : Launcher [ProcessorClass] [InputPath] [OutputPath] [numOfMappers] [numOfReducers][isInputSequenceFile] <name1=value1> <name2=value2>...");
        System.exit(1);
    }

    public static void main(String args[]) throws Exception
    {

        try {
            //	Process process = Runtime.getRuntime().exec("rm -rf D:\\test\\"); 
            if (args.length<6) {
                System.err.println("args.length:"+ args.length + "<6");
                printErrorAndExit("Invalid number of arguments");
            }
            Processor processor=null;
            try {
                processor=(Processor)(Class.forName(args[0]).newInstance());	
            } catch (Exception e) {
                System.err.println("try processor " + args[0] + " fail");
                printErrorAndExit("Invalid processor "+args[0]);
            }
            Map<String,String> properties=new HashMap<String,String>();
            if (args.length>6) {
                for (int i=6;i<args.length;i++) {
                    String arg=args[i];
                    int p=arg.indexOf("=");
                    if (p!=-1) {
                        properties.put(arg.substring(0,p),arg.substring(p+1));
                    } else {
                        System.err.println("Invalid parameter "+arg);
                        printErrorAndExit("Invalid parameter "+arg);
                    }
                }
            }
            boolean success=processor.run(args[1], args[2],Integer.parseInt(args[3]),Integer.parseInt(args[4]),Boolean.parseBoolean(args[5]),properties);
            System.err.println("input : " + args[1]);
            System.err.println("output : " + args[2]);
            System.err.println("Alimama_Status : "+success);
            System.exit(0);
        } catch (Exception e) {
            e.printStackTrace();
            System.out.println("Alimama_Status : false");
            System.exit(1);
        }
    }
}

