package com.test;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.Set;
import java.util.SortedSet;
import java.util.Stack;
import java.util.Vector;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;

import com.taobao.algo.ecpm.admidtier.common.AdMidTierMessage.BoardInfo;

import display.algo.common.Constants;


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
	
	public static boolean isValid(String s) {
        Stack<Character> stk = new Stack<Character>();
        for(int i = 0; i < s.length(); i++){
            char tmp = s.charAt(i);
            if(tmp == '(' || tmp == '[' || tmp == '{') stk.push(tmp);
            else{
                if(stk.isEmpty()||stk.pop() != tmp) return false;
            }
        }
        if(!stk.isEmpty()) return false;
        return true;
    }
	
	 public static int maximalRectangle(char[][] matrix) {
	        if(matrix.length==0 || matrix[0].length == 0) return 0;
	        int[] line = new int[matrix[0].length];
	        for(int i = 0; i < line.length; i++) line[i] = 0;
	        int max = 0;
	        for(int i = 0; i < matrix.length; i++){
	            Stack<Integer> stk = new Stack<Integer>();
	            for(int j = 0; j < matrix[0].length; j++){
	                if(matrix[i][j] == '1') line[j]++;
	                else line[j] = 0;
	            }
	            int[] tmpline = Arrays.copyOf(line, line.length+1);
	            int k = 0;
	            while(k < tmpline.length){
	                if(stk.isEmpty() || tmpline[k] >= tmpline[stk.peek()]) stk.push(k++);
	                else{
	                    int tmp = stk.pop();
	                    max = Math.max(max, tmpline[tmp]*(k-tmp));
	                }
	            }
	        }
	        return max;
	    }
	 public static boolean isPalindrome(int x) {
	        int y = 0;
	        while(x != 0){
	            y = y*10+(x%10);
	            x /= 10;
	        }
	        return (x==y);
	    }
	 public ArrayList<ArrayList<Integer>> subsets(int[] S) {
	        ArrayList<ArrayList<Integer>> result = new ArrayList<ArrayList<Integer>>();
	        boolean[] bool = new boolean[S.length];
	        Arrays.sort(S);
	        while(true){
	            ArrayList<Integer> subset = new ArrayList<Integer>();
	            boolean flag = true;
	            for(int i = 0; i < bool.length; i++){
	                if(bool[i]) subset.add(S[i]);
	                else flag = false;
	            }
	            result.add(subset);
	            if(flag) break;
	            boolean carry = true;
	            for(int i = bool.length-1; i>=0; i--){
	                if(bool[i] && carry) bool[i] = false;
	                else if(carry){
	                    bool[i] = true;
	                    carry = false;
	                }
	                else break;
	            }
	        }
	        
	        return result;
	    }
	 public class Node{
	        public int x;
	        public int y;
	        public Node(int x, int y){
	            this.x = x;
	            this.y = y;
	        }
	    }
	 public void solve(char[][] board) {
        if(board.length <= 2 || board[0].length <= 2) return;
        boolean[][] checked = new boolean[board.length][board[0].length];
        for(int i = 0; i < board.length; i++){
            for(int j = 0; j < board[0].length; j++){
                checked[i][j] = false;
            }
        }
        ArrayList<Node> al = new ArrayList<Node>();
        LinkedList<Node> li = new LinkedList<Node>();
        for(int i = 0; i < board.length; i++){
            for(int j = 0; j < board[0].length; j++){
                if(checked[i][j] || board[i][j] == 'X') continue;
                al.clear();
                checked[i][j] = true;
                boolean flag = true;
                li.addLast(new Node(i, j));
                while(!li.isEmpty()){
                    Node temp = li.pollFirst();
                    al.add(temp);
                    if(temp.x == 0 || temp.x == board.length-1 || temp.y == 0 || temp.y == board[0].length-1) flag = false;
                    if(temp.x > 0 && !checked[temp.x-1][temp.y] && board[temp.x-1][temp.y] == 'O') {
                        al.add(new Node(temp.x-1, temp.y));
                        checked[temp.x-1][temp.y] = true;
                        li.addLast(new Node(temp.x-1, temp.y));
                    }
                    if(temp.x < board.length-1 && !checked[temp.x+1][temp.y] && board[temp.x+1][temp.y] == 'O'){
                        al.add(new Node(temp.x+1, temp.y));
                        checked[temp.x+1][temp.y] = true;
                        li.addLast(new Node(temp.x+1, temp.y));
                    }
                    if(temp.y > 0 && !checked[temp.x][temp.y-1] && board[temp.x][temp.y-1] == 'O'){
                        al.add(new Node(temp.x, temp.y-1));
                        checked[temp.x][temp.y-1] = true;
                        li.addLast(new Node(temp.x, temp.y-1));
                    }
                    if(temp.y < board[0].length-1 && !checked[temp.x][temp.y+1] && board[temp.x][temp.y+1] == 'O'){
                        al.add(new Node(temp.x, temp.y+1));
                        checked[temp.x][temp.y+1] = true;
                        li.addLast(new Node(temp.x, temp.y+1));
                    }
                }
                if(flag){
                    for(Node n:al){
                        board[n.x][n.y] = 'X';
                    }
                }
            }
        }
    }
	 
	 public static ArrayList<Integer> spiralOrder(int[][] matrix) {
        ArrayList<Integer> result = new ArrayList<Integer>();
        int leftboard = -1;
        if(matrix == null || matrix.length <= 0) return result;
        int rightboard = matrix[0].length;
        int topboard = -1;
        int bottomboard = matrix.length;
        int posx = 0;
        int posy = -1;
        
        while(leftboard+1 < rightboard && topboard+1 < bottomboard){
            //go right to rightboard
            while(posy+1 < rightboard){
                result.add(matrix[posx][++posy]);
            }
            topboard++;
            if(posx+1 == bottomboard) break;
            //go bottom
            while(posx+1 < bottomboard){
                result.add(matrix[++posx][rightboard-1]);
            }
            rightboard--;
            if(posy - 1 == leftboard) break;
            //go left
            while(posy-1 > leftboard){
                result.add(matrix[bottomboard-1][--posy]);
            }
            bottomboard--;
            if(posx-1 <= topboard) break;
            //go top
            while(posx-1 > topboard){
                result.add(matrix[--posx][leftboard+1]);
            }
            leftboard++;
        }
        return result;
    }
	 
	 public static String multiply(String num1, String num2) {
        StringBuffer sb = new StringBuffer("");
        for(int i = 0; i< 1000; i++) sb.append(0);
        
        for(int i = num1.length()-1; i >= 0; i--){
            int index = num1.length()-1-i;
            int a = num1.charAt(i) - '0';
            int carry = 0;
            for(int j = num2.length()-1; j>=0; j--){
                int b = num2.charAt(j) - '0';
                int pos = index+num2.length()-j-1;
                int c = sb.charAt(pos) - '0';
                int value = (a*b+carry+c) % 10;
                carry = (a*b+carry+c) / 10;
                
                sb.setCharAt(pos, (char) (value + '0'));
            }
            int pos = index+num2.length();
            while(carry != 0){
                int c= sb.charAt(pos) - '0';
                int value = (carry + c) % 10;
                carry = (carry + c) / 10;
                sb.setCharAt(pos, (char)(value + '0'));
                pos++;
            }
            index++;
        }
        
        int i = 0;
        sb = sb.reverse();
        for(; i < sb.length(); i++){
            if(sb.charAt(i)-'0' != 0) break;
        }
        if(i==sb.length()) return "0";
        return sb.substring(i);
    }
	 
	  public ArrayList<ArrayList<Integer>> combinationSum(int[] candidates, int target) {
	        ArrayList<ArrayList<Integer>> result= new ArrayList<ArrayList<Integer>>();
	        if(candidates.length < 1) return result;
	        int[] tmpcandidates = new int[candidates.length];
	        for(int i = 0; i < candidates.length; i++) tmpcandidates[i] = candidates[i];
	        Arrays.sort(tmpcandidates);
	        ArrayList<Integer> candList = new ArrayList<Integer>();
	        candList.add(tmpcandidates[0]);
	        for(int i = 1; i < tmpcandidates.length; i++){
	            if(tmpcandidates[i] == tmpcandidates[i-1]) continue;
	            candList.add(tmpcandidates[i]);
	        }
	        
	        LinkedList<Integer> path = new LinkedList<Integer>();
	        combinationSumSub(candList, 0, target, path, result);
	        return result;
	    }
	    
	    public void combinationSumSub(ArrayList<Integer> candidates, int pos, int target, LinkedList<Integer> path, ArrayList<ArrayList<Integer>> result){
	        if(target == 0){
	            ArrayList<Integer> set = new ArrayList<Integer>();
	            if(path.size()==0) return;
	            for(int i = 0; i < path.size(); i++) set.add(path.get(i));
	            result.add(set);
	        }
	        else{
	            for(int i = pos; i < candidates.size(); i++){
	                if(candidates.get(i) > target) break;
	                path.addLast(candidates.get(i));
	                combinationSumSub(candidates, pos, target-candidates.get(i), path, result);
	                path.pollLast();
	            }
	        }
	    }
	    public static boolean isNumber(String s) {
	    	s = s.trim();
	        int numPoints = 0;
	        int numE = 0;
	        int numDigit = 0;
	        if(s.length() == 0) return false;
	        for(int i = 0; i < s.length(); i++){
	            char ch = s.charAt(i);
	            if(ch == '.'){
	                numPoints++;
	                if(numPoints > 1 || s.length()==1) return false;
	            }
	            else if(ch == 'e'){
	                numE++;
	                if(numE > 1 || i == s.length()-1 || i == 0) return false;
	                if(s.charAt(i-1) == '.' || s.charAt(i+1) == '.') return false;
	            }
	            else if(ch == '-' || ch == '+'){
	                if(i != 0) return false;
	            } 
	            else if(ch == ' ') return false;
	            else if(ch < '0' || ch > '9') return false;
	            numDigit++;
	        }
	        if(numDigit == 0) return false;
	        return true;
	    }
	    public static int lengthOfLongestSubstring(String s) {
	        int maxSoFar = 0;
	        if(s.length() <= 1) return s.length();
	        int i = 0;
	        int j = i+1;
	        while(j < s.length()){
	            for(int k = i;k < j; k++){
	                if(s.charAt(j) == s.charAt(k)){
	                    maxSoFar = Math.max(maxSoFar, j-i);
	                    System.out.println(i+" "+j+" "+(j-i));
	                    i = k+1;
	                    j++;
	                    break;
	                }
	            }
	            j++;
	        }
	        System.out.println(i+" "+j+" "+(j-i));
	        return Math.max(maxSoFar, j-i);
	    }
	    public static String longestPalindrome(String s) {
	        int dp[][] = new int[s.length()][s.length()];
	        int maxLength = 1;
	        int indexX = 0; int indexY = 0;
	        for(int j = 0; j < s.length(); j++){
	            for(int i = 0; i < s.length()-j; i++){
	                if(i == j) dp[i][j] = 1;
	                else if(s.charAt(i) == s.charAt(j)){
	                    if(i+1 <= j-1){
	                        dp[i][j] = dp[i+1][j-1];
	                    }
	                    else dp[j][i] = 1;
	                }
	                else{
	                    dp[j][i] = 0;
	                }
	            }
	        }
	        for(int i = 0; i < s.length(); i++){
	            for(int j = i; j < s.length(); j++){
	                if(j-i+1 > maxLength){
	                    indexY = j; indexX = i;
	                    maxLength = j-i+1;
	                }
	            }
	        }
	        return s.substring(indexX, indexY+1);
	    }
	    
	    public static ArrayList<ArrayList<Integer>> permuteUnique(int[] num) {
	        ArrayList<ArrayList<Integer>> result = new ArrayList<ArrayList<Integer>>();
	        if(num.length <= 0) return result;
	        int[] numTmp = new int[num.length];
	        boolean[] used = new boolean[num.length];
	        for(int i = 0; i < num.length; i++) {
	            numTmp[i] = num[i];
	            used[i] = false;
	        }
	        Arrays.sort(num);
	        LinkedList<Integer> path = new LinkedList<Integer>();
	        permuteHelper(num, used, result, path);
	        return result;
	    }
	    private static void permuteHelper(int[] num, boolean[] used, ArrayList<ArrayList<Integer>> result, LinkedList<Integer> path){
	        if(path.size() == num.length){
	            boolean dup = false;
	            for(int i = result.size()-1; i >=0; i--){
	                //check duplicate
	                ArrayList<Integer> tmp = result.get(i);
	                int j = 0;
	                for(; j < tmp.size(); j++){
	                    if( tmp.get(j) != path.get(j)) break;
	                }
	                if(j == tmp.size()){
	                    dup = true;
	                    break;
	                } 
	            }
	            if(!dup){
	                ArrayList<Integer> ls = new ArrayList<Integer>();
	                for(int i = 0; i < path.size(); i++) ls.add(path.get(i));
	                result.add(ls);
	            }
	        }
	        else{
	            for(int i = 0; i < num.length; i++){
	                if(!used[i]){
	                    path.addLast(num[i]);
	                    used[i] = true;
	                    permuteHelper(num, used, result, path);
	                    used[i] = false;
	                    path.pollLast();
	                }
	                while(i+1 < num.length && num[i+1] == num[i]) ++i;
	            }
	        }
	    }
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
		
		Long a = (long) 1;
		Long b = (long) 1;
		HashMap<String, Long> hm = new HashMap<String, Long>();
		hm.put("1", a);
		System.out.println(hm.containsKey(a.toString()));
		System.out.println(a.equals(b));
		
		Pattern p = Pattern.compile("(\\?|&)(item(_){0,1}(num_){0,1}){0,1}(i|I)d=[0-9]+");
		String url = "http://item.taobao.com/item.htm?item_id=17274844550&id=fasdfasdf&sucai=maozi1&kid=_318189";
		Matcher m = p.matcher(url);
	
        String temp = "";
        if(m.find()){
           temp = url.substring(m.start(), m.end());
        }
        if(temp.equals("")) return;
		String itemId = temp.split("=")[1].trim();
			if(url.contains("item") && itemId != null && !itemId.equals("")){
				System.out.println(itemId);
			} 
		*/
		//int[][] matrix = {{1,2,3,4},{1,2,3,4}};
		//System.out.println(spiralOrder(matrix));
		ArrayList<Integer> al = new ArrayList<Integer>();
		Vector<Integer> v = new Vector<Integer>();
		Stack<Character> stk = new Stack<Character>();
		
		Set<String> dict = new HashSet<String>();
		String str= "123"; long l = 1;
		double d = 0.234;
		System.out.println((int)(d*100)*1.00);
		System.out.println(lengthOfLongestSubstring("whtaciohordtqkvwcsgspqo"));
		System.out.println(multiply("11000000000000000","01111111111"));
		int[] array = {1,1};
		System.out.println((int)(char)(byte)-2);

		permuteUnique(array);
	}
}

