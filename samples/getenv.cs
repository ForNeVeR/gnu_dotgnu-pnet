// An example that demonstrates access to environment variables.
//
// "getenv" with no command-line arguments will print the values
// of all environment variables.
//
// "getenv" with an argument will print the value of just that
// environment variable.

using System.Collections;

public class getenv
{
	public static void Main(String[] args)
	{
		if(args.Length != 0)
		{
			String value = Environment.GetEnvironmentVariable(args[0]);
			if(value != null)
			{
				Console.WriteLine(value);
			}
			else
			{
				Console.Write(args[0]);
				Console.WriteLine(" does not exist in the environment");
			}
		}
		else
		{
			IDictionary vars = Environment.GetEnvironmentVariables();
			IDictionaryEnumerator e = vars.GetEnumerator();
			while(e.MoveNext())
			{
				Console.Write((String)(e.Key));
				Console.Write("=");
				Console.WriteLine((String)(e.Value));
			}
		}
	}
}
