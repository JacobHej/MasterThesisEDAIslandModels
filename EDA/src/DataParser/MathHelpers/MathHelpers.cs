using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using DataParser.Data;

namespace DataParser.MathHelpers
{
    public static class MathHelper {
        public static double STD(IEnumerable<double> fitEvalsToOptFitness) {
            double mean = Mean(fitEvalsToOptFitness);
            return Math.Sqrt(fitEvalsToOptFitness.Sum(evalCount => Math.Pow(evalCount - mean, 2)) / fitEvalsToOptFitness.Count());
        }

        public static double Percentile(IEnumerable<double> fitEvalsToOptFitness, double percentile) {
            List<Double> fitEvalsToOptFitnessList = fitEvalsToOptFitness.ToList();
            fitEvalsToOptFitnessList.Sort();

            double index = percentile * (fitEvalsToOptFitnessList.Count + 1) / 100 - 1;

            if (index < 0) index = 0;
            int lower = (int)Math.Floor(index);
            int upper = (int)Math.Ceiling(index);
            double weight = index - Math.Floor(index);
            if (upper >= fitEvalsToOptFitnessList.Count) upper = fitEvalsToOptFitnessList.Count - 1;

            return fitEvalsToOptFitnessList[lower] * (1 - weight) + fitEvalsToOptFitnessList[upper] * weight;
        }

        public static double Median(IEnumerable<double> fitEvalsToOptFitness) {
            var fitEvalsToOptFitnessList = fitEvalsToOptFitness.ToList();
            fitEvalsToOptFitnessList.Sort();
            int n = fitEvalsToOptFitnessList.Count;
            if (n % 2 == 0) {
                return (fitEvalsToOptFitnessList[n / 2 - 1] + fitEvalsToOptFitnessList[n / 2]) / 2.0;
            } else {
                return fitEvalsToOptFitnessList[n / 2];
            }
        }

        public static double Mean(IEnumerable<double> fitEvalsToOptFitness) {
            return fitEvalsToOptFitness.Average();
        }

        //Finds max values at each index taking in an enumerable of runs
        public static int[] MaxValues(IEnumerable<IEnumerable<int>> valueslist, int maxSize) {
            int[] bests = new int[maxSize];
            for (int i = 0; i < maxSize; i++) {
                int max = 0;
                foreach (IEnumerable<int> values in valueslist) {
                    if (i >= values.Count()) {
                        max = Math.Max(values.ElementAt(values.Count() - 1), max);
                    } else {
                        max = Math.Max(values.ElementAt(i), max);
                    }
                }
                bests[i] = max;
            }
            return bests;
        }

        //Finds max values at each index taking in an enumerable of runs
        public static int[] MaxValuesNeg(IEnumerable<IEnumerable<int>> valueslist, int maxSize)
        {
            int[] bests = new int[maxSize];
            for (int i = 0; i < maxSize; i++)
            {
                int max = -1;
                foreach (IEnumerable<int> values in valueslist)
                {
                    if (i >= values.Count())
                    {
                        max = Math.Max(-1, max);
                    }
                    else
                    {
                        max = Math.Max(values.ElementAt(i), max);
                    }
                }
                bests[i] = max;
            }
            return bests;
        }

        //Finds max values at each index taking in an enumerable of runs
        public static int[] MaxValuesFit(IEnumerable<IEnumerable<int>> valueslist, int maxSize, int bestFitItr, int bestFit)
        {
            int[] bests = new int[maxSize];
            for (int i = 0; i < maxSize; i++)
            {
                int max = -1;
                foreach (IEnumerable<int> values in valueslist)
                {
                    if (i >= bestFitItr)
                    {
                        max = bestFit;
                    }
                    else
                    {
                        max = Math.Max(values.ElementAt(i), max);
                    }
                }
                bests[i] = max;
            }
            return bests;
        }

        public static int[] TotalValues(IEnumerable<IEnumerable<int>> valueslist, int maxSize) {
            int[] totalss = new int[maxSize];
            for (int i = 0; i < maxSize; i++) {
                int total = 0;
                foreach (IEnumerable<int> values in valueslist) {
                    if (i >= values.Count()) {
                        total += values.ElementAt(values.Count() - 1);
                    } else {
                        total += values.ElementAt(i);
                    }
                }
                totalss[i] = total;
            }
            return totalss;
        }

        public static List<int> MinValues(IEnumerable<AlgorithmRun> runs) {
            var worsts = runs.SelectMany(run => run.IterationSnapshots.Select((snapshot, index) => new { snapshot.WorstFitness, index }))
                .GroupBy(pair => pair.index, pair => pair.WorstFitness)
                .OrderBy(g => g.Key)
                .Select(g => g.Min())
                .ToList();
            return worsts;
        }
    }
}
