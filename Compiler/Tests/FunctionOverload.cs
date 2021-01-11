namespace FunctionOverload
{
    public class M
    {
        public System.Console Console = new System.Console;
        public void Method(int a)
        {
            Console.WriteLine("Called with int");
        }
        public void Method(int[] a)
        {
            Console.WriteLine("Called with int[]");
        }
        public void Method(M a)
        {
            Console.WriteLine("Called with M");
        }
        public static void Main()
        {
            var m = new M;
            m.Method(0);
            m.Method(new int[1]);
            m.Method(new M);
        }
    }
}