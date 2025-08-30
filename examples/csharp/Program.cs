using System;
using System.Runtime.InteropServices;
class Program {
    [DllImport("neon_c", CallingConvention = CallingConvention.Cdecl)]
    static extern IntPtr neon_c_version();
    static void Main(string[] args) {
        var ptr = neon_c_version();
        string ver = Marshal.PtrToStringAnsi(ptr) ?? "unknown";
        Console.WriteLine($"NEON version (C ABI): {ver}");
    }
}
