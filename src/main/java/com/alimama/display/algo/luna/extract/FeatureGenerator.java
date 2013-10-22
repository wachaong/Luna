package com.alimama.display.algo.luna.extract;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.net.URISyntaxException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;

import org.apache.hadoop.conf.Configuration;

import com.alimama.display.algo.luna.message.Luna.Ad;
import com.alimama.display.algo.luna.message.Luna.Context;
import com.alimama.display.algo.luna.message.Luna.Display;
import com.alimama.display.algo.luna.message.Luna.Label;
import com.alimama.display.algo.luna.message.Luna.User;
import com.alimama.display.algo.luna.util.LunaConstants;

public class FeatureGenerator {
	
	
	private Map<String, Long> shop2maincate = new  HashMap<String, Long>();
	private FeatureGenerator() {
	}
	
	public static FeatureGenerator newInstance(Configuration conf) throws IOException, URISyntaxException {		

		FeatureGenerator generator = new FeatureGenerator();
		generator.init(conf);		
		return generator;
	}
	
	private void init(Configuration conf) throws IOException, URISyntaxException {
		
		System.out.println("Feature Generator init...");
		readShopId2MainCate(conf);
		System.out.println("Feature Generator init Success!");

	}

	private void readShopId2MainCate(Configuration conf) {
		System.out.println("read readShopId2MainCate...");
		String file = conf.get("shop_maincate");
		try {
			BufferedReader br = new BufferedReader(new FileReader(file));
			String line = null;
			long no = 1;
			while ((line = br.readLine()) != null) {
				line = line.trim();
				shop2maincate.put(line,no++);
			}
			br.close();
		}catch (Exception e) {
			throw new RuntimeException(e);
		}
		System.out.println("shop2maincate.size()="+shop2maincate.size());
	}
	
	/*
	 * Get User Features
	 * Input: acookie
	 * Output: ArrayList<String> 
	 */
	
	
	public static ArrayList<String> GetUserFeatures(User u){
		ArrayList<String> features = new ArrayList<String>();
		return features;
	}
	
	
	public static String GetUserFeaturesStr(User u){
		String result = "";
		result += u.getAcookie();
		//result += u.getAcookie() + "_";
		
		//String nickName = u.getNickname();
		//if(nickName == null || nickName.equals("")) nickName = "NULL";
		//result += nickName;
		
		//String userId = u.getUserid();
		//if(userId == null || userId.equals("")) userId = "NULL";
		
		//result += userId;
		
		//get targeting information
		
		for(int i = 0; i < u.getLabelsCount(); i++){
			result += "_";
			Label l = u.getLabels(i);
			result += l.getType() + ":";
			for(int j = 0; j < l.getTagsCount(); j++){
				result += l.getTags(j).getId() +  "/" + l.getTags(j).getValue();
			}
		}
		
		return result;
	}
	
	/*
	 * Get Ad Features
	 * Input: aboardId
	 * Output: ArrayList<String>
	 */
	
	public static ArrayList<String> GetAdFeatures(String aboardId){
		ArrayList<String> features = new ArrayList<String>();
		return features;
	}
	
	
	public static String GetAdFeaturesStr(Ad ad){
		String result = "";
		
		for(int i = 0; i < ad.getLabelsCount(); i++){
			Label l = ad.getLabels(i);
			result += l.getType() + ":";
			for(int j = 0; j < l.getTagsCount(); j++){
				result += l.getTags(j).getId()+"_";
			}
		}
		/*
		result += ad.getAdboardId()+"_";
		result += ad.getTransId() +"_";
		result += ad.getCustomerId() +"_";
		result += ad.getMaincate() + "_";
		result += ad.getProductType();
		*/
		
		return result;
	}
	
	public static String GetContextFeaturesStr(Context c){
		String result = "";
		String pid = "";
		if(c.getPid().endsWith("777")){
			pid = "0";
		}
		else if(c.getPid().endsWith("778")){
			pid = "1";
		}
		else{
			pid = "2";
		}
		result += pid + "_";
		result += c.getWeek() + "_";
		result += c.getTime();
		//out += display.getContext().getPid()+"_";
		//out += display.getContext().getUrl()+"_";
		
		return result;
	}

	public ArrayList<String> calculateMatchFeature(User u,Ad a, org.apache.hadoop.mapreduce.Mapper.Context context){
		Set<String> userinfoSet = new HashSet<String>();
		Set<String> adinfoSet = new HashSet<String>();
		ArrayList<String> result = new ArrayList<String>();
		for(int i = 0; i < u.getLabelsCount(); i++){
			Label l = u.getLabels(i);
			if(l.getType() == 8){
				for(int j = 0; j < l.getTagsCount(); j++)
					userinfoSet.add(LunaConstants.CROWDPOWER_PREFIX + l.getTags(j).getId());
			}
			else if(l.getType() == 16){
				for(int j = 0; j < l.getTagsCount(); j++){
					//get maincate of the shop
					long shopid = l.getTags(j).getId();
					if(!shop2maincate.containsKey(shopid)) continue;
					Long maincate = shop2maincate.get(shopid);
					userinfoSet.add(LunaConstants.SHOPTARGETING_PREFIX + maincate);
				}
					
			}
			else if(l.getType() == 64){
				for(int j = 0; j < l.getTagsCount(); j++)
					userinfoSet.add(LunaConstants.INTEREST_PREFIX + l.getTags(j).getId());
			}
		}
		
		for(int i = 0; i < a.getLabelsCount(); i++){
			Label l = a.getLabels(i);
			if(l.getType() == 8){
				for(int j = 0; j < l.getTagsCount(); j++)
					adinfoSet.add(LunaConstants.CROWDPOWER_PREFIX + l.getTags(j).getId());
			}
			else if(l.getType() == 16){
				for(int j = 0; j < l.getTagsCount(); j++){
					//get maincate of the shop
					long shopid = l.getTags(j).getId();
					if(!shop2maincate.containsKey(shopid)) continue;
					Long maincate = shop2maincate.get(shopid);
					adinfoSet.add(LunaConstants.SHOPTARGETING_PREFIX + maincate);
				}
					
			}
			else if(l.getType() == 64){
				for(int j = 0; j < l.getTagsCount(); j++)
					adinfoSet.add(LunaConstants.INTEREST_PREFIX + l.getTags(j).getId());
			}
		}
		
		for(Iterator<String> it=adinfoSet.iterator();it.hasNext();){
			String adinfo = it.next();
			if(userinfoSet.contains(adinfo)){
				result.add(adinfo);
				if(adinfo.contains(LunaConstants.CROWDPOWER_PREFIX))
					context.getCounter("match_type", "CROWDPOWER").increment(1);
				else if(adinfo.contains(LunaConstants.SHOPTARGETING_PREFIX))
					context.getCounter("match_type", "SHOPTARGETING").increment(1);
				else if(adinfo.contains(LunaConstants.INTEREST_PREFIX))
					context.getCounter("match_type", "INTEREST").increment(1);
			}
		}
		
		return result;
	}
	
	public  ArrayList<String> getAllFeatures(Display display, org.apache.hadoop.mapreduce.Mapper.Context context) {
		ArrayList<String> allFeatures = new ArrayList<String>();
		
		Context c = display.getContext();
		User u = display.getUser();
		Ad a = display.getAd();
		
		//allFeatures.add(LunaConstants.CONTEXT_PID_PREFIX + c.getPid());
		//allFeatures.add(LunaConstants.CONTEXT_WEEK_PREFIX + c.getWeek());
		//allFeatures.add(LunaConstants.CONTEXT_TIME_PREFIX + c.getTime());
		
		//allFeatures.add(LunaConstants.ADID_PREFIX + a.getAdboardId());
		//allFeatures.add(LunaConstants.TRANSID_PREFIX + a.getTransId());
		//allFeatures.add(LunaConstants.CUSTOMERID_PREFIX + a.getCustomerId());
		//allFeatures.add(LunaConstants.MAINCATE_PREFIX + a.getMaincate());
		//allFeatures.add(LunaConstants.PRODUCTTYPE_PREFIX + a.getProductType());
		
		//allFeatures.add(LunaConstants.ACOOKIE_PREFIX + u.getAcookie());
		//allFeatures.add(LunaConstants.NICKNAME_PREFIX + u.getNickname());
		
		
		//Targetting Information
		
		for(int i = 0; i < u.getLabelsCount(); i++){
			Label l = u.getLabels(i);
			if(l.getType() == 8){
				//context.getCounter("USER_LABLE_CROWDPOWER_CNT", String.valueOf(l.getTagsCount())).increment(1);
				for(int j = 0; j < l.getTagsCount(); j++)
					allFeatures.add(LunaConstants.USER_CROWDPOWER_PREFIX + l.getTags(j).getId());
			}
			else if(l.getType() == 16){
				//context.getCounter("USER_LABLE_SHOP_CNT", String.valueOf(l.getTagsCount())).increment(1);
				for(int j = 0; j < l.getTagsCount(); j++){
					//get maincate of the shop
					long shopid = l.getTags(j).getId();
					if(!shop2maincate.containsKey(shopid)) continue;
					Long maincate = shop2maincate.get(shopid);
					allFeatures.add(LunaConstants.USER_SHOPTARGETING_PREFIX + maincate);
				}
					
			}
			else if(l.getType() == 64){
				//context.getCounter("USER_LABLE_INTEREST_CNT", String.valueOf(l.getTagsCount())).increment(1);
				for(int j = 0; j < l.getTagsCount(); j++)
					allFeatures.add(LunaConstants.USER_INTEREST_PREFIX + l.getTags(j).getId());
			}
		}
		//if(allFeatures.size() < 5) return null;
		//int num = 0;
		for(int i = 0; i < a.getLabelsCount(); i++){
			Label l = a.getLabels(i);
			if(l.getType() == 8){
				//context.getCounter("SHOP_LABLE_CROWDPOWER_CNT", String.valueOf(l.getTagsCount())).increment(1);
				//num += l.getTagsCount();
				for(int j = 0; j < l.getTagsCount(); j++)
					allFeatures.add(LunaConstants.AD_CROWDPOWER_PREFIX + l.getTags(j).getId());
			}
			else if(l.getType() == 16){
				//context.getCounter("SHOP_LABLE_SHOP_CNT", String.valueOf(l.getTagsCount())).increment(1);
				//num += l.getTagsCount();
				for(int j = 0; j < l.getTagsCount(); j++){
					//get maincate of the shop
					long shopid = l.getTags(j).getId();
					if(!shop2maincate.containsKey(shopid)) continue;
					Long maincate = shop2maincate.get(shopid);
					allFeatures.add(LunaConstants.AD_SHOPTARGETING_PREFIX + maincate);
				}
					
			}
			else if(l.getType() == 64){
				//context.getCounter("SHOP_LABLE_INTEREST_CNT", String.valueOf(l.getTagsCount())).increment(1);
				//num += l.getTagsCount();
				for(int j = 0; j < l.getTagsCount(); j++)
					allFeatures.add(LunaConstants.AD_INTEREST_PREFIX + l.getTags(j).getId());
			}
		}
		//if(num < 5) return null;
		/*
		
		
		
		ArrayList<String> matchFeatures = calculateMatchFeature(u, a, context);
		for(int i = 0; i < matchFeatures.size(); i++){
			allFeatures.add(matchFeatures.get(i));
		}
		*/
		//allFeatures.add();
		
		return allFeatures;
	}
}
