// An example that demonstrates access to environment variables.

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
				Console.Write(args[0]);
				Console.Write("=");
				Console.WriteLine(value);
			}
			else
			{
				Console.Write(args[0]);
				Console.WriteLine(" does not exist in the environment");
			}
		}
#if false
		// Doesn't compile yet: working on it - Rhys.
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
#else
		else
		{
			Console.WriteLine("Usage: getenv varname");
		}
#endif
	}
}
