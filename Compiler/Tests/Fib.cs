namespace Fib
{
    public class Main 
    {
        public int Fib(int n)
        {
            if (n == 0)
                return 0;
            if (n == 1)
                return 1;

            return Fib(n - 1) + Fib(n - 2);
        }

        public static void Main()
        {
            var Console = new System.Console;

            Console.Write("Enter n: ");
            int n = Console.ReadInt();

            var main = new Main;
            Console.Write("Fib(n) = ");
            Console.WriteLine(main.Fib(n));
        }
    }
}