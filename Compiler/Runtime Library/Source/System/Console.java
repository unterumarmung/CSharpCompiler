package System;

import java.io.IOException;
import java.util.Scanner;

public class Console {
    Scanner in = new Scanner(System.in);

    //Функции печати в консоль в строку
    public void WriteLine(int value) {
        System.out.println(value);
    }

    public void WriteLine(float value) {
        System.out.println(value);
    }

    public void WriteLine(char value) {
        System.out.println(value);
    }

    public void WriteLine(String value) {
        System.out.println(value.strVal);
    }

    public void WriteLine(boolean value) {
        System.out.println(value);
    }

    //Функции чтения из консоли
    public int ReadInt() {
        return in.nextInt();
    }

    public float ReadFloat() {
        return in.nextFloat();
    }

    public char ReadChar() {
        char value = 0;
        try {
            value = (char) System.in.read();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return value;
    }

    public String ReadString() {
        return new String(in.next());
    }

    public boolean ReadBool() {
        return in.nextBoolean();
    }

    //Функции печати в консоль
    public void Write(int value) {
        System.out.print(value);
    }

    public void Write(float value) {
        System.out.print(value);
    }

    public void Write(char value) {
        System.out.print(value);
    }

    public void Write(String value) {
        System.out.print(value.strVal);
    }

    public void Write(boolean value) {
        System.out.print(value);
    }
}