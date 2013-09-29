package com.alimama.display.algo.luna.util;


import java.io.IOException;
import java.net.URISyntaxException;
import java.util.Calendar;
import java.util.Collection;
import java.util.Vector;

import org.apache.hadoop.conf.Configuration;

import com.alimama.display.algo.luna.ad.AdExtractor;
import com.alimama.display.algo.luna.message.Luna.Ad;
import com.alimama.display.algo.luna.message.Luna.Context;
import com.alimama.display.algo.luna.message.Luna.Display;
import com.alimama.display.algo.luna.message.Luna.Label;
import com.alimama.display.algo.luna.message.Luna.Tag;
import com.alimama.display.algo.luna.message.Luna.User;

import display.algo.common.Constants;
import display.algo.logs.proto.MiddataMessage.DiamondMidData;


public class DataTransform {
	static String[] day_split;
	static String[] hour_split;
	private Calendar calendar = Calendar.getInstance();
	private static User.Builder ub = User.newBuilder();
	private static Ad.Builder adb = Ad.newBuilder();
	private static Context.Builder ctxb = Context.newBuilder();
	private static Display.Builder db = Display.newBuilder();
	private Label.Builder lbb = Label.newBuilder();
	private Tag.Builder tgb = Tag.newBuilder();
	
	private AdExtractor adExtractor;
	
	private static void clear(){
		ub.clear();
		adb.clear();
		ctxb.clear();
		db.clear();
	}
	
	
	public void init(Configuration conf) throws IOException, URISyntaxException {
		System.out.println("DataTransform init...");
		day_split = "1,0,0,0,0,0,1".split(",", -1);
		hour_split = "2,2,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2".split(",",-1);
		if (day_split.length != 7) {
			throw new RuntimeException("DAY_SPLIT Conf Error");
		}
		if (hour_split.length != 24) {
			throw new RuntimeException("HOUR_SPLIT Conf Error");
		}
		
		String adpath = conf.get(LunaConstants.AD_PATH);
		if(adpath==null){
        	throw new RuntimeException("adpath is null");
        }
        
        try {
			AdExtractor.addPath(conf, adpath);
		} catch (URISyntaxException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		adExtractor = AdExtractor.newInstance(conf);
		
		clear();
		System.out.println("DataTransform success...");
	}
	
	public int getWeekLabel(long timeStamp){
		calendar.setTimeInMillis(timeStamp * 1000);
		int DAY=calendar.get(Calendar.DAY_OF_WEEK);
		return Integer.parseInt(day_split[DAY-1]);
	}
	
	public int getTimeLabel(long timeStamp){
		calendar.setTimeInMillis(timeStamp * 1000);
		int HOUR=calendar.get(Calendar.HOUR_OF_DAY);
		return Integer.parseInt(hour_split[HOUR]);
	}
	
	public static String getPidZoonID(String pid){
		return pid.split(Constants.UNDERLINE)[3];
	}
	
	/*
	 * Context contains: pid time week url size pos
	 */
	public Context getContext(DiamondMidData dmd){
		ctxb.clear();
		display.algo.logs.proto.MiddataMessage.Publish p = dmd.getPublish();
		String pid = getPidZoonID(p.getPid());
		String url = p.getUrl();
		int time = getTimeLabel(p.getTimestamp());
		int week = getWeekLabel(p.getTimestamp());
		String size = p.getSize();
		return ctxb.setPid(pid)
				.setUrl(url).setTime(time)
				.setWeek(week).setSize(size).build();
	}
	
	public User getUser(DiamondMidData dmd){
		ub.clear();
		display.algo.logs.proto.MiddataMessage.User u = dmd.getUser();
		
		String acookie = u.getAcookie();
		String nickname = u.getNickname();
		ub.setAcookie(acookie);
		ub.setNickname(nickname);
		
		//Crowd Targeting
		lbb.clear();
		lbb.setType(8);
		for(display.algo.logs.proto.BasicMessage.Crowd c : u.getCrowdList()){
			tgb.clear();
			tgb.setId(c.getCrowdid());
			tgb.setValue(c.getBuy());
			lbb.addTags(tgb.build());
		}
		ub.addLabels(lbb.build());
		
		//Shop Targeting
		lbb.clear();
		lbb.setType(16);
		for(display.algo.logs.proto.BasicMessage.Shop s : u.getShopList()){
			tgb.clear();
			tgb.setId(s.getShopid());
			tgb.setValue(s.getScore());
			lbb.addTags(tgb.build());
		}
		ub.addLabels(lbb.build());
		
		//Interest Targeting
		lbb.clear();
		lbb.setType(64);
		for(display.algo.logs.proto.BasicMessage.Interest i : u.getInterestList()){
			tgb.clear();
			tgb.setId(i.getInterestid());
			tgb.setValue(i.getScore());
			lbb.addTags(tgb.build());
		}
		ub.addLabels(lbb.build());
		
		return ub.build();
	}
	
	
	public Ad getAd(display.algo.logs.proto.MiddataMessage.Ad ad){
		adb.clear().setAdboardId(ad.getAdboradId())
		.setTransId(ad.getTransId())
		.setMaincate(ad.getMaincateId())
		.setCustomerId(ad.getCustomerId())
		.setCustomerPrice(ad.getCustomerId())
		.setProductType(ad.getProductType());
		
		
		if (adExtractor != null) {
			Ad tmp = adExtractor.getAdInfo(ad.getTransId(), ad.getAdboradId());
			if (null == tmp) {
				System.out.println("AD_NOT_FOUND");
				return null;
			}
			adb.addAllLabels(tmp.getLabelsList());
		}
		
		return adb.build();
	}
	
	
	public Collection<Ad> getAds(DiamondMidData dmd){
		Vector<Ad> result = new Vector<Ad>();
		for(display.algo.logs.proto.MiddataMessage.Ad ad :dmd.getAdList()){
			Ad adx = getAd(ad);
			if(adx != null)
				result.add(getAd(ad));
		}
		return result;
	}
	
	public Vector<Display> getDisplays(DiamondMidData dmd){
		Vector<Display> result = new Vector<Display>();
		db.clear();
		
		db.setUser(getUser(dmd));
		db.setSessionid(dmd.getSessionid());
		Context.Builder ctxb= getContext(dmd).toBuilder();
		//System.out.println("Ads number in a record "+dmd.getAdCount());
		for(display.algo.logs.proto.MiddataMessage.Ad t :dmd.getAdList()){
			
			Ad ad = getAd(t);
			display.algo.logs.proto.MiddataMessage.AdEx adex = t.getAdex();
			db.setAd(ad);
			db.setClick(0);
			if(adex.hasClickid() && !adex.getClickid().isEmpty())
				db.setClick(1);
			db.setContext(ctxb.build());
			result.add(db.build());
		}
		return result;
	}
	
}
