package System;

public class String {
    public final int Length;
    final java.lang.String strVal;

    public String() {
        this("");
    }

    public String(java.lang.String s) {
        this.strVal = s;
        Length = s.length();
    }

    public static String __operator_plus(String lhs, String rhs) {
        return new String(lhs.strVal + rhs.strVal);
    }

    public char CharAt(int index) {
        return strVal.charAt(index);
    }

    public boolean Equals(String other) {
        return strVal.equals(other.strVal);
    }

    public String Substring(int startIndex) {
        return new String(strVal.substring(startIndex));
    }

    public String Substring(int startIndex, int length) {
        return new String(strVal.substring(startIndex, startIndex + length));
    }

    public int IndexOf(char ch) {
        return strVal.indexOf(ch);
    }

    public int IndexOf(String str) {
        return strVal.indexOf(str.strVal);
    }
}