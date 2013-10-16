/** 
 * @copyright (c) 2012 Taobao.com, Inc. All Rights Reserved
 * @author : xiaowen.zl
 * @fax    : +86.10.815.72428
 * @e-mail : xiaowen.zl@taobao.com
 * @date   : 2013-01-08 - 16:12
 * @version: 1.0.0.1
 * 
 * @file   : Common.java
 * @brief  :
 */

package com.taobao.mpi.algo;

import java.util.*;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.List;

public class Common
{
    //Spliters
	public static final String CTRL_A = "\u0001";
    public static final String CTRL_B = "\u0002";
    public static final String CTRL_C = "\u0003";
    public static final String CTRL_D = "\u0004";
    public static final String CTRL_E = "\u0005";
    public static final String CTRL_Z = "\u001A";
    public static final String SP = "\"";
    public static final String SPACE = " ";
    public static final String SP_TAB = "\t";
    public static final String SP_COMMON = ":";
    public static final String SP_COMMA = ",";
    public static final String SP_SEMI = ";";
    public static final String SP_SIGN_EQUAL = "=";
    public static final String SP_UNDERLINE = "_";
 
   public static final String RDCLK = "rd_clk";
   public static final String P4PCLK = "p4p_clk";
   public static final String P4PCLKCOST = "p4p_clk_cost";
   public static final String ATHENAPV = "athena_pv";
   public static final String ECPM = "ecpm";
   public static final String ELEMENTID = "elementid";

    public static final String BUCKETID[]={
        "dynamic_bucket"};
   
    /**
     * @brief 判断字符串是否为�?     *
     * @param str
     *
     * @return �?返回 true �?非空返回 false
     */
    public static boolean isEmpty(String str)
    {
	    if (str == null || str.equals("")) {
		    return true;
        }
	    return false;
    }

    public static long getDateTime(String strDate) {
           return getDateByFormat(strDate, "yyyyMMdd").getTime();
    }

    /**
     * 计算出两个日期之间相差的天数
     * 建议date1 大于 date2 这样计算的�?为正�?     * @param date1 日期1
     * @param date2 日期2
     * @return date1 - date2
     */
    public static int dateInterval(long date1, long date2) {
        if(date2 > date1){
            date2 = date2 + date1;
            date1 = date2 - date1;
            date2 = date2 - date1;
        }
        /*
         * Canlendar 该类是一个抽象类
         * 提供了丰富的日历字段
         *
         * 本程序中使用到了
         * Calendar.YEAR 日期中的年份
         * Calendar.DAY_OF_YEAR   当前年中的天�?         * getActualMaximum(Calendar.DAY_OF_YEAR) 返回今年�?365 天还�?66�?         */
        Calendar calendar1 = Calendar.getInstance(); // 获得�?��日历
        calendar1.setTimeInMillis(date1); // 用给定的 long 值设置此 Calendar 的当前时间�?�?
        Calendar calendar2 = Calendar.getInstance();
        calendar2.setTimeInMillis(date2);
        // 先判断是否同�?        
        int y1 = calendar1.get(Calendar.YEAR);
        int y2 = calendar2.get(Calendar.YEAR);

        int d1 = calendar1.get(Calendar.DAY_OF_YEAR);
        int d2 = calendar2.get(Calendar.DAY_OF_YEAR);
        int maxDays = 0;
        int day = 0;
        if(y1 - y2 > 0){
            day = numerical(maxDays, d1, d2, y1, y2, calendar2);
        }else{
            day = d1 - d2;
        }
        return day;
    }

    /**
     * 日期间隔计算
     * 计算公式(示例):
     *    20121230 - 20071001
     *    取出20121230这一年过了多少天 d1 = 365   取出20071001这一年过了多少天 d2 = 274
     *    如果2007年这�?���?66天就要让间隔的天�?1，因�?月份�?9日�?
     * @param maxDays 用于记录�?��中有365天还�?66�?     * @param d1 表示在这年中过了多少�?     * @param d2 表示在这年中过了多少�?     * @param y1 当前�?010�?     * @param y2 当前�?012�?     * @param calendar 根据日历对象来获取一年中有多少天
     * @return 计算后日期间隔的天数
     */
    public static int numerical(int maxDays, int d1, int d2, int y1, int y2, Calendar calendar){
        int day = d1 - d2;
        int betweenYears = y1 - y2;
        List<Integer> d366 = new ArrayList<Integer>();

        if(calendar.getActualMaximum(Calendar.DAY_OF_YEAR) == 366){
            //System.out.println(calendar.getActualMaximum(Calendar.DAY_OF_YEAR));
            day += 1;
        }

        for (int i = 0; i < betweenYears; i++) {
            // 当年 + 1 设置下一年中有多少天
            calendar.set(Calendar.YEAR, (calendar.get(Calendar.YEAR)) + 1);
            maxDays = calendar.getActualMaximum(Calendar.DAY_OF_YEAR);
            // 第一�?366 天不�?+ 1 将所�?66记录，先不进行加入然后再少加�?��
            if(maxDays != 366){
                day += maxDays;
            }else{
                d366.add(maxDays);
            }
            // 如果�?���?�� maxDays 等于366 day - 1
            if(i == betweenYears-1 && betweenYears > 1 && maxDays == 366){
                day -= 1;
            }
        }

        for(int i = 0; i < d366.size(); i++){
            // �?��或一个以上的366�?            
        	if(d366.size() >= 1){
                day += d366.get(i);
            }
            //    else{
            //     day -= 1;
            //    }
        }
        return day;
    }

    /**
     * * @param strDate 日期字符�?     * * @param format 日期格式
     * * @return   Date
     * */
    public static Date getDateByFormat(String strDate, String format) {
        SimpleDateFormat sdf = new SimpleDateFormat(format);
        try{
            return (sdf.parse(strDate));
        }catch (Exception e){
            return null;
        }
    }

}

