namespace FieldInit
{
    public class M
    {
        public System.Console Console = new System.Console;
        public int Answer = 42;
        
        public static void Main()
        {
            var main = new M;
            main.Console.WriteLine(main.Answer);
        }
    }
}