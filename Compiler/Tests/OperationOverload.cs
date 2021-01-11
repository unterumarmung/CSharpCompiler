namespace OperationOverload
{
    public class Foo
    {
        public static Foo operator+(Foo lhs, Foo rhs)
        {
            var Console = new System.Console;
            Console.WriteLine("operator+(Foo, Foo)");
            return new Foo;
        }

        public static Foo operator-(Foo lhs, Foo rhs)
        {
            var Console = new System.Console;
            Console.WriteLine("operator-(Foo, Foo)");
            return new Foo;
        }

        public static Foo operator*(Foo lhs, Foo rhs)
        {
            var Console = new System.Console;
            Console.WriteLine("operator*(Foo, Foo)");
            return new Foo;
        }

        public static Foo operator/(Foo lhs, Foo rhs)
        {
            var Console = new System.Console;
            Console.WriteLine("operator/(Foo, Foo)");
            return new Foo;
        }
        
        public static Foo operator+(Foo lhs, int rhs)
        {
            var Console = new System.Console;
            Console.WriteLine("operator+(Foo, int)");
            return new Foo;
        }
        
        public static Foo operator+(int lhs, Foo rhs)
        {
            var Console = new System.Console;
            Console.WriteLine("operator+(int, Foo)");
            return new Foo;
        }

        public static bool operator<(Foo lhs, Foo rhs)
        {
            var Console = new System.Console;
            Console.WriteLine("operator<(Foo, Foo)");
            return false;
        }

        public static bool operator>(Foo lhs, Foo rhs)
        {
            var Console = new System.Console;
            Console.WriteLine("operator>(Foo, Foo)");
            return false;
        }

        public static bool operator<=(Foo lhs, Foo rhs)
        {
            var Console = new System.Console;
            Console.WriteLine("operator<=(Foo, Foo)");
            return false;
        }
        
        public static bool operator>=(Foo lhs, Foo rhs)
        {
            var Console = new System.Console;
            Console.WriteLine("operator>=(Foo, Foo)");
            return false;
        }

        public static bool operator==(Foo lhs, Foo rhs)
        {
            var Console = new System.Console;
            Console.WriteLine("operator==(Foo, Foo)");
            return false;
        }

        public static bool operator!=(Foo lhs, Foo rhs)
        {
            var Console = new System.Console;
            Console.WriteLine("operator!=(Foo, Foo)");
            return false;
        }
    }

    public class M
    {
        public static void Main()
        {
            var foo1 = new Foo;
            var foo2 = new Foo;

            var a = foo1 + foo2;
            var b = foo1 - foo2;
            var c = foo1 * foo2;
            var d = foo1 / foo2;
            var e = 1 + foo2;
            var f = foo1 + 1;
            var g = foo1 < foo2;
            var h = foo1 > foo2;
            var k = foo1 <= foo2;
            var l = foo1 >= foo2;
            var m = foo1 == foo2;
            var n = foo1 != foo2;
        }
    }
}