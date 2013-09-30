package com.alimama.display.algo.luna.extract;

import java.util.ArrayList;

import com.alimama.display.algo.luna.message.Luna.Ad;
import com.alimama.display.algo.luna.message.Luna.Context;
import com.alimama.display.algo.luna.message.Luna.Label;
import com.alimama.display.algo.luna.message.Luna.User;

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
		/*
		for(int i = 0; i < u.getLabelsCount(); i++){
			Label l = u.getLabels(i);
			result += "Label" + l.getType() + ":";
			for(int j = 0; j < l.getTagsCount(); j++){
				result += l.getTags(j).getId() +  "_" + l.getTags(j).getValue()+"==";
			}
		}
		*/
		result += u.getAcookie() + "_";
		
		String nickName = u.getNickname();
		if(nickName == null || nickName.equals("")) nickName = "NULL";
		result += nickName + "_";
		
		String userId = u.getUserid();
		if(userId == null || userId.equals("")) nickName = "NULL";
		
		result += userId;
		
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
		/*
		for(int i = 0; i < ad.getLabelsCount(); i++){
			Label l = ad.getLabels(i);
			result += l.getType() + ":";
			for(int j = 0; j < l.getTagsCount(); j++){
				result += l.getTags(j).getId()+"_";
			}
		}
		*/
		result += ad.getAdboardId()+"_";
		result += ad.getTransId() +"_";
		result += ad.getCustomerId();
		
		return result;
	}
	
	public static String GetContextFeaturesStr(Context c){
		String result = "";
		
		result += c.getPid() + "_";
		result += c.getWeek() + "_";
		result += c.getTime();
		//out += display.getContext().getPid()+"_";
		//out += display.getContext().getUrl()+"_";
		
		return result;
	}
}
