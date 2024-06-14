using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;

namespace DataParser.Data {
    public static class DataReader {
        public static List<AlgorithmRun> ReadData(string directory) {
            return ReadData(directory, "*.txt");
        }

        public static List<AlgorithmRun> ReadData(string directory, string searchPattern)
        {
            ConcurrentBag<AlgorithmRun> runs = new ConcurrentBag<AlgorithmRun>();
            Console.WriteLine($"Reading From: {directory}");
            Parallel.ForEach(Directory.EnumerateFiles(directory, searchPattern, SearchOption.AllDirectories), fileName => {
                runs.Add(ParseDataFromFile(fileName));
            });
            return runs.ToList();
        }

        public static AlgorithmRun ParseDataFromFile(string fileName) {
            return JsonSerializer.Deserialize<AlgorithmRun>(File.ReadAllText(fileName))!;
        }
    }
}
