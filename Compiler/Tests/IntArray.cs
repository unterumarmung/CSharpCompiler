namespace IntArray
{
    public class Main
    {
        public static void Main()
        {
            System.Console Console = new System.Console;
            int[] array = new int[10];

            for (int i = 0; i < array.Length; ++i)
            {
                array[i] = i;
            }

            for (int i = 0; i < array.Length; ++i)
            {
                Console.WriteLine(array[i]);
            }
        }
    }
}