namespace Parameters
{
    public class M
    {
		public void Method(int a)
		{
			a = 42;
		}
		
		public void Method(Foo a)
		{
			a.A = 42;
		}
		
		public void Method(int[] a)
		{
			a[0] = 42;
		}
		
        public static void Main()
        {
            var Console = new System.Console;
			int a1 = 0;
			Foo a2 = new Foo;
			int[] a3 = new int[2];
			
			Console.WriteLine(a1);
			Console.WriteLine(a2.A);
			Console.WriteLine(a3[0]);
			
			Console.WriteLine("------------");
			
			M m = new M;
			m.Method(a1);
			m.Method(a2);
			m.Method(a3);
			
			Console.WriteLine(a1);
			Console.WriteLine(a2.A);
			Console.WriteLine(a3[0]);
        }
    }
	
	public class Foo
	{
		public int A;
	}
}