namespace FieldInit
{
    public class Main
    {
        public System.Console Console = new System.Console;
        public int Answer = 42;
        
        public static void Main()
        {
            var main = new Main;
            main.Console.WriteLine(main.Answer);
        }
    }
}