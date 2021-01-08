namespace ComplexArray
{
    public class Main
    { 
        public int a;

        public static void Main()
        {
            Main[] array = new Main[10];

            int i = 0;
            while (i < 10)
            {
                array[i] = new Main;
                i = i + 1;
            }   

            i = 0;
            while(i < 10)
            {
                array[i].a = i;
                 i = i + 1;
            }

            i = 0;
            while(i < 10)
            {
                System.Console Console = new System.Console;
                Console.WriteLine(array[i].a);
                 i = i + 1;
            }
        }
    }
}