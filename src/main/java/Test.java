import java.io.File;


public class Test {
	public static void main(String[] args){
		File f = new File("a.txt");
		if(f.exists()){
			System.out.println("You Are Kidding?");
		}
		System.out.println("here");
	}
}
