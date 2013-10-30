package com.test;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;



public class MeetParty {
	public static class Rectangle{
		int x1, x2, y1, y2;
		public Rectangle(int x1, int y1, int x2, int y2){
			this.x1 = x1;
			this.x2 = x2;
			this.y1 = y1;
			this.y2 = y2;
		}
	}
	public static class Position{
		int x;
		int y;
		public Position(int x, int y){
			this.x = x;
			this.y = y;
		}
		public int getX(){
			return x;
		}
		public int getY(){
			return y;
		}
		public int getDistance(Position b){
			return Math.abs(b.x-x) + Math.abs(b.y -y);
		}
	}
	
	public static String solve(ArrayList<Position> al){
		//enum
		int distance = 0;
		int minDistance = 9999999;
		Position that = al.get(0);
		for(int i = 0; i<al.size(); i++){
			distance = 0;
			for(int j = 0; j < al.size(); j++){
				distance += al.get(i).getDistance(al.get(j));
			}
			if(distance < minDistance){
				minDistance = distance;
				that = al.get(i);
			}
		}
		String result = that.getX() + " " + that.getY() + " " + minDistance;
		return result;
	}
	public static void main(String[] args) throws IOException{
		BufferedReader br = new BufferedReader(new FileReader("B-small-practice.in"));
		BufferedWriter bw = new BufferedWriter(new FileWriter("B-small-practice.out"));
		int T = Integer.parseInt(br.readLine());
		System.out.println(T);
		for(int i = 0; i< T; i++){
			int N = Integer.parseInt(br.readLine());
			System.out.println(N);
			
			ArrayList<Position> al = new ArrayList<Position>();
			for(int j = 0; j < N; j++){
				String[] temp= br.readLine().split(" ");
				int x1 = Integer.parseInt(temp[0]);
				int y1 = Integer.parseInt(temp[1]);
				int x2 = Integer.parseInt(temp[2]);
				int y2 = Integer.parseInt(temp[3]);
				
				for(int ii = x1; ii <=x2; ii++){
					for(int jj = y1; jj <= y2; jj++){
						boolean exists = false;
						for(int kk = 0; kk < al.size(); kk++){
							if(al.get(kk).getX() == ii && al.get(kk).getY()==jj){
								exists = true;
								break;
							}
							if(exists) continue;
						}
						al.add(new Position(ii,jj));
					}
				}
			}
			
			String ans = solve(al);
			bw.write("Case #"+(i+1)+": "+ans+"\n");
		}
		br.close();
		bw.close();
	}
}
