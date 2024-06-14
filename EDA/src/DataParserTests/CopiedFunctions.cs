using DataParser.Data.Selection;
using DataParser.Data;
using DataParser.Maple;
using DataParser.Naming;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static System.Runtime.InteropServices.JavaScript.JSType;

namespace DataParserTests
{
    internal class CopiedFunctions
    {
        public struct InfoHolder
        {
            public InfoHolder(int fitnessEvals, int fitnessValue)
            {
                this.fitnessEvals = fitnessEvals;
                this.fitnessValue = fitnessValue;
            }

            public int fitnessEvals { get; private set; }
            public int fitnessValue { get; private set; }
        }

        public static InfoHolder recreatePoint(List<IterationSnapshot> x, int currIndex, int iteration)
        {
            int deltaItr = x[currIndex].Iteration - x[currIndex - 1].Iteration;
            int deltaFitnessEvals = x[currIndex].FitnessEvals - x[currIndex - 1].FitnessEvals;
            int deltaFitness = x[currIndex].BestFitness - x[currIndex - 1].BestFitness;
            int distanceFromPrev = iteration - x[currIndex - 1].Iteration;
            double percentDistance = distanceFromPrev / (double)deltaItr;
            var h1 = x[currIndex - 1].FitnessEvals + deltaFitnessEvals * percentDistance;
            var h2 = x[currIndex - 1].BestFitness + deltaFitness * percentDistance;
            return new InfoHolder((int)h1, (int)h2);
        }
        public static List<MapleMatrix> ParseTruncatedDataFitnessEval(List<AlgorithmRun> data, bool total, DisplayNameFormatter formatter)
        {
            List<IGrouping<Configuration, AlgorithmRun>> detailGroups = data.GroupBy(Configuration.Exclude(["run", "rank"])).ToList();
            ConcurrentBag<MapleMatrix> matrices = new ConcurrentBag<MapleMatrix>();
            //foreach (var detailGroup in detailGroups)
            //{
            Parallel.ForEach(detailGroups, detailGroup =>
            {
                //Select the longest run amongst algorithms runs of different run no and rank
                AlgorithmRun longestRun = DataSelector.SelectLongestRun(detailGroup);
                int longestRunSize = longestRun.IterationSnapshots.Count;
                var runGroups = detailGroup.GroupBy(Configuration.Include(["run"]));
                List<List<IterationSnapshot>> bestSnapshotLists = new List<List<IterationSnapshot>>();


                foreach (var runGroup in runGroups)
                {
                    var bestSnapshots = runGroup.SelectMany(x => x.IterationSnapshots.Select((y, i) => new { y, i }))
                        .GroupBy(x => x.i, x => x.y)
                        .OrderBy(x => x.Key)
                        .Select(x => {
                            IterationSnapshot snapshot = new IterationSnapshot();
                            snapshot.Iteration = x.First().Iteration;
                            snapshot.BestFitness = x.Select(y => y.BestFitness).Max();
                            snapshot.FitnessEvals = total
                            ? x.Select(x => x.FitnessEvals).Sum()
                            : x.Select(x => x.FitnessEvals).Max();
                            return snapshot;
                        }).ToList();
                    bestSnapshotLists.Add(bestSnapshots);
                }

                List<int> iterations = runGroups.SelectMany(x => x.First().IterationSnapshots.Select(y => y.Iteration)).Distinct().Order().ToList();
                int runCount = runGroups.Count();
                int[] indexes = new int[runCount];
                List<int> fevalavg = new List<int>();
                List<int> fitavg = new List<int>();

                foreach (int iteration in iterations)
                {
                    var valuesAtIteration = bestSnapshotLists.Select((x, i) => {
                        if (i == 0)
                        {
                            int f = 10;
                        }
                        int length = x.Count();
                        if (indexes[i] >= length) // if past last element
                        {
                            var h1 = x[length - 1].FitnessEvals;
                            var h2 = x[length - 1].BestFitness;
                            return new InfoHolder(h1, h2);
                        }
                        else if (iteration == x.ElementAt(indexes[i]).Iteration) // If element exists
                        {
                            var h1 = x[indexes[i]].FitnessEvals;
                            var h2 = x[indexes[i]].BestFitness;
                            indexes[i] = indexes[i] + 1;
                            return new InfoHolder(h1, h2);
                        }
                        else //We must be in between
                        {
                            return recreatePoint(x, indexes[i], iteration);
                        }

                    }).ToList();

                    fevalavg.Add((int)valuesAtIteration.Select(x => x.fitnessEvals).Average());
                    fitavg.Add((int)valuesAtIteration.Select(x => x.fitnessValue).Average());
                }

                string matrixName = detailGroup.Key.ToString() + (total ? "_TotalEvals.csv" : "_LargestEvals.csv");

                MapleMatrix matrix = new MapleMatrix(matrixName, formatter.FormatName(detailGroup.Key));
                matrix.AppendRow("iterations", iterations);
                matrix.AppendRow("fitness_evals", fevalavg);
                matrix.AppendRow("bestFitnessAvg", fitavg);
                matrices.Add(matrix);
                //};
            });
            return matrices.ToList();
        }
    }
}
