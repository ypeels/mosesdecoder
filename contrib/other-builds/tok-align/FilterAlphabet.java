import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.OutputStream;
import java.util.HashSet;

public class FilterAlphabet 
{
	HashSet<Character> alphabet = new HashSet<Character>();
	
	public static void main(String[] args) throws Exception
	{
		// TODO Auto-generated method stub
		System.err.println("Starting");
		
		BufferedReader alphaBuff = new BufferedReader(
									new InputStreamReader(
									new FileInputStream(args[0]), "UTF8")); 		
		
		
		BufferedReader inBuff = new BufferedReader(new InputStreamReader(System.in));
		BufferedWriter outBuff = new BufferedWriter(new OutputStreamWriter(System.out));

		FilterAlphabet obj = new FilterAlphabet();
		obj.LoadAlphabet(alphaBuff);
		obj.filter(inBuff, outBuff);
		 
		alphaBuff.close();
		inBuff.close();
		outBuff.close();

		System.err.println("Finished");
	}

	void LoadAlphabet(BufferedReader alphaBuff) throws Exception
	{
		String str = alphaBuff.readLine();
		for (int i = 0; i < str.length(); ++i) {
			char c = str.charAt(i);
			alphabet.add(c);
		}
	}
	
	void filter(BufferedReader inBuff, BufferedWriter outBuff) throws Exception
	{
		String str;
		while ((str = inBuff.readLine()) != null) {
			Boolean output = true;
			for (int i = 0; i < str.length(); ++i) {
				char c = str.charAt(i);
				output = alphabet.contains(c);
				if (!output) {
					break;
				}
			}
			
			if (output) {
				outBuff.write(str + "\n");
			}
		}
	}
	
}
