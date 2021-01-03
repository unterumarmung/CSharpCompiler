package System;

import java.io.IOException;
import java.util.Scanner;

public class Console {
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
        System.out.println(value);
    }

    public void WriteLine(boolean value) {
        System.out.println(value);
    }

    //Функции чтения из консоли
    public int ReadInt() {
        Scanner in = new Scanner(System.in);
        int value = in.nextInt();
        in.close();
        return value;
    }

    public float ReadFloat() {
        Scanner in = new Scanner(System.in);
        float value = in.nextInt();
        in.close();
        return value;
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
        Scanner in = new Scanner(System.in);
        String value = in.next();
        in.close();
        return value;
    }

    public boolean ReadBool() {
        Scanner in = new Scanner(System.in);
        boolean value = in.nextBoolean();
        in.close();
        return value;
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
        System.out.print(value);
    }

    public void Write(boolean value) {
        System.out.print(value);
    }
}