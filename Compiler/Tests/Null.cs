namespace Null
{
    public class M
    {
        public void Foo()
        {
            var Console = new System.Console;
            Console.WriteLine("Foo");
        }
        public static void Main()
        {
            M m = new M;
            m.Foo();
            m = null;
            m.Foo();
        }
    }
}