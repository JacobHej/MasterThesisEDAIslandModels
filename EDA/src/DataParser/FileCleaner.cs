using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace DataParser
{
    internal class FileCleaner
    {

        public static void CleanFiles(string path)
        {
            Parallel.ForEach(Directory.GetFiles(path, "*.txt"), fileName =>
            {
                string[] lines = File.ReadAllLines(fileName);

                string pattern = ",( |\r|\n)*(}|])";

                string cleanString = "";

                for (int i = 0; i < lines.Length - 1; i++)
                {
                    Match m1 = Regex.Match(lines[i] + lines[i + 1], pattern);
                    if (m1.Success)
                    {
                        cleanString += lines[i].Remove(m1.Index, 1);
                    }
                    else
                    {
                        cleanString += lines[i];
                    }
                }
                Match m = Regex.Match(lines[lines.Length - 1], pattern);
                if (m.Success)
                {
                    cleanString += lines[lines.Length - 1].Remove(m.Index, 1);
                }
                else
                {
                    cleanString += lines[lines.Length - 1];
                }


                File.WriteAllText(fileName, cleanString);
            });
            
        }
    }
}
