package com.alimama.display.algo.luna.extract;

import java.util.ArrayList;

import com.alimama.display.algo.luna.message.Luna.Ad;
import com.alimama.display.algo.luna.message.Luna.Context;
import com.alimama.display.algo.luna.message.Luna.Display;
import com.alimama.display.algo.luna.message.Luna.Label;
import com.alimama.display.algo.luna.message.Luna.User;
import com.alimama.display.algo.luna.util.LunaConstants;

public class FeatureGenerator {
	
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


	public static ArrayList<String> getAllFeatures(Display display) {
		ArrayList<String> allFeatures = new ArrayList<String>();
		
		Context c = display.getContext();
		User u = display.getUser();
		Ad a = display.getAd();
		
		allFeatures.add(LunaConstants.CONTEXT_PREFIX + c.getPid());
		allFeatures.add(LunaConstants.CONTEXT_PREFIX + c.getWeek());
		allFeatures.add(LunaConstants.CONTEXT_PREFIX + c.getTime());
		
		//allFeatures.add();
		
		
		
		return null;
	}
}
