namespace DoWhile
{
    public class M
    {
        public static void Main()
        {
            var Console = new System.Console;

            int i = 0;
            do 
            {
                Console.WriteLine(i);
                i = i + 1;
            } while (i < 0);
        }
    }
}