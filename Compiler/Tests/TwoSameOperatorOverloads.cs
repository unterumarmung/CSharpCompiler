namespace OperatorOverload
{
    public class B
    {
        public static int operator+(B lhs, C rhs)
        {
            return 0;
        }
    }

    public class C
    {
        public static int operator+(B lhs, C rhs)
        {
            return 0;
        }
    }

    public class Main
    {
        public static void Main()
        {
            B var1 = new B;
            C var2 = new C;

            var1 + var2;
        }
    }
}