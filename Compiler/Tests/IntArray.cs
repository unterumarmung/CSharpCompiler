namespace IntArray
{
    public class Main
    {
        public static void Main()
        {
            int[] array = new int[10];

            int i = 0;
            while(i < 10)
            {
                array[i] = i;
                i = i + 1;
            }

            i = 0;
            while(i < 10)
            {
                System.Console Console = new System.Console;
                Console.WriteLine(array[i]);
                i = i + 1;
            }
        }
    }
}