package com.test;

import java.io.IOException;
import java.util.Arrays;
import java.util.HashMap;


public class Test {
	public static class Sudoku{
		int [][]num;
		int N;
		public Sudoku(int[][] matrix, int N){
			num = matrix;
			this.N = N;
//			for(int i = 0; i< N*N; i++){
//				for(int j = 0; j< N*N; j++){
//					//System.out.println(i+" "+j);
//					System.out.print(matrix[i][j] +" ");
//				}
//				System.out.println();
//			}
		}
		public boolean isLegalList(int[] line){
			
			Arrays.sort(line);
			if(line[0] != 1 || line[N*N-1] != N*N){
				for(int i = 0; i<line.length; i++){
					System.out.print(line[i] + " ");
				}
				System.out.println();
				return false;
			}
			else{
				for(int i =1; i <N*N; i++){
					if(line[i] == line[i-1]) return false;
				}
			}
				
			return true;
		}
		public boolean isLegal(){
			int[] line;
			//row
			for(int i = 0; i < N*N; i++){
				line = new int[N*N];
				for(int j = 0; j < N*N; j++)
					line[j] = num[i][j];
				if(!isLegalList(line)) return false;
			}
			System.out.println("here1");
			//column
			for(int j = 0; j < N*N; j++){
				line = new int[N*N];
				for(int i = 0; i < N*N; i++)
					line[i] = num[i][j];
				if(!isLegalList(line)) return false;
			}
			System.out.println("here2");
			//each cube
			for(int i = 0; i < N; i++){
				for(int j = 0; j < N; j++){
					line = new int[N*N];
					for(int ii = 0; ii < N; ii++){
						for(int jj=0; jj < N; jj++){
//							System.out.print((i*N+ii) + " " +(j*N+jj) +" " +(num[i*N+ii][j*N+jj]));
							line[ii*N+jj] = num[i*N+ii][j*N+jj];
						}
					}
					if(!isLegalList(line)) return false;
				}
			}
			return true;
		}
		
	};
	public static void main(String[] args) throws IOException{
		/*
		BufferedReader br = new BufferedReader(new FileReader("A-large.in"));
		BufferedWriter bw = new BufferedWriter(new FileWriter("A-large.out"));
		int T = Integer.parseInt(br.readLine());
		System.out.println(T);
		for(int i = 0; i< T; i++){
			int N = Integer.parseInt(br.readLine());
			System.out.println(N);
			int [][]matrix = new int[N*N][];
			for(int j = 0; j < N*N; j++){
				matrix[j] = new int[N*N];
				String[] temp= br.readLine().split(" ");	
				for(int k = 0; k < N*N; k++){
					matrix[j][k] = Integer.parseInt(temp[k]);
				}
			}
			Sudoku s = new Sudoku(matrix, N);
			if(s.isLegal()){
				bw.write("Case #"+(i+1)+": Yes\n");
			}
			else{
				bw.write("Case #"+(i+1)+": No\n");
			}
		}
		br.close();
		bw.close();
		*/
		Long a = (long) 1;
		Long b = (long) 1;
		HashMap<String, Long> hm = new HashMap<String, Long>();
		hm.put("1", a);
		System.out.println(hm.containsKey(a.toString()));
		System.out.println(a.equals(b));
		
	}
}
