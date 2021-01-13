namespace Logical
{
    public class M
    {
        public System.Console Console = new System.Console;
        public bool First()
        {
            Console.WriteLine("First");
            return false;
        }

        public bool Second()
        {
            Console.WriteLine("Second");
            return true;
        }

        public static void Main()
        {
            M m = new M;
            if (m.Second() && m.First())
            {
                
            }

            if (m.Second() || m.First())
            {
                
            }
        }
    }
}