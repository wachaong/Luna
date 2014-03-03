package com.test;
import java.util.*;
public class PrimePairSet{
     public static int range = 100000000;
     public static boolean[] primes = new boolean[range];
     public static ArrayList<Integer> p = new ArrayList<Integer>();
     public static int ans = Integer.MAX_VALUE;
     public static ArrayList<Integer> list = new ArrayList<Integer>();
     public static void getPrimes(){
          for(int i = 1; i < range; i++){
               primes[i] = true;
          }
          primes[1] = false;
          for(int i = 2; i < range; i++){
               if(primes[i]){
                    if(i < 20000)
                         p.add(i);
                    long j = (long)i*(long)i;
                    while(j < range){
                         primes[(int)j] = false;
                         j = j+i;
                    }
               }
          }
     }
     
     public static boolean isPrime(int n){
          int sqrtN = (int)Math.sqrt(n);
          int i = 0;
          while(i < p.size() && p.get(i) <= sqrtN){
               if(n % p.get(i) == 0) return false;
               i++;
          }
          if(i==p.size()){
               for(i = p.get(i-1); i < sqrtN; i++){
                    if(n % i == 0) return false;
               }
          }
          
          return true;
     }

     public static boolean check(int a, int b){
          int tempb = b;
          int x = 1;
          while(tempb != 0){
               tempb /= 10;
               x *= 10;
          }
          int t = a*x + b;
          if(t < range) return primes[t];
          else return isPrime(t);
     }
     
     public static int sum(ArrayList<Integer> list){
          int sum = 0;
          for(int i = 0; i < list.size(); i++) sum += list.get(i);
          return sum;
     }
     public static void printList(ArrayList<Integer> list){
          for(int i = 0; i < list.size(); i++) System.out.print(list.get(i)+"\t");
     }
     public static void dfs(int step, int pos, int N){
          if(step == N){
               int newAns = sum(list);
               printList(list);
               System.out.println(">>>"+newAns);
               ans = Math.min(ans, newAns);
               return;
          }
          for(int i = pos + 1; i < p.size(); i++){
               boolean valid = true;
               for(int j = 0; j < list.size(); j++){
                    if(!check(p.get(i), list.get(j)) || !check(list.get(j), p.get(i))){
                         valid = false;
                         break;
                    }
               }
               if(valid){
                     if(sum(list) + p.get(i)*(N-step) >= ans) return;
                     else{
                          list.add(p.get(i));
                          dfs(step+1, i, N);
                          list.remove(list.size()-1);
                     }
               }
              
          }
     }
     public static void solve(int N){
          getPrimes();
          dfs(0, -1, N);
     }
     
     
     public static <T> boolean linkedListHasACycle(LinkedList<T> list){
         Iterator<T> fastIter = list.iterator();
         Iterator<T> slowIter = list.iterator();
         while(slowIter.hasNext() && fastIter.hasNext())
         {
              T a = slowIter.next();
              System.out.print(a + " ");
              T b = fastIter.next();
              if(fastIter.hasNext()){
            	  b = fastIter.next();
            	  System.out.println(b);
              }
              
              else return false;
              if(a == b) return true;
         }
         return false;
    }

     public static <T extends Comparable<T>> void insertionSort(T[] array){
         for(int i = 1; i < array.length; i++){
              T current = array[i];
              
              int j = i - 1;
              for(; j >= 0; j--){
            	   if(array[j].compareTo(current) < 0) break;
                   array[j+1] = array[j]; 
              }
              if(j != i-1)  array[j+1] = current;
         }
    }

    
    
    
    
    public static int minPaints(String row){
    	int red = 0;
    	int green = 0;
         for(int i = 0; i < row.length(); i++){
              if(row.charAt(i) == 'R') red++;
              else green++;
         }
         return paint(row, 0, red, green);
    }
    public static int paint(String row, int index, int red, int green){
         if(index == row.length()) return 0;
         if(row.charAt(index) == 'R'){
              // turn it and all the latter 'R' to 'G'  or   paint(row, index+1)
              return  Math.min(green, paint(row, index+1, red-1, green));
             
         }
         else{
              // turn it to Red and paint(row, index+1) or turn all latter 'R' to 'G'
              return Math.min(paint(row, index+1, red, green-1)+1, red);
         }
    }
    
    public static int countCorections(String par){
        int result = 0;
        int left = 0; 
        for(int i = 0; i < par.length(); i++){
             if(par.charAt(i) == '('){
                  left++;
             }
             else{
                  if(left == 0) result++;
                  else left--;
             }
        }
        return result+left;
   }


    public static void Test(){     
        String row = "(()(()";
        System.out.println(countCorections(row));
    }
    
     public static void main(String[] args){
    	//  long begin = System.currentTimeMillis();
        //  solve(5);
        //  long end = System.currentTimeMillis();
        //  long period = end - begin;
        //  System.out.println(ans + "\t" + period+"ms");
         /* 
          LinkedList<Integer> list = new LinkedList<Integer>();
          list.add(1);
          list.add(2);
          list.add(3);
          list.add(5);
          System.out.println(linkedListHasACycle(list));
          */
    	 Test();
     }
}

