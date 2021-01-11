namespace Namespaces
{
    public class M
    {
        public static void Main()
        {
            var first = new First.Class;
            var second = new Second.Class;
            first.Print();
            second.Print();
        }
    }
}
namespace First
{
    public class Class
    {
        public void Print()
        {
            var Console = new System.Console;
            Console.WriteLine("Namespace First");
        }
    }
}
namespace Second
{
    public class Class
    {
        public void Print()
        {
            var Console = new System.Console;
            Console.WriteLine("Namespace Second");
        }
    }
}