namespace String
{
    public class M
    {
        public static void Main()
        {
            var console = new System.Console;
            string str = "Test string";

            console.WriteLine(@"The testing string is """ + str + @"""");

            console.Write("Length: ");
            console.WriteLine(str.Length);

            // Trying to change value of str.Length
            // str.Length = 10;

            var ch = 'g';
            console.Write(@"Index of '");
            console.Write(ch);
            console.Write(@"': ");
            console.WriteLine(str.IndexOf(ch));

            var s = "str";
            console.Write(@"IndexOf """ + s + @""": ");
            console.WriteLine(str.IndexOf(s));

            int start = 5;
            console.Write(@"Substring from ");
            console.Write(start);
            console.Write(@" to end: ");
            console.WriteLine(str.Substring(start));

            int length = 5;
            console.Write(@"Substring from ");
            console.Write(start);
            console.Write(@" with length ");
            console.Write(start);
            console.Write(@": ");
            console.WriteLine(str.Substring(start));

            var index = 5;
            console.Write(@"CharAt ");
            console.Write(index);
            console.Write(@": ");
            console.WriteLine(str.CharAt(index));

            console.Write(@"String equals same string: ");
            console.WriteLine(str.Equals("Test string"));

            console.Write(@"String equals not same string: ");
            console.WriteLine(str.Equals("Hello, World!"));

            console.Write(@"operator+ with self: ");
            console.WriteLine(str + str);
            console.Write(@"operator== with self: ");
            console.WriteLine(str == str);
            console.Write(@"operator!= with self: ");
            console.WriteLine(str != str);
        }
    }
}