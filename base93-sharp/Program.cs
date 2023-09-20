// See https://aka.ms/new-console-template for more information
using base93;

Console.WriteLine("Hello, World!");

CBase93 s93 = "4seCoNds In$Space";

Console.WriteLine($"ORIGINAL : {s93.Value}");

Console.WriteLine($"CONVERTED: {s93.Encoded}");

