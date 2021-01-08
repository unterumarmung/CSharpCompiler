namespace VariableScoping
{
    public class Main
    {
        public static void Main()
        {
            System.Console Console = new System.Console;
            int number = Console.ReadInt();

            if (number < 0)
            {
                int i = -1;
                Console.WriteLine(i);
            } 
            else if (number == 0)
            {
                int i = 0;
                Console.WriteLine(i);
            }
            else 
            {
                int i = 1;
                Console.WriteLine(i);
            }
        }
    }
}