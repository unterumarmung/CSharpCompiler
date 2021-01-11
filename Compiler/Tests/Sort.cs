namespace Sort
{
    public class M
    {
        public void PrintArray(int[] array)
        {
            var Console = new System.Console;
            Console.Write("[");
            for (int i = 0; i < array.Length; ++i)
            {
                Console.Write(array[i]);
                bool isLast = i == array.Length - 1;
                if (!isLast)
                {
                    Console.Write(", ");
                }
            }
            Console.WriteLine("]");
        }

        public void SortArray(int[] array)
        {
            for (int outer = 0; outer < array.Length; ++outer)
            {
                for (int inner = 0; inner < array.Length - 1; ++inner)
                {
                    if (array[inner] < array[inner + 1]) 
					{
						int temp = array[inner];
						array[inner] = array[inner + 1];
						array[inner + 1] = temp;
					}
                }
            }
        }

        public static void Main()
        {
            var Console = new System.Console;

            Console.Write("Enter array length: ");
            int length = Console.ReadInt();

            int[] array = new int[length];

            for (int i = 0; i < array.Length; ++i)
                array[i] = Console.ReadInt();

            var main = new M;

            Console.Write("Array before sorting: ");
            main.PrintArray(array);

            main.SortArray(array);
            Console.Write("Array after sorting: ");
            main.PrintArray(array);
        }
    }
}