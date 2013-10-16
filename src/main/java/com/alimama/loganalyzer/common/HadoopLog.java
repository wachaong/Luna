package com.alimama.loganalyzer.common;

//import log4j
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.apache.log4j.xml.DOMConfigurator;

public class HadoopLog {
    
    public Log hadooplog = LogFactory.getLog(this.getClass());    
    private static HadoopLog singleton = null;
    public static HadoopLog getInstance() {
        if (singleton == null) {
            singleton = new HadoopLog();
        }
        return singleton;
    }

    private HadoopLog() {
        DOMConfigurator.configure("log4j.xml");
    }
}
