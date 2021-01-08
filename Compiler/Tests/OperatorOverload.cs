namespace OperatorOverload
{
    public class Fraction
    {
        private int numerator;
        private int denominator;

        public int GetNumerator()
        {
            return numerator;
        }
        
        public int GetDenominator()
        {
            return denominator;
        }

        public Fraction SetNumerator(int newNumerator)
        {
            Fraction fraction = new Fraction;
            fraction.numerator = newNumerator;
            fraction.denominator = denominator;
            return fraction;
        }

        public Fraction SetDenominator(int newDenominator)
        {
            Fraction fraction = new Fraction;
            fraction.numerator = numerator;
            fraction.denominator = newDenominator;
            return fraction;
        }

        public void WriteToConsole()
        {
            System.Console Console = new System.Console;
            Console.Write(GetNumerator());
            Console.Write("/");
            Console.WriteLine(GetDenominator());
        }

        public static Fraction operator+(Fraction lhs, Fraction rhs)
        {
            return lhs.SetNumerator(lhs.numerator * rhs.denominator + rhs.numerator * lhs.denominator).SetDenominator(lhs.denominator * rhs.denominator);
        }
        public static Fraction operator-(Fraction lhs, Fraction rhs)
        {
            Fraction rhsNegative = rhs.SetNumerator(-rhs.numerator);
            return lhs + rhsNegative;
        }
        public static Fraction operator*(Fraction lhs, Fraction rhs)
        {
            return lhs.SetNumerator(lhs.numerator * rhs.numerator).SetDenominator(lhs.denominator * rhs.denominator);
        }
        public static Fraction operator/(Fraction lhs, Fraction rhs)
        {
            return rhs.SetNumerator(lhs.numerator * rhs.denominator).SetDenominator(lhs.denominator * rhs.numerator);
        }
    }

    public class MainClass
    {
        public static void Main()
        {
            System.Console Console = new System.Console;
            Fraction fr1 = (new Fraction).SetNumerator(1).SetDenominator(2);
            Fraction fr2 = (new Fraction).SetNumerator(1).SetDenominator(3);
            Console.Write("Initial fraction 1: ");
            fr1.WriteToConsole();
            Console.Write("Initial fraction 2: ");
            fr2.WriteToConsole();
            Console.Write("Fractions after operator+: ");
            (fr1 + fr2).WriteToConsole();
            Console.Write("Fractions after operator-: ");
            (fr1 - fr2).WriteToConsole();
            Console.Write("Fractions after operator*: ");
            (fr1 * fr2).WriteToConsole();
            Console.Write("Fractions after operator/: ");
            (fr1 / fr2).WriteToConsole();
        }
    }
}