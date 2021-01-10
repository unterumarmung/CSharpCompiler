namespace Sort
{
    public class Main
    {
        public void PrintArray(int[] array)
        {
            var Console = new System.Console;
            Console.Write("[");
            int i = 0;
            while (i < array.Length)
            {
                Console.Write(array[i]);
                bool isLast = i == array.Length - 1;
                if (!isLast)
                {
                    Console.Write(", ");
                }
                i = i + 1;
            }
            Console.WriteLine("]");
        }

        public void SortArray(int[] array)
        {
            int outer = 0;
            while (outer < array.Length)
            {
                int inner = 0;
                while (inner < array.Length - 1)
                {
                    if (array[inner] > array[inner + 1]) 
					{
						int temp = array[inner];
						array[inner] = array[inner + 1];
						array[inner + 1] = temp;
					}
					inner = inner + 1;
                }
                outer = outer + 1;
            }
        }

        public static void Main()
        {
            var Console = new System.Console;

            Console.Write("Enter array length: ");
            int length = Console.ReadInt();

            int[] array = new int[length];

            int i = 0;
            while (i < array.Length)
            {
                array[i] = Console.ReadInt();
                i = i + 1;
            }
            i = 0;

            var main = new Main;

            Console.Write("Array before sorting: ");
            main.PrintArray(array);

            main.SortArray(array);
            Console.Write("Array after sorting: ");
            main.PrintArray(array);
        }
    }
}