package com.alimama.display.algo.luna.extract;

import java.util.ArrayList;

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
		for(int i = 0; i < u.getLabelsCount(); i++){
			Label l = u.getLabels(i);
			result += "Label" + l.getType() + ":";
			for(int j = 0; j < l.getTagsCount(); j++){
				result += l.getTags(j).getId() +  "_" + l.getTags(j).getValue()+"==";
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
	
	

}
