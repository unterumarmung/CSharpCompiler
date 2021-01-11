namespace ComplexArray
{
    public class M
    { 
        public int a;

        public static void Main()
        {
            Main[] array = new M[10];

            int i = 0;
            while (i < array.Length)
            {
                array[i] = new M;
                i = i + 1;
            }   

            i = 0;
            while(i < array.Length)
            {
                array[i].a = i;
                 i = i + 1;
            }

            i = 0;
            while(i < array.Length)
            {
                System.Console Console = new System.Console;
                Console.WriteLine(array[i].a);
                 i = i + 1;
            }
        }
    }
}