using DataParser.Data;
using DataParser.Data.Selection;
using DataParser.Latex;
using DataParser.Maple;
using DataParser.MathHelpers;
using DataParser.Naming;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Reflection.Metadata.Ecma335;
using System.Runtime.Intrinsics.X86;
using System.Runtime.Serialization;
using System.Text.RegularExpressions;
using System.Xml.Linq;
using static DataParser.Data.Data;

namespace DataParser {

    public class Parser {

        public static FitnessEvalAndValuePair recreatePoint(List<IterationSnapshot> x, int currIndex, int iteration)
        {
            int deltaItr = x[currIndex].Iteration - x[currIndex - 1].Iteration;
            int deltaFitnessEvals = x[currIndex].FitnessEvals - x[currIndex - 1].FitnessEvals;
            int deltaFitness = x[currIndex].BestFitness - x[currIndex - 1].BestFitness;
            int distanceFromPrev = iteration - x[currIndex - 1].Iteration;
            double percentDistance = distanceFromPrev / (double)deltaItr;
            var h1 = x[currIndex - 1].FitnessEvals + deltaFitnessEvals * percentDistance;
            var h2 = x[currIndex - 1].BestFitness + deltaFitness * percentDistance;
            return new FitnessEvalAndValuePair((int)h1, (int)h2);
        }

        public static IterationAndProbBoundCountPair RecreateProbPoint(List<(int, double)> x, int currIndex, int iteration) {
            int deltaItr = x[currIndex].Item1 - x[currIndex - 1].Item1;
            double delta_prob_bounds_count = x[currIndex].Item2 - x[currIndex - 1].Item2;

            int distanceFromPrev = iteration - x[currIndex - 1].Item1;
            double percentDistance = distanceFromPrev / (double)deltaItr;
            var h1 = x[currIndex - 1].Item2 + delta_prob_bounds_count * percentDistance;

            return new IterationAndProbBoundCountPair(iteration, h1);
        }

        public static FitnessEvalAndBoundCountPair RecreateFitEvalAndBoundPoint(List<(int, double, double)> x, int currIndex, int iteration)
        {
            int deltaItr = x[currIndex].Item1 - x[currIndex - 1].Item1;
            double deltaFitnessEvals = x[currIndex].Item2 - x[currIndex - 1].Item2;
            double delta_prob_bounds_count = x[currIndex].Item3 - x[currIndex - 1].Item3;

            int distanceFromPrev = iteration - x[currIndex - 1].Item1;
            double percentDistance = distanceFromPrev / (double)deltaItr;
            var h1 = x[currIndex - 1].Item2 + deltaFitnessEvals * percentDistance;
            var h2 = x[currIndex - 1].Item3 + delta_prob_bounds_count * percentDistance;

            return new FitnessEvalAndBoundCountPair(h1, h2);
        }

        //private static string staticbasePath = "C:\\Users\\erikv\\OneDrive\\Skrivebord\\MASTERS SPECIALE\\MasterEDA\\";

        private static string staticbasePath = "D:/Workspaces/MasterEDA2/";
        private static string staticasePathIn = "ResultsNewHello/";
        private static string staticbasePathOut = "CSVResultsResultsNewHello/";

        private string basePath = staticbasePath;
        private string basePathIn = staticbasePath + staticasePathIn;
        private string basePathOut = staticbasePath + staticbasePathOut;
        private static readonly string rawDataPath = staticbasePath + staticbasePathOut + "RawData.txt";

        private readonly List<AlgorithmRun> data;
        private readonly string testName;

        public Parser(string testName)
        {
            data = DataReader.ReadData($"{basePathIn}{testName}");
            this.testName = testName;
        }

        public Parser(string testName, string folder)
        {
            basePath = staticbasePath + folder;
            basePathIn = basePath + staticasePathIn;
            basePathOut = basePath + staticbasePathOut;
            data = DataReader.ReadData($"{basePathIn}{testName}");
            DataWriter.AppendText(rawDataPath, $"\n\n#Entry from {testName} [{DateTime.Now}]\n");
            this.testName = testName;
        }

        public Parser(string testName, string folder, string searchPattern)
        {
            basePath = staticbasePath + folder;
            basePathIn = basePath + staticasePathIn;
            basePathOut = basePath + staticbasePathOut;
            data = DataReader.ReadData($"{basePathIn}{testName}", searchPattern);
            DataWriter.AppendText(rawDataPath, $"\n\n#Entry from {testName} [{DateTime.Now}]\n");
            this.testName = testName;
        }

        static Dictionary<string,int> MaxSatClausCount = new Dictionary<string, int>{
            {"20", 91}, {"50", 218}, {"75", 325}, {"100", 430}, {"125", 538}, 
            {"150", 645}, {"175", 753}, {"200", 860}, {"225", 960}, {"250", 1065} 
        };

        public void ParseMaxSatLatexData(DisplayNameFormatter formatter, List<string> instances, int maxConfig, DisplayNameFormatter Rformatter = null) {
            DataWriter.AppendText(rawDataPath, $"\n\n #{new string('-', 100)} \n #Entry from {testName.Replace("/", "-")} [{DateTime.Now}]\n #{new string('-', 100)} \n\n");
            IEnumerable<IGrouping<Configuration, AlgorithmRun>> nGroups = data.GroupBy(Configuration.Include(["n"]));

            foreach (IGrouping<Configuration, AlgorithmRun> nGroup in nGroups) {
                if (!MaxSatClausCount.TryGetValue(nGroup.Key.Vars.Single(x => x.VariableName == "n").VariableValue, out int clauseCount)) {
                    throw new Exception("Could not find clause count for literal count");
                }

                ConcurrentBag<JoinedTable> instanceSummaries = new ConcurrentBag<JoinedTable>();

                foreach (IGrouping<Configuration, AlgorithmRun> instanceGroup in nGroup.GroupBy(Configuration.Include(["instance"]))) {
                    if(!instances.Any(instance => instance == instanceGroup.Key.Vars.Single(v => v.VariableName == "instance").VariableValue)) {
                        continue;
                    }

                    ConcurrentBag<(SummaryCollection, int)> detailSummaries = new ConcurrentBag<(SummaryCollection, int)>();
                    int j = 0;
                    int runAmountInstanceGroup = instanceGroup.Count();

                    List<double> instanceLargestFitEvals = new List<double>();
                    List<double> instanceTotalFitEvals = new List<double>();
                    List<double> instanceLowestFitItrs = new List<double>();
                    List<(int,string)> SolvedRatio = new List<(int, string)>();

                    foreach (IGrouping<Configuration, AlgorithmRun> detailGroup in instanceGroup.GroupBy(Configuration.Exclude(["run", "instance", "n", "rank"]))) {
                        int k = 0;

                        List<double> detailLargestFitEvals = new List<double>();
                        List<double> detailTotalFitEvals = new List<double>();
                        List<double> detailLowestFitItrs = new List<double>();
                        var runGroups = detailGroup.GroupBy(Configuration.Include(["run"]));
                        double solvedRunCount = runGroups.Where(runGroup => runGroup.Any(run => run.BestFit == clauseCount)).Count();
                        double totalRuns = runGroups.Count();

                        foreach (IGrouping<Configuration, AlgorithmRun> runGroup in runGroups) {
                            var solvedRuns = runGroup.Where(run => run.BestFit == clauseCount);
                            if (solvedRuns.Any()) { // cluster solved the problem
                                k++;
                                var fastestSolvedRun = solvedRuns.MinBy(run => run.BestFitItr);
                                int largest = runGroup.Select(run => run.IterationSnapshots.Single(i => i.Iteration == fastestSolvedRun.BestFitItr).FitnessEvals).Max();
                                int total = runGroup.Select(run => run.IterationSnapshots.Single(i => i.Iteration == fastestSolvedRun.BestFitItr).FitnessEvals).Sum();
                                detailLargestFitEvals.Add(largest);
                                detailTotalFitEvals.Add(total);
                                detailLowestFitItrs.Add(fastestSolvedRun.BestFitItr);
                            }
                        }

                        int solvedCount = k;
                        int runCount = runGroups.Count();
                        string mi = detailGroup.Key.Vars.Single(v => v.VariableName == "mi").VariableValue;
                        SolvedRatio.Add((int.Parse(mi), $"[{mi}, {solvedCount}/{runCount}]"));

                        string detailName = formatter.FormatName(detailGroup.Key) + "Solved: " + k + "/" + detailGroup.GroupBy(Configuration.Include(["run"])).Count();



                        string chunkyfy(List<double> numbers, int chunkSize) {

                            return string.Join(",\n", numbers.Select((x, i) => new { x, i }).GroupBy(x => x.i / chunkSize).Select(g => string.Join(",", g.Select(v => v.x))));
                        }

                        if (Rformatter != null) {
                            string rawData = $"#--- ---{nGroup.Key} {instanceGroup.Key} {detailGroup.Key}: \n" + $"{Rformatter.FormatName(detailGroup.Key)}_Instance_{instanceGroup.Key.Vars.Single(v => v.VariableName == "instance").VariableValue}_LargestFitness <- c({chunkyfy(detailLargestFitEvals, 100)}) \n";
                            DataWriter.AppendText(rawDataPath, rawData);
                        }

                        if (k == 0) { // none solved
                            var detailCollection = new SummaryCollection(new StatisticsSummary("None", 0, 0, 0, 0, 0), detailName);
                            detailSummaries.Add((detailCollection, 0));
                        } else {
                            var detailCollection = new SummaryCollection(CreateStatisticsSummary("Largest FitEval", detailLargestFitEvals), detailName);
                            detailCollection.Add(CreateStatisticsSummary("Total FitEval", detailTotalFitEvals));
                            detailCollection.Add(CreateStatisticsSummary("Iterations", detailLowestFitItrs));
                            detailSummaries.Add((detailCollection, k));
                        }
                    }

                    Console.WriteLine($"\n\n{instanceGroup.Key}");
                    Console.WriteLine($"plot([{string.Join(",", SolvedRatio.OrderBy(e => e.Item1).Select(e => e.Item2))}], labels=[\"MI\", \"Success Rate\"], legend=\"{formatter.Description} {instanceGroup.Key.ToString().Replace("_", ": ")}\")\n\n");

                    var selection = detailSummaries.OrderByDescending(ds => ds.Item2).ThenBy(ds => ds.Item1.Primary.GetMean()).Take(maxConfig).Select(ds => ds.Item1).ToList();
                    instanceSummaries.Add(
                        new JoinedTable(
                            $"Instance: {instanceGroup.Key.Vars.Single(v => v.VariableName == "instance").VariableValue}", 
                            selection));
                }

                string instancesTable = TableGenerator.GenerateLatexTable(instanceSummaries);
                string instancesCommandsFile = $"{basePathOut}\\" + testName + "\\" + nGroup.Key + ".txt";
                DataWriter.WriteText(instancesCommandsFile, instancesTable);
            }
        }

        public void parseMaxSatConst(DisplayNameFormatter formatter, string constName = "lambda") {
            List<IGrouping<Configuration, AlgorithmRun>> detailLambdaGroups = data.GroupBy(Configuration.Exclude(["mu", constName, "run", "rank"])).ToList();
            ConcurrentBag<MapleMatrix> largestMatrices = new ConcurrentBag<MapleMatrix>();
            ConcurrentBag<MapleMatrix> totalMatrices = new ConcurrentBag<MapleMatrix>();

            //Parallel.ForEach(detailLambdaGroups, detailLambdaGroup => {
            foreach(var detailLambdaGroup in detailLambdaGroups) { 
            List<double> lambdas = new List<double>();
                List<double> nTotalFitEvalAvg = new List<double>();
                List<double> nLargestFitEvalAvg = new List<double>();

                if (!MaxSatClausCount.TryGetValue(detailLambdaGroup.Key.Vars.Single(x => x.VariableName == "n").VariableValue, out int optFitness)) {
                    throw new Exception("Could not find clause count for literal count");
                }
                var lambdaGroups = detailLambdaGroup.GroupBy(Configuration.Include(constName)).ToList();
                foreach (var lambdaGroup in lambdaGroups) {

                    List<double> largestFitEvals = new List<double>();
                    List<double> totalFitEvals = new List<double>();

                    var runGroups = lambdaGroup.GroupBy(Configuration.Include("run")).ToList();
                    double solvedRunCount = runGroups.Where(runGroup => runGroup.Any(run => run.BestFit == optFitness)).Count();
                    double totalRuns = runGroups.Count();
                    if ((solvedRunCount / totalRuns) < 0.5) {
                        Console.WriteLine($"Ignored: {detailLambdaGroup.Key} Only {solvedRunCount}/{totalRuns}");
                        continue;
                    }

                    foreach (var runGroup in runGroups) {
                        var solvedRuns = runGroup.Where(run => run.BestFit == optFitness);
                        if (!solvedRuns.Any()) continue;

                        var fastestSolvedNode = solvedRuns.MinBy(run => run.BestFitItr);

                        int largest = runGroup.Select(run => run.IterationSnapshots.Single(i => i.Iteration == fastestSolvedNode.BestFitItr).FitnessEvals).Max();
                        int total = runGroup.Select(run => run.IterationSnapshots.Single(i => i.Iteration == fastestSolvedNode.BestFitItr).FitnessEvals).Sum();
                        largestFitEvals.Add(largest);
                        totalFitEvals.Add(total);
                    }
                    if (totalFitEvals.Count() < 1) continue;
                    lambdas.Add(int.Parse(lambdaGroup.Key.Vars.Single(x => x.VariableName == constName).VariableValue));
                    nTotalFitEvalAvg.Add(totalFitEvals.Average());
                    nLargestFitEvalAvg.Add(largestFitEvals.Average());
                }

                foreach ((bool, List<double>) list in new List<(bool, List<double>)> { (true, nTotalFitEvalAvg), (false, nLargestFitEvalAvg) }) {
                    var sorted = lambdas.Select((x, i) => new { N = x, FitEvalAvg = list.Item2[i] }).OrderBy(x => x.N);
                    string matrixName = detailLambdaGroup.Key.ToString() + (list.Item1 ? "_TotalEvals.csv" : "_LargestEvals.csv");

                    MapleMatrix matrix = new MapleMatrix(matrixName, formatter.FormatName(detailLambdaGroup.Key));
                    matrix.AppendRow(constName, sorted.Select(x => x.N));
                    matrix.AppendRow("FitEvalAvg", sorted.Select(x => x.FitEvalAvg));

                    if (list.Item1) totalMatrices.Add(matrix);
                    else largestMatrices.Add(matrix);
                }
            }//);

            WriteMatrices(totalMatrices);
            WriteMatrices(largestMatrices);
            WriteMapleCommands("MapleCommandsTotalFE_N.txt", totalMatrices, constName, "fitnessEvaluationsTotal", 1, 2);
            WriteMapleCommands("MapleCommandsLargestFE_N.txt", largestMatrices, constName, "fitnessEvaluationsMax", 1, 2);

        }

        public void ParseDataOverN(DisplayNameFormatter formatter) {
            IEnumerable<IGrouping<Configuration, AlgorithmRun>> detailnGroups = DataSelector.GroupByDetailsWithoutN(data);
            ConcurrentBag<MapleMatrix> largestMatrices = new ConcurrentBag<MapleMatrix>();
            ConcurrentBag<MapleMatrix> totalMatrices = new ConcurrentBag<MapleMatrix>();

            foreach (var detailnGroup in detailnGroups)
            {
                //Parallel.ForEach(detailnGroups, detailnGroup => {
                List<double> ns = new List<double>();
                List<double> nTotalFitEvalAvg = new List<double>();
                List<double> nLargestFitEvalAvg = new List<double>();
                var nGroups = DataSelector.GroupByN(detailnGroup);

                foreach (var nGroup in nGroups) {
                    var optFitness = int.Parse(nGroup.Key.Vars.Single(x => x.VariableName == "n").VariableValue); // Works for all but MaxSat :-) (OneMax, LeadingOnes, JumpK, TwoPeaks)
                    List<double> largestFitEvals = new List<double>();
                    List<double> totalFitEvals = new List<double>();

                    foreach (var runGroup in DataSelector.GroupByRun(nGroup)) {
                        var solvedRuns = runGroup.Where(run => run.BestFit == optFitness);
                        if (!solvedRuns.Any()) continue;

                        var fastestSolvedNode = solvedRuns.MinBy(run => run.BestFitItr);

                        int largest = runGroup.Select(run => run.IterationSnapshots.Single(i => i.Iteration == fastestSolvedNode.BestFitItr).FitnessEvals).Max();
                        int total = runGroup.Select(run => run.IterationSnapshots.Single(i => i.Iteration == fastestSolvedNode.BestFitItr).FitnessEvals).Sum();
                        largestFitEvals.Add(largest);
                        totalFitEvals.Add(total);
                    }

                    ns.Add(optFitness);
                    nTotalFitEvalAvg.Add(totalFitEvals.Average());
                    nLargestFitEvalAvg.Add(largestFitEvals.Average());
                }

                foreach ((bool, List<double>) list in new List<(bool, List<double>)> { (true, nTotalFitEvalAvg), (false, nLargestFitEvalAvg) }) {
                    var sorted = ns.Select((x, i) => new { N = x, FitEvalAvg = list.Item2[i] }).OrderBy(x => x.N);
                    string matrixName = detailnGroup.Key.ToString() + (list.Item1 ? "_TotalEvals.csv" : "_LargestEvals.csv");

                    MapleMatrix matrix = new MapleMatrix(matrixName, formatter.FormatName(detailnGroup.Key));
                    matrix.AppendRow("nValues", sorted.Select(x => x.N));
                    matrix.AppendRow("FitEvalAvg", sorted.Select(x => x.FitEvalAvg));

                    if (list.Item1) totalMatrices.Add(matrix);
                    else largestMatrices.Add(matrix);
                }
            //});
            }

            WriteMatrices(totalMatrices);
            WriteMatrices(largestMatrices);
            WriteMapleCommands("MapleCommandsTotalFE_N.txt", totalMatrices, "n", "fitnessEvaluationsTotal" , 1, 2);
            WriteMapleCommands("MapleCommandsLargestFE_N.txt", largestMatrices, "n", "fitnessEvaluationsMax", 1, 2);
        }

        public void ParseTruncatedDataFitnessEval(bool total, DisplayNameFormatter formatter)
        {
            IEnumerable<IGrouping<Configuration, AlgorithmRun>> detailGroups = data.GroupBy(Configuration.Exclude(["run", "rank"]));
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
                    var bestSnapshots = runGroup.SelectMany(run => run.IterationSnapshots.Select((snapshot, index) => new { snapshot, index }))
                        .GroupBy(pair => pair.index, pair => pair.snapshot)
                        .OrderBy(pair => pair.Key)
                        .Select(snapshotGroup => {
                            IterationSnapshot snapshot = new IterationSnapshot();
                            snapshot.Iteration = snapshotGroup.First().Iteration;
                            snapshot.BestFitness = snapshotGroup.Select(snapshotx => snapshotx.BestFitness).Max();
                            snapshot.FitnessEvals = total
                            ? snapshotGroup.Select(x => x.FitnessEvals).Sum()
                            : snapshotGroup.Select(x => x.FitnessEvals).Max();
                            return snapshot;
                        }).ToList();
                    bestSnapshotLists.Add(bestSnapshots);
                }

                List<int> iterations = runGroups.SelectMany(runGroup => runGroup.First().IterationSnapshots.Select(snapshot => snapshot.Iteration)).Distinct().Order().ToList();
                int runCount = runGroups.Count();
                int[] indexes = new int[runCount];
                List<double> fevalavg = new List<double>();
                List<double> fitavg = new List<double>();

                foreach (int iteration in iterations)
                {
                    var valuesAtIteration = bestSnapshotLists.Select((snapshotList, runIndex) => {
                        int length = snapshotList.Count();
                        if (indexes[runIndex] >= length) // if past last element
                        {
                            var h1 = snapshotList[length - 1].FitnessEvals;
                            var h2 = snapshotList[length - 1].BestFitness;
                            return new FitnessEvalAndValuePair(h1, h2);
                        }
                        else if (iteration == snapshotList[indexes[runIndex]].Iteration) // If element exists
                        {
                            var h1 = snapshotList[indexes[runIndex]].FitnessEvals;
                            var h2 = snapshotList[indexes[runIndex]].BestFitness;
                            indexes[runIndex] = indexes[runIndex] + 1;
                            return new FitnessEvalAndValuePair(h1, h2);
                        }
                        else //We must be in between
                        {
                            return recreatePoint(snapshotList, indexes[runIndex],iteration);
                        }

                    }).ToList();

                    fevalavg.Add(valuesAtIteration.Select(x => x.fitnessEvals).Average());
                    fitavg.Add(valuesAtIteration.Select(x => x.fitnessValue).Average());
                }

                string matrixName = detailGroup.Key.ToString() + (total ? "_TotalEvals.csv" : "_LargestEvals.csv");

                MapleMatrix matrix = new MapleMatrix(matrixName, formatter.FormatName(detailGroup.Key));
                matrix.AppendRow("iterations", iterations);
                matrix.AppendRow("fitness_evals", fevalavg);
                matrix.AppendRow("bestFitnessAvg", fitavg);
                matrices.Add(matrix);
            //};
            });

            WriteMatrices(matrices);
            WriteMapleCommands(total ? "MapleCommandsTotalFE.txt" : "MapleCommandsLargestFE.txt", matrices, total ? "fitnessEvaluationsTotal" : "fitnessEvaluationsMax", "fitness", 2, 3);
        }


        public void ParseDataFitnessEval(bool total, DisplayNameFormatter formatter)
        {
            IEnumerable<IGrouping<Configuration, AlgorithmRun>> detailGroups = DataSelector.GroupByDetails(data);
            ConcurrentBag<MapleMatrix> matrices = new ConcurrentBag<MapleMatrix>();

            Parallel.ForEach(detailGroups, detailGroup => {
                //Select the longest run amongst algorithms runs of different run no and rank
                AlgorithmRun longestRun = DataSelector.SelectLongestRun(detailGroup);
                int longestRunSize = longestRun.IterationSnapshots.Count;
                var runGroups = DataSelector.GroupByRun(detailGroup);

                double[] avg = new double[longestRunSize];
                double[] fevalavg = new double[longestRunSize];
                int[] fevalCounts = new int[longestRunSize];

                foreach (var runGroup in runGroups)
                {
                    int bestFit = runGroup.Select(x => x.BestFit).Max();
                    int bestFitItr = runGroup.Where(x => x.BestFit == bestFit).Select(x=>x.BestFitItr).Min();
                    int[] bestFitnesses = MathHelper.MaxValuesFit(DataSelector.SelectBestFitnesses(runGroup), longestRunSize, bestFitItr, bestFit);
                    int[] bestFitnessEvals = total
                        ? MathHelper.TotalValues(DataSelector.SelectFitnessEvaluations(runGroup), longestRunSize)
                        : MathHelper.MaxValuesNeg(DataSelector.SelectFitnessEvaluations(runGroup), longestRunSize);

                    for (int j = 0; j < longestRunSize; j++)
                    {
                        avg[j] += bestFitnesses[j];
                        if (bestFitnessEvals[j] != -1)
                        {
                            fevalavg[j] += bestFitnessEvals[j];
                            fevalCounts[j]++;
                        }
                    }
                }

                int groupAmount = runGroups.Count();

                for (int j = 0; j < longestRunSize; j++)
                {
                    avg[j] = avg[j] / groupAmount;
                    fevalavg[j] = fevalavg[j] / fevalCounts[j];
                }

                int[] iterations = DataSelector.SelectIterations(longestRun);
                string matrixName = detailGroup.Key.ToSimpleString() + (total ? "_TotalEvals.csv" : "_LargestEvals.csv");

                MapleMatrix matrix = new MapleMatrix(matrixName, formatter.FormatName(detailGroup.Key));
                matrix.AppendRow("iterations", iterations);
                matrix.AppendRow("fitness_evals", fevalavg);
                matrix.AppendRow("bestFitnessAvg", avg);
                matrices.Add(matrix);
            });

            WriteMatrices(matrices);
            WriteMapleCommands(total ? "MapleCommandsTotalFE.txt" : "MapleCommandsLargestFE.txt", matrices, total ? "fitnessEvaluationsTotal" : "fitnessEvaluationsMax", "fitness", 2, 3);
        }

        public void LatexData(int maxTableSize, DisplayNameFormatter formatter, DisplayNameFormatter Rformatter = null, bool showSolved = false) {

            DataWriter.AppendText(rawDataPath, $"\n\n #{new string('-', 100)} \n #Entry from {testName.Replace("/", "-")} [{DateTime.Now}]\n #{new string('-', 100)} \n\n");
            IEnumerable<IGrouping<Configuration, AlgorithmRun>> detailGroups = DataSelector.GroupByDetails(data);
            ConcurrentBag<(SummaryCollection, int, int)> summaries = new ConcurrentBag<(SummaryCollection, int, int)>();

            foreach(var detailGroup in detailGroups) {
                var optFitness = int.Parse(detailGroup.Key.Vars.Single(x => x.VariableName == "n").VariableValue);
                var FitnessFunction = detailGroup.First().Details.Single(x => x.VariableName == "fitnessFunction").VariableValue;

                switch (FitnessFunction)
                {
                    case "Jumpk":
                        if (detailGroup.First().Details.Where(x => x.VariableName == "jumpK").Count() == 1)
                        {
                            optFitness += int.Parse(detailGroup.First().Details.Single(x => x.VariableName == "jumpK").VariableValue);
                        }
                        else
                        {
                            optFitness += 4;
                        }
                        
                        break;
                }

                List<double> lowestFitnessItrs = new List<double>();
                List<double> largestFitEvals = new List<double>();
                List<double> totalFitEvals = new List<double>();
                IEnumerable<IGrouping<Configuration, AlgorithmRun>> runGroups = DataSelector.GroupByRun(detailGroup);
                int k = 0;

                foreach (var runGroup in runGroups)
                {


                    var solvedRuns = runGroup.Where(run => run.BestFit == optFitness);
                    if (!solvedRuns.Any()) continue;

                    k++;

                    var fastestSolvedNode = solvedRuns.MinBy(run => run.BestFitItr);

                    var largestFitEval = runGroup.Select(run => {
                        try
                        {
                            return run.IterationSnapshots.Single(i => i.Iteration == fastestSolvedNode.BestFitItr).FitnessEvals;
                        }
                        catch (Exception e)
                        {
                            return fastestSolvedNode.IterationSnapshots[fastestSolvedNode.BestFitItr].FitnessEvals;
                        }
                    }
                        ).Max();
                    var totalFitEval = runGroup.Select(run => {
                        try
                        {
                            return run.IterationSnapshots.Single(i => i.Iteration == fastestSolvedNode.BestFitItr).FitnessEvals;
                        }
                        catch (Exception e)
                        {
                            return fastestSolvedNode.IterationSnapshots[fastestSolvedNode.BestFitItr].FitnessEvals;
                        }
                    }
                        ).Sum();
                    lowestFitnessItrs.Add(fastestSolvedNode.BestFitItr);
                    largestFitEvals.Add(largestFitEval);
                    totalFitEvals.Add(totalFitEval);
                }

                string chunkyfy(List<double> numbers, int chunkSize) {

                    return string.Join(",\n", numbers.Select((x, i) => new { x, i }).GroupBy(x => x.i / chunkSize).Select(g => string.Join(",", g.Select(v => v.x)))) ;
                }

                if (Rformatter != null) {
                    string rawData = $"#--- {detailGroup.Key}: \n" + $"{Rformatter.FormatName(detailGroup.Key)}_LargestFitness <- c({chunkyfy(largestFitEvals, 100)}) \n";
                    DataWriter.AppendText(rawDataPath, rawData);
                }

                string name = formatter.FormatName(detailGroup.Key);
                if (showSolved)
                {
                    name = name + " " + k + "/" + runGroups.Count();
                }
                if (k > 0)
                {
                    var collection = new SummaryCollection(CreateStatisticsSummary("Largest fitness evaluations", largestFitEvals), name);
                    collection.Add(CreateStatisticsSummary("Total fitness evaluations", totalFitEvals));
                    collection.Add(CreateStatisticsSummary("Iterations", lowestFitnessItrs));
                    summaries.Add((collection, k, runGroups.Count()));
                }  
            };

            var selection = summaries.OrderByDescending(x => ((float)x.Item2) / ((float)x.Item3)).ThenBy(x => x.Item1.Primary.GetMean()).Take(Math.Min(maxTableSize, summaries.Count())).Select(x => x.Item1);
            string table = TableGenerator.GenerateLatexTable(selection);
            string commandsFile = $"{basePathOut}\\" + testName + "\\LatexCommands.txt";
            DataWriter.WriteText(commandsFile, table);
        }


        public void LatexDataBetween(int maxTableSize, DisplayNameFormatter formatter, int from, int to, bool showSolved = false)
        {
            IEnumerable<IGrouping<Configuration, AlgorithmRun>> detailGroups = DataSelector.GroupByDetails(data);
            ConcurrentBag<(SummaryCollection, int, int)> summaries = new ConcurrentBag<(SummaryCollection, int, int)>();

            Parallel.ForEach(detailGroups, detailGroup => {
                var optFitness = int.Parse(detailGroup.Key.Vars.Single(x => x.VariableName == "n").VariableValue);

                List<double> lowestFitnessItrs = new List<double>();
                List<double> largestFitEvals = new List<double>();
                List<double> totalFitEvals = new List<double>();
                IEnumerable<IGrouping<Configuration, AlgorithmRun>> runGroups = DataSelector.GroupByRun(detailGroup);
                int k = 0;

                foreach (var rungroup in runGroups)
                {


                    var solvedRuns = rungroup.Where(run => run.BestFit == optFitness);
                    if (!solvedRuns.Any()) continue;

                    k++;

                    var fastestSolvedNode = solvedRuns.MinBy(run => run.BestFitItr);

                    var largestFitEval = rungroup.Select(x => x.IterationSnapshots.Single(x => x.Iteration == fastestSolvedNode.BestFitItr).FitnessEvals).Max();
                    var totalFitEval = rungroup.Select(x => x.IterationSnapshots.Single(x => x.Iteration == fastestSolvedNode.BestFitItr).FitnessEvals).Sum();
                    lowestFitnessItrs.Add(fastestSolvedNode.BestFitItr);
                    largestFitEvals.Add(largestFitEval);
                    totalFitEvals.Add(totalFitEval);
                }

                string name = formatter.FormatName(detailGroup.Key);
                if (showSolved)
                {
                    name = name + " " + k + "/" + runGroups.Count();
                }
                if (k > 0) 
                {
                    var collection = new SummaryCollection(CreateStatisticsSummary("Largest fitness evaluations", largestFitEvals), name);
                    collection.Add(CreateStatisticsSummary("Total fitness evaluations", totalFitEvals));
                    collection.Add(CreateStatisticsSummary("Iterations", lowestFitnessItrs));
                    summaries.Add((collection, k, runGroups.Count()));
                }

            });

            var selection = summaries.OrderByDescending(x => x.Item2 / x.Item3).ThenBy(x => x.Item1.Primary.GetMean()).Take(Math.Min(maxTableSize, summaries.Count())).Select(x => x.Item1);
            string table = TableGenerator.GenerateLatexTable(selection);
            string commandsFile = $"{basePathOut}\\" + testName + "\\LatexCommands.txt";
            DataWriter.WriteText(commandsFile, table);
        }

        public void JumpLatexData(int maxTableSize, DisplayNameFormatter formatter, int maxJumpAmount, bool showSolved = false)
        {

            IEnumerable<IGrouping<Configuration, AlgorithmRun>> jumpGroups = data.GroupBy(Configuration.Include(["jumpSize"]));
            ConcurrentBag<(SummaryCollection, int, int)> summaries = new ConcurrentBag<(SummaryCollection, int, int)>();
            foreach (var jumpGroup in jumpGroups)
            {
                IEnumerable<IGrouping<Configuration, AlgorithmRun>> detailGroups = DataSelector.GroupByDetails(jumpGroup);
                ConcurrentBag<(SummaryCollection, int, int)> jumpSummaries = new ConcurrentBag<(SummaryCollection, int, int)>();

                Parallel.ForEach(detailGroups, detailGroup => {
                    var optFitness = int.Parse(detailGroup.Key.Vars.Single(x => x.VariableName == "n").VariableValue);

                    List<double> lowestFitnessItrs = new List<double>();
                    List<double> largestFitEvals = new List<double>();
                    List<double> totalFitEvals = new List<double>();
                    IEnumerable<IGrouping<Configuration, AlgorithmRun>> runGroups = DataSelector.GroupByRun(detailGroup);
                    int k = 0;

                    foreach (var rungroup in runGroups)
                    {


                        var solvedRuns = rungroup.Where(run => run.BestFit == optFitness);
                        if (!solvedRuns.Any()) continue;

                        k++;

                        var fastestSolvedNode = solvedRuns.MinBy(run => run.BestFitItr);

                        var largestFitEval = rungroup.Select(x => x.IterationSnapshots.Single(x => x.Iteration == fastestSolvedNode.BestFitItr).FitnessEvals).Max();
                        var totalFitEval = rungroup.Select(x => x.IterationSnapshots.Single(x => x.Iteration == fastestSolvedNode.BestFitItr).FitnessEvals).Sum();
                        lowestFitnessItrs.Add(fastestSolvedNode.BestFitItr);
                        largestFitEvals.Add(largestFitEval);
                        totalFitEvals.Add(totalFitEval);
                    }

                    string name = formatter.FormatName(detailGroup.Key);
                    if (showSolved)
                    {
                        name = name + " " + k + "/" + runGroups.Count();
                    }
                    if (k > 0)
                    {
                        var collection = new SummaryCollection(CreateStatisticsSummary("Largest fitness evaluations", largestFitEvals), name);
                        collection.Add(CreateStatisticsSummary("Total fitness evaluations", totalFitEvals));
                        collection.Add(CreateStatisticsSummary("Iterations", lowestFitnessItrs));
                        jumpSummaries.Add((collection, k, runGroups.Count()));
                    }

                });
                var jumpSelection = jumpSummaries.OrderByDescending(x => (double)x.Item2 / x.Item3).ThenBy(x => x.Item1.Primary.GetMean()).Take(Math.Min(maxJumpAmount, jumpSummaries.Count()));
                foreach (var summary in jumpSelection)
                {
                    summaries.Add(summary);
                }

            }
            

            var selection = summaries.OrderByDescending(x => (double)x.Item2 / x.Item3).ThenBy(x => x.Item1.Primary.GetMean()).Take(Math.Min(maxTableSize, summaries.Count())).Select(x => x.Item1);
            string table = TableGenerator.GenerateLatexTable(selection);
            string commandsFile = $"{basePathOut}\\" + testName + "\\LatexCommands.txt";
            DataWriter.WriteText(commandsFile, table);
        }

        public void parseConst(DisplayNameFormatter formatter, string constName = "lambda", string[] extraConsts = null)
        {
            List<string> exclusions = [constName, "run", "rank"];
            if (extraConsts != null)
            {
                foreach (var exclusion in extraConsts)
                {
                    exclusions.Add(exclusion);
                }
            }
            List<IGrouping<Configuration, AlgorithmRun>> detailLambdaGroups = data.GroupBy(Configuration.Exclude(exclusions.ToArray())).ToList();
            ConcurrentBag<MapleMatrix> largestMatrices = new ConcurrentBag<MapleMatrix>();
            ConcurrentBag<MapleMatrix> totalMatrices = new ConcurrentBag<MapleMatrix>();

            Parallel.ForEach(detailLambdaGroups, detailLambdaGroup => {
                List<double> lambdas = new List<double>();
                List<double> nTotalFitEvalAvg = new List<double>();
                List<double> nLargestFitEvalAvg = new List<double>();

                

                var lambdaGroups = detailLambdaGroup.GroupBy(Configuration.Include(constName)).ToList();
                foreach (var lambdaGroup in lambdaGroups)
                {
                    var optFitness = int.Parse(lambdaGroup.First().Details.Single(x => x.VariableName == "n").VariableValue); // Works for all but MaxSat :-) (OneMax, LeadingOnes, JumpK, TwoPeaks)
                    var FitnessFunction = lambdaGroup.First().Details.Single(x => x.VariableName == "fitnessFunction").VariableValue;

                    switch (FitnessFunction)
                    {
                        case "Jumpk":
                            optFitness += int.Parse(lambdaGroup.First().Details.Single(x => x.VariableName == "jumpK").VariableValue);
                            break;
                    }

                    List<double> largestFitEvals = new List<double>();
                    List<double> totalFitEvals = new List<double>();

                    var runGroups = lambdaGroup.GroupBy(Configuration.Include("run")).ToList();
                    foreach (var runGroup in runGroups)
                    {
                        var solvedRuns = runGroup.Where(run => run.BestFit == optFitness);
                        if (!solvedRuns.Any()) continue;

                        var fastestSolvedNode = solvedRuns.MinBy(run => run.BestFitItr);
                        var fitnessEvalsCollection = runGroup.Select(run => {
                            try
                            {
                                return run.IterationSnapshots.Single(i => i.Iteration == fastestSolvedNode.BestFitItr).FitnessEvals;
                            }
                            catch (Exception e)
                            {
                                return fastestSolvedNode.IterationSnapshots[fastestSolvedNode.BestFitItr].FitnessEvals;
                            }
                        }
                        );
                        int largest = fitnessEvalsCollection.Max();
                        int total = fitnessEvalsCollection.Sum();
                        largestFitEvals.Add(largest);
                        totalFitEvals.Add(total);
                    }
                    if (totalFitEvals.Count() < 1) continue;
                    lambdas.Add(int.Parse(lambdaGroup.Key.Vars.Single(x => x.VariableName == constName).VariableValue));
                    nTotalFitEvalAvg.Add(totalFitEvals.Average());
                    nLargestFitEvalAvg.Add(largestFitEvals.Average());
                }
                if (lambdas.Count() < 1) return;
                foreach ((bool, List<double>) list in new List<(bool, List<double>)> { (true, nTotalFitEvalAvg), (false, nLargestFitEvalAvg) })
                {
                    var sorted = lambdas.Select((x, i) => new { N = x, FitEvalAvg = list.Item2[i] }).OrderBy(x => x.N);
                    string matrixName = detailLambdaGroup.Key.ToSimpleString() + (list.Item1 ? "Const_TotalEvals.csv" : "Const_LargestEvals.csv");

                    MapleMatrix matrix = new MapleMatrix(matrixName, formatter.FormatName(detailLambdaGroup.Key));
                    matrix.AppendRow(constName, sorted.Select(x => x.N));
                    matrix.AppendRow("FitEvalAvg", sorted.Select(x => x.FitEvalAvg));

                    if (list.Item1) totalMatrices.Add(matrix);
                    else largestMatrices.Add(matrix);
                }
            });

            WriteMatrices(totalMatrices);
            WriteMatrices(largestMatrices);
            WriteMapleCommands("MapleCommandsTotalFE.txt", totalMatrices, constName, "fitnessEvaluationsTotal", 1, 2);
            WriteMapleCommands("MapleCommandsLargestFE.txt", largestMatrices, constName, "fitnessEvaluationsMax", 1, 2);

        }

        public void parseConstFixed(DisplayNameFormatter formatter, string constName = "lambda", string[] extraConsts = null)
        {
            List<string> inclusions = [constName];
            if (extraConsts != null)
            {
                foreach (var inclusion in extraConsts)
                {
                    inclusions.Add(inclusion);
                }
            }
            List<IGrouping<Configuration, AlgorithmRun>> constGroups = data.GroupBy(Configuration.Include(inclusions.ToArray())).ToList();
            ConcurrentBag<MapleMatrix> largestMatrices = new ConcurrentBag<MapleMatrix>();
            ConcurrentBag<MapleMatrix> totalMatrices = new ConcurrentBag<MapleMatrix>();

            Parallel.ForEach(constGroups, constGroup => {
                List<double> lambdas = new List<double>();
                List<double> nTotalFitEvalAvg = new List<double>();
                List<double> nLargestFitEvalAvg = new List<double>();



                var detailGroups = constGroup.GroupBy(Configuration.Exclude(["run", "rank"])).ToList();
                foreach (var detailGroup in detailGroups)
                {
                    var optFitness = int.Parse(detailGroup.First().Details.Single(x => x.VariableName == "n").VariableValue); // Works for all but MaxSat :-) (OneMax, LeadingOnes, JumpK, TwoPeaks)
                    List<double> largestFitEvals = new List<double>();
                    List<double> totalFitEvals = new List<double>();

                    var runGroups = detailGroup.GroupBy(Configuration.Include("run")).ToList();
                    foreach (var runGroup in runGroups)
                    {
                        var solvedRuns = runGroup.Where(run => run.BestFit == optFitness);
                        if (!solvedRuns.Any()) continue;

                        var fastestSolvedNode = solvedRuns.MinBy(run => run.BestFitItr);
                        var fitnessEvalsCollection = runGroup.Select(run => {
                            try
                            {
                                return run.IterationSnapshots.Single(i => i.Iteration == fastestSolvedNode.BestFitItr).FitnessEvals;
                            }
                            catch (Exception e)
                            {
                                return fastestSolvedNode.IterationSnapshots[fastestSolvedNode.BestFitItr].FitnessEvals;
                            }
                        }
                        );
                        int largest = fitnessEvalsCollection.Max();
                        int total = fitnessEvalsCollection.Sum();
                        largestFitEvals.Add(largest);
                        totalFitEvals.Add(total);
                    }
                    if (totalFitEvals.Count() < 1) continue;
                    lambdas.Add(int.Parse(detailGroup.Key.Vars.Single(x => x.VariableName == constName).VariableValue));
                    nTotalFitEvalAvg.Add(totalFitEvals.Average());
                    nLargestFitEvalAvg.Add(largestFitEvals.Average());
                }
                if (lambdas.Count() < 1) return;
                foreach ((bool, List<double>) list in new List<(bool, List<double>)> { (true, nTotalFitEvalAvg), (false, nLargestFitEvalAvg) })
                {
                    var sorted = lambdas.Select((x, i) => new { N = x, FitEvalAvg = list.Item2[i] }).OrderBy(x => x.N);
                    string matrixName = constGroup.Key.ToSimpleString() + (list.Item1 ? "_TotalEvals.csv" : "_LargestEvals.csv");

                    MapleMatrix matrix = new MapleMatrix(matrixName, formatter.FormatName(constGroup.Key));
                    matrix.AppendRow(constName, sorted.Select(x => x.N));
                    matrix.AppendRow("FitEvalAvg", sorted.Select(x => x.FitEvalAvg));

                    if (list.Item1) totalMatrices.Add(matrix);
                    else largestMatrices.Add(matrix);
                }
            });

            WriteMatrices(totalMatrices);
            WriteMatrices(largestMatrices);
            WriteMapleCommands("MapleCommandsTotalFE.txt", totalMatrices, constName, "fitnessEvaluationsTotal", 1, 2);
            WriteMapleCommands("MapleCommandsLargestFE.txt", largestMatrices, constName, "fitnessEvaluationsMax", 1, 2);

        }


        public void parseNTest(DisplayNameFormatter formatter) {
            List<IGrouping<Configuration, AlgorithmRun>> detailLambdaGroups = data.GroupBy(Configuration.Exclude(["cK", "mu", "n", "run", "rank"])).ToList();
            ConcurrentBag<MapleMatrix> largestMatrices = new ConcurrentBag<MapleMatrix>();
            ConcurrentBag<MapleMatrix> totalMatrices = new ConcurrentBag<MapleMatrix>();

            //Parallel.ForEach(detailLambdaGroups, detailLambdaGroup => {
            foreach(var detailLambdaGroup in detailLambdaGroups) {   
                List<double> lambdas = new List<double>();
                List<double> nTotalFitEvalAvg = new List<double>();
                List<double> nLargestFitEvalAvg = new List<double>();
                
                var lambdaGroups = detailLambdaGroup.GroupBy(Configuration.Include("n")).ToList();
                foreach (var lambdaGroup in lambdaGroups) {

                    var optFitness = int.Parse(lambdaGroup.Key.Vars.Single(x => x.VariableName == "n").VariableValue); // Works for all but MaxSat :-) (OneMax, LeadingOnes, JumpK, TwoPeaks)

                    List<double> largestFitEvals = new List<double>();
                    List<double> totalFitEvals = new List<double>();

                    var runGroups = lambdaGroup.GroupBy(Configuration.Include("run")).ToList();
                    foreach (var runGroup in runGroups) {
                        var solvedRuns = runGroup.Where(run => run.BestFit == optFitness);
                        if (!solvedRuns.Any()) continue;

                        var fastestSolvedNode = solvedRuns.MinBy(run => run.BestFitItr);

                        int largest = runGroup.Select(run => run.IterationSnapshots.Single(i => i.Iteration == fastestSolvedNode.BestFitItr).FitnessEvals).Max();
                        int total = runGroup.Select(run => run.IterationSnapshots.Single(i => i.Iteration == fastestSolvedNode.BestFitItr).FitnessEvals).Sum();
                        largestFitEvals.Add(largest);
                        totalFitEvals.Add(total);
                    }
                    if (totalFitEvals.Count() < 1) continue;
                    lambdas.Add(int.Parse(lambdaGroup.Key.Vars.Single(x => x.VariableName == "n").VariableValue));
                    nTotalFitEvalAvg.Add(totalFitEvals.Average());
                    nLargestFitEvalAvg.Add(largestFitEvals.Average());
                }

                foreach ((bool, List<double>) list in new List<(bool, List<double>)> { (true, nTotalFitEvalAvg), (false, nLargestFitEvalAvg) }) {
                    var sorted = lambdas.Select((x, i) => new { N = x, FitEvalAvg = list.Item2[i] }).OrderBy(x => x.N);
                    string matrixName = detailLambdaGroup.Key.ToString() + (list.Item1 ? "_TotalEvals.csv" : "_LargestEvals.csv");

                    MapleMatrix matrix = new MapleMatrix(matrixName, formatter.FormatName(detailLambdaGroup.Key));
                    matrix.AppendRow("N", sorted.Select(x => x.N));
                    matrix.AppendRow("FitEvalAvg", sorted.Select(x => x.FitEvalAvg));

                    if (list.Item1) totalMatrices.Add(matrix);
                    else largestMatrices.Add(matrix);
                }
            };//);

            WriteMatrices(totalMatrices);
            WriteMatrices(largestMatrices);
            WriteMapleCommands("MapleCommandsTotalFE_N.txt", totalMatrices, "N", "fitnessEvaluationsTotal", 1, 2);
            WriteMapleCommands("MapleCommandsLargestFE_N.txt", largestMatrices, "N", "fitnessEvaluationsMax", 1, 2);
        }


        public void parseProbDist(DisplayNameFormatter formatter, bool lower = true) {
            List<IGrouping<Configuration, AlgorithmRun>> detailLambdaGroups = data.GroupBy(Configuration.Exclude(["run", "rank"])).ToList();
            ConcurrentBag<MapleMatrix> matrices = new ConcurrentBag<MapleMatrix>();

            //Parallel.ForEach(detailLambdaGroups, detailLambdaGroup => {
            foreach (var detailLambdaGroup in detailLambdaGroups) {

                var n = int.Parse(detailLambdaGroup.First().Details.Single(x => x.VariableName == "n").VariableValue);
                List<List<(int, double)>> prob_dist_avgs = new List<List<(int, double)>>();

                var runGroups = detailLambdaGroup.GroupBy(Configuration.Include(["run"])).ToList();

                foreach (var runGroup in runGroups) {
                    int maxLength = runGroup.Select(g => g.IterationSnapshots.Count).Max();
                    List<(int, double)> bounds = new List<(int, double)>(maxLength);
                    for (int i = 0; i < maxLength; i++) bounds.Add((runGroup.First().IterationSnapshots[i].Iteration, 0));

                    foreach (var node in runGroup) {
                        for (int i = 0; i < node.IterationSnapshots.Count; i++) {
                            if (lower) bounds[i] = (node.IterationSnapshots[i].Iteration, bounds[i].Item2 + node.IterationSnapshots[i].LowerBoundHits);
                            else bounds[i] = (node.IterationSnapshots[i].Iteration, bounds[i].Item2 + node.IterationSnapshots[i].UpperBoundHits);
                        }
                    }

                    prob_dist_avgs.Add(bounds.Select(t => (t.Item1, t.Item2 / runGroup.Count())).ToList());
                }

                List<int> iterations = runGroups.SelectMany(runGroup => runGroup.First().IterationSnapshots.Select(snapshot => snapshot.Iteration)).Distinct().Order().ToList();
                int runCount = runGroups.Count();
                int[] indexes = new int[runCount];
                List<double> prob_count_avg = new List<double>();

                foreach (int iteration in iterations) {
                    var valuesAtIteration = prob_dist_avgs.Select((snapshotList, runIndex) => {
                        int length = snapshotList.Count();
                        if (indexes[runIndex] >= length) {
                            return new IterationAndProbBoundCountPair(-1, snapshotList[length - 1].Item2);
                        } else if (iteration == snapshotList[indexes[runIndex]].Item1) {
                            var res = new IterationAndProbBoundCountPair(-1, snapshotList[indexes[runIndex]].Item2);
                            indexes[runIndex] = indexes[runIndex] + 1;
                            return res;
                        } else {
                            return RecreateProbPoint(snapshotList, indexes[runIndex], iteration);
                        }
                    }).ToList();

                    // average here
                    prob_count_avg.Add(valuesAtIteration.Average(IPBCP => IPBCP.ProbBoundCount));
                }

                string matrixName = detailLambdaGroup.Key.ToString() + (lower ? "Lower" : "Upper") + "_GeneticDriftAnalysis.csv";

                MapleMatrix matrix = new MapleMatrix(matrixName, formatter.FormatName(detailLambdaGroup.Key));
                matrix.AppendRow("iterations", iterations);
                matrix.AppendRow("Prob_Bounds_Count", prob_count_avg);
                matrices.Add(matrix);

            }

            WriteMatrices(matrices);
            WriteMapleCommands((lower ? "Lower" : "Upper") + "GeneticDriftAnalysis.txt", matrices, "Iterations", lower ? "Lower Bounds Hit Count" : "Upper Bounds Hit Count", 1, 2);
        }

        public void parseBoundHits(DisplayNameFormatter formatter, bool lower = true)
        {
            List<IGrouping<Configuration, AlgorithmRun>> detailLambdaGroups = data.GroupBy(Configuration.Exclude(["run", "rank"])).ToList();
            ConcurrentBag<MapleMatrix> matrices = new ConcurrentBag<MapleMatrix>();

            //Parallel.ForEach(detailLambdaGroups, detailLambdaGroup => {
            foreach (var detailLambdaGroup in detailLambdaGroups)
            {

                var n = int.Parse(detailLambdaGroup.First().Details.Single(x => x.VariableName == "n").VariableValue);
                List<List<(int, double, double)>> avgs = new List<List<(int, double, double)>>();

                var runGroups = detailLambdaGroup.GroupBy(Configuration.Include(["run"])).ToList();

                foreach (var runGroup in runGroups)
                {
                    int maxLength = runGroup.Select(g => g.IterationSnapshots.Count).Max();
                    List<(int, double, double)> bounds = new List<(int, double, double)>(maxLength); //Fitness evals and bound hits
                    for (int i = 0; i < maxLength; i++) bounds.Add((runGroup.First().IterationSnapshots[i].Iteration, 0, 0));

                    foreach (var node in runGroup)
                    {
                        for (int i = 0; i < node.IterationSnapshots.Count; i++)
                        {
                            if (lower) bounds[i] = (
                                    node.IterationSnapshots[i].Iteration,
                                    bounds[i].Item2 + node.IterationSnapshots[i].FitnessEvals, 
                                    bounds[i].Item3 + node.IterationSnapshots[i].LowerBoundHits
                                );
                            else bounds[i] = (
                                    node.IterationSnapshots[i].Iteration,
                                    bounds[i].Item2 + node.IterationSnapshots[i].FitnessEvals, 
                                    bounds[i].Item3 + node.IterationSnapshots[i].UpperBoundHits
                                );
                        }
                    }

                    avgs.Add(bounds.Select(t => (t.Item1, t.Item2 / runGroup.Count(), t.Item3 / runGroup.Count())).ToList());
                }

                List<int> iterations = runGroups.SelectMany(runGroup => runGroup.First().IterationSnapshots.Select(snapshot => snapshot.Iteration)).Distinct().Order().ToList();
                int runCount = runGroups.Count();
                int[] indexes = new int[runCount];
                List<double> fevalavg = new List<double>();
                List<double> prob_count_avg = new List<double>();

                foreach (int iteration in iterations)
                {
                    var valuesAtIteration = avgs.Select((snapshotList, runIndex) => {
                        int length = snapshotList.Count();
                        if (indexes[runIndex] >= length)
                        {
                            return new FitnessEvalAndBoundCountPair(snapshotList[length - 1].Item2, snapshotList[length - 1].Item3);
                        }
                        else if (iteration == snapshotList[indexes[runIndex]].Item1)
                        {

                            var res = new FitnessEvalAndBoundCountPair(snapshotList[indexes[runIndex]].Item2, snapshotList[indexes[runIndex]].Item3);
                            indexes[runIndex] = indexes[runIndex] + 1;
                            return res;
                        }
                        else
                        {
                            return RecreateFitEvalAndBoundPoint(snapshotList, indexes[runIndex], iteration);
                        }
                    }).ToList();

                    // average here
                    fevalavg.Add(valuesAtIteration.Select(x => x.fitnessEvals).Average());
                    prob_count_avg.Add(valuesAtIteration.Average(IPBCP => IPBCP.ProbBoundCount));
                }

                string matrixName = detailLambdaGroup.Key.ToSimpleString() + (lower ? "Lower" : "Upper") + "_GeneticDriftAnalysis.csv";

                MapleMatrix matrix = new MapleMatrix(matrixName, formatter.FormatName(detailLambdaGroup.Key));
                matrix.AppendRow("iterations", iterations);
                matrix.AppendRow("fitness_evals", fevalavg);
                matrix.AppendRow("Prob_Bounds_Count", prob_count_avg);
                matrices.Add(matrix);

            }

            WriteMatrices(matrices);
            WriteMapleCommands((lower ? "Lower" : "Upper") + "GeneticDriftAnalysis.txt", matrices, "FitnessEvaluations", lower ? "Lower Bounds Hit Count" : "Upper Bounds Hit Count", 2, 3);
        }

        private void WriteMatrices(IEnumerable<MapleMatrix> matrices) {
            foreach (MapleMatrix matrix in matrices) {
                string path = $"{basePathOut}{testName}/{matrix.Name}";
                DataWriter.WriteMapleMatrix(path, matrix);
            }
        }

        private void WriteMapleCommands(string fileName, IEnumerable<MapleMatrix> matrices, string xaxis, string yaxis, int xindex, int yindex) {
            var paths = matrices.Select(GetMapleCommandFilePath);
            var displaynames = matrices.Select(m => m.DisplayName);

            DataWriter.WriteText(
                $"{basePathOut}{testName}/{fileName}", 
                MapleCommandGenerator.GenerateMapleCommands(paths, displaynames, xaxis, yaxis, xindex, yindex));
        }

        private string GetMapleCommandFilePath(MapleMatrix matrix) {
            return $"{staticbasePathOut}{testName}/{matrix.Name}";
        }

        private static StatisticsSummary CreateStatisticsSummary(string name, List<double> data) {
            return new StatisticsSummary(name, MathHelper.Mean(data), MathHelper.Median(data), MathHelper.Percentile(data, 25), MathHelper.Percentile(data, 75), MathHelper.STD(data));
        }

        public void parseXoverY(DisplayNameFormatter formatter, string[] xs, string[] ys)
        {
            List<string> exclusions = ["run", "rank"];
            foreach (string s in xs)
            {
                exclusions.Add(s);
            }
            List<IGrouping<Configuration, AlgorithmRun>> detailGroups = data.GroupBy(Configuration.Exclude(exclusions.ToArray())).ToList();
            ConcurrentBag<MapleMatrix> matrices = new ConcurrentBag<MapleMatrix>();

            Parallel.ForEach(detailGroups, detailGroup => {

                var optFitness = int.Parse(detailGroup.Key.Vars.Single(x => x.VariableName == "n").VariableValue); // Works for all but MaxSat :-) (OneMax, LeadingOnes, JumpK, TwoPeaks)

                List<double> xValues = new List<double>();
                List<double> yValues = new List<double>();

                var xGroups = detailGroup.GroupBy(Configuration.Include(xs)).ToList();
                foreach (var xGroup in xGroups)
                {

                    List<double> yValuesInner = new List<double>();

                    var runGroups = xGroup.GroupBy(Configuration.Include("run")).ToList();
                    foreach (var runGroup in runGroups)
                    {
                        var solvedRuns = runGroup.Where(run => run.BestFit == optFitness);
                        if (!solvedRuns.Any()) continue;

                        var fastestSolvedNode = solvedRuns.MinBy(run => run.BestFitItr);

                        double yValue = runGroup.SelectMany(
                            run => run.Details.Where(
                                detail=> ys.Contains(detail.VariableName))
                            .Select(
                                detail=>detail.VariableValue))
                        .Select(
                            value=>Int32.Parse(value))
                        .Average();
                        yValuesInner.Add(yValue);
                    }
                    if (yValuesInner.Count() < 1) continue;
                    xValues.Add(xGroup.Key.Vars.Where(
                        x => xs.Contains(x.VariableName))
                        .Select(
                                detail => detail.VariableValue)
                        .Select(
                            value => Int32.Parse(value))
                        .Average());

                    yValues.Add(yValuesInner.Average());
                }
                var sorted = xValues.Select((x, i) => new { itemXValue = x, itemYValue = yValues[i] }).OrderBy(x => x.itemXValue);
                string matrixName = detailGroup.Key.ToString() + ("XoverY.csv");

                MapleMatrix matrix = new MapleMatrix(matrixName, formatter.FormatName(detailGroup.Key));
                matrix.AppendRow(xs.ToString(), sorted.Select(x => x.itemXValue));
                matrix.AppendRow(ys.ToString(), sorted.Select(x => x.itemYValue));

                matrices.Add(matrix);
            });

            WriteMatrices(matrices);
            WriteMapleCommands("MapleCommandsXoverY.txt", matrices, xs.ToString(), ys.ToString(), 1, 2);

        }

        public void parseXoverYBestYs(DisplayNameFormatter formatter, string[] xs, string[] ys, string[] extraXs, string[] extraYs)
        {
            List<string> exclusions = ["run", "rank"];
            foreach (string s in xs)
            {
                exclusions.Add(s);
            }
            foreach (string s in ys)
            {
                exclusions.Add(s);
            }
            foreach (string s in extraXs)
            {
                exclusions.Add(s);
            }
            foreach (string s in extraYs)
            {
                exclusions.Add(s);
            }
            List<IGrouping<Configuration, AlgorithmRun>> detailGroups = data.GroupBy(Configuration.Exclude(exclusions.ToArray())).ToList();
            ConcurrentBag<MapleMatrix> matrices = new ConcurrentBag<MapleMatrix>();

            Parallel.ForEach(detailGroups, detailGroup => {

                List<double> xValues = new List<double>();
                List<double> yValues = new List<double>();
                List<string> Xinclusions = new List<string>();
                Xinclusions.AddRange(xs);
                Xinclusions.AddRange(extraXs);

                var xGroups = detailGroup.GroupBy(Configuration.Include(Xinclusions.ToArray())).ToList();
                foreach (var xGroup in xGroups)
                {
                    List<double> yValuesMiddle = new List<double>();
                    List<double> fitnessEvalsMiddle = new List<double>();
                    List<string> Yinclusions = new List<string>();
                    Yinclusions.AddRange(ys);
                    Yinclusions.AddRange(extraYs);
                    var yGroups = xGroup.GroupBy(Configuration.Include(Yinclusions.ToArray())).ToList();
                    foreach (var yGroup in yGroups)
                    {
                        bool fullySolved = true;
                        List<double> yValuesInner = new List<double>();
                        List<double> fitnessEvalsInner = new List<double>();

                        var runGroups = yGroup.GroupBy(Configuration.Include("run")).ToList();
                        foreach (var runGroup in runGroups)
                        {
                            var optFitness = int.Parse(runGroup.First().Details.Single(x => x.VariableName == "n").VariableValue); // Works for all but MaxSat :-) (OneMax, LeadingOnes, JumpK, TwoPeaks)
                            var solvedRuns = runGroup.Where(run => run.BestFit == optFitness);
                            if (!solvedRuns.Any())
                            {
                                fullySolved = false;
                                break;
                            }
                            var fastestSolvedNode = solvedRuns.MinBy(run => run.BestFitItr);

                            double yValue = runGroup.SelectMany(
                                run => run.Details.Where(
                                    detail => ys.Contains(detail.VariableName))
                                .Select(
                                    detail => detail.VariableValue))
                            .Select(
                                value => Int32.Parse(value))
                            .Average();
                            double fitnessEval = runGroup.Select(run => run.IterationSnapshots.Single(i => i.Iteration == fastestSolvedNode.BestFitItr).FitnessEvals).Max();
                            yValuesInner.Add(yValue);
                            fitnessEvalsInner.Add(fitnessEval);
                        }
                        if (!fullySolved) continue;


                        yValuesMiddle.Add(yValuesInner.Average());
                        fitnessEvalsMiddle.Add(fitnessEvalsInner.Average());
                    }

                    xValues.Add(xGroup.Key.Vars.Where(
                            x => xs.Contains(x.VariableName))
                            .Select(
                                    detail => detail.VariableValue)
                            .Select(
                                value => Int32.Parse(value))
                            .Average());
                    int bestIndex = fitnessEvalsMiddle.IndexOf(fitnessEvalsMiddle.Min());
                    yValues.Add(yValuesMiddle[bestIndex]);


                }
                if (xValues.Count()==0)
                {
                    return;
                }
                var sorted = xValues.Select((x, i) => new { itemXValue = x, itemYValue = yValues[i] }).OrderBy(x => x.itemXValue);
                string matrixName = detailGroup.Key.ToString() + ("XoverY.csv");

                MapleMatrix matrix = new MapleMatrix(matrixName, formatter.FormatName(detailGroup.Key));
                matrix.AppendRow(String.Join(" ",xs), sorted.Select(x => x.itemXValue));
                matrix.AppendRow(String.Join(" ", ys), sorted.Select(x => x.itemYValue));

                matrices.Add(matrix);
            });

            WriteMatrices(matrices);
            WriteMapleCommands("MapleCommandsXoverY.txt", matrices, String.Join(" ", xs), String.Join(" ", ys), 1, 2);

        }
    }
}
