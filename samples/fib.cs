
public class Fib
{
	// Get the n'th Fibonacci number by iteration,
	// where fib(0) = 0, fib(1) = 1.
	public static uint fib(uint n)
	{
		uint a = 0;
		uint b = 1;
		uint temp;
		if(n == 0)
		{
			return 0;
		}
		while(n > 0)
		{
			temp = a + b;
			a = b;
			b = temp;
			n = n - (uint)1;
		}
		return b;
	}
}
