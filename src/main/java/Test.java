import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;


public class Test {
	public static void main(String[] args) throws IOException{
		File f = new File("a.txt");
		if(f.exists()){
			System.out.println("You Are Kidding?");
		}
		System.out.println("here");
		BufferedWriter bw = new BufferedWriter(new FileWriter("shop2cate.txt"));
		bw.close();
	}
}
