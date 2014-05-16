package com.test;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Scanner;
/*
public class Main {
	public static void main(String[] args){
		Scanner scanner = new Scanner(System.in);
		int[] countNum = new int[10];
		int[] countChar = new int[26];
		while(scanner.hasNext()){
			String str = scanner.nextLine();
			int length = str.length();
			boolean flag = true;
			for(int i = 0; i < countNum.length; i++) countNum[i] = 0;
			for(int i = 0; i < countChar.length; i++) countChar[i] = 0;
			for(int i = 0; i < str.length(); i++){
				char ch = str.charAt(i);
				if(ch >= '0' && ch <= '9' ){
					countNum[ch-'0']++;
				}
				else if(ch >= 'a' && ch <= 'z'){
					countChar[ch-'a']++;
				}
				else{
					System.out.println("<invalid input string>");
					flag = false;
					break;
				}
			}
			if(flag){
				StringBuffer sb = new StringBuffer("");
				while(length > 0){
					for(int i = 0; i < 10;i++){
						if(countNum[i] > 0){
							sb.append((char)((char)('0'+i)));
							countNum[i]--;
							length--;
						}
					}
					for(int i = 0; i < 26; i++){
						if(countChar[i] > 0){
							sb.append((char)((char)('a'+i)));
							countChar[i]--;
							length--;
						}
					}
				}
				System.out.println(sb.toString());
			}
		}
		scanner.close();
	}
}
*/




/*
public class Main {
	public static void main(String[] args){
		Scanner scanner = new Scanner(System.in);
		while(scanner.hasNext()){
			String[] tmp = scanner.nextLine().split(",");
			if(tmp.length <= 0) continue;
			int[] array = new int[tmp.length];
			int[] inverCount = new int[tmp.length];
			for(int i = 0; i < tmp.length; i++){
				array[i] = Integer.parseInt(tmp[i]);
				inverCount[i] = 0;
			}
			int inv = invCount(Arrays.copyOfRange(array, 0, array.length), 0, array.length-1, inverCount);
			if(inv == 0){
				System.out.println(0);
			}
			else{
				int max = 0;
				int maxIndex = 0;
				int second = 0;
				int secondIndex = 0;
				for(int i = 0; i < array.length; i++){
					if(inverCount[i] > max){
						max = inverCount[i];
						maxIndex = i;
					}
				}
				for(int i = 0; i < array.length; i++){
					if(i != maxIndex && inverCount[i] > second){
						second = inverCount[i];
						secondIndex = i;
					}
				}
				int tmpx = array[maxIndex];
				array[maxIndex] = array[secondIndex];
				array[secondIndex] = tmpx;
				int newInv = invCount(Arrays.copyOfRange(array, 0, array.length), 0, array.length-1, inverCount);
				System.out.println(newInv);
			}
		}
		scanner.close();
	}
	
	private static int invCount(int[] arr, int begin, int end, int[] inverCount){
		if(begin >= end) return 0;
		int m = (end+begin) / 2;
		int left1 = begin;
		int right1 =m;
		int left2 = m+1;
		int right2 = end;

		return invCount(arr, left1, right1, inverCount) 
				+ invCount(arr, left2, right2, inverCount)
				+ merge(arr, left1, right1, left2, right2, inverCount);
		
	}
	private static int merge(int[] arr, int left1, int right1, int left2, int right2, int[] inverCount){
		int i = left1;int j = left2;
		int count = 0;
		int[] helper = Arrays.copyOfRange(arr, 0, arr.length);
		
		//for(int k = 0; k < arr.length; k++) System.out.println(arr[k]);
		//System.out.println(left1+" " +right1+" " + left2 +" " + right2);
		int index = left1;
		while(i <= right1 || j <= right2){
			
			if(i == right1+1){
				helper[index++] = arr[j];
				j++;
			}
			else if(j == right2+1){
				helper[index++] = arr[i];
				i++;
			}
			else if(arr[i] <= arr[j]){
				helper[index++] = arr[i];
				i++;
			}
			else{
				helper[index++] = arr[j];
				count += right1 - left1+1-i;
				j++;
				for(int k = i; k <= right1; k++){
					inverCount[k]++;
				}
				for(int k = j; k <= left2; k-- ){
					inverCount[k]++;
				}
			}
		}
		for(int t = 0; t < arr.length; t++){
			arr[t] = helper[t];
		//	System.out.println(helper[t]);
		}
		return count;
	}
	
	
}
*/