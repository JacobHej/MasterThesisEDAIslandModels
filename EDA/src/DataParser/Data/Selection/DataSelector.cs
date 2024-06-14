using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataParser.Data.Selection
{
    public static class DataSelector {
        /// <summary>
        /// Returns groupings containing all runs and ranks for an experiment. Groups based on details
        /// </summary>
        /// <param name="runs"></param>
        /// <returns></returns>
        public static IEnumerable<IGrouping<Configuration, AlgorithmRun>> GroupByDetails(IEnumerable<AlgorithmRun> runs) => runs.GroupBy(Configuration.Exclude(["run", "rank"]));
        /// <summary>
        /// Returns groupings containing all runs and ranks for an experiment. Groups based on details
        /// </summary>
        /// <param name="runs"></param>
        /// <returns></returns>
        public static IEnumerable<IGrouping<Configuration, AlgorithmRun>> GroupByDetailsWithoutN(IEnumerable<AlgorithmRun> runs) => runs.GroupBy(Configuration.Exclude(["run", "rank", "n"]));
        /// <summary>
        /// Returns groupings containing all runs and ranks for an experiment. Groups based on details
        /// </summary>
        /// <param name="runs"></param>
        /// <returns></returns>
        public static IEnumerable<IGrouping<Configuration, AlgorithmRun>> MaxSatGroupByDetailsWithoutInstance(IEnumerable<AlgorithmRun> runs) => runs.GroupBy(Configuration.Exclude(["run", "rank", "instance"]));

        /// <summary>
        /// Returns groupings containing all nodes an experiment for a given run. Groups based on run
        /// </summary>
        /// <param name="runs"></param>
        /// <returns></returns>
        public static IEnumerable<IGrouping<Configuration, AlgorithmRun>> GroupByRun(IGrouping<Configuration, AlgorithmRun> groups) => groups.GroupBy(Configuration.Include(["run"]));

        /// <summary>
        /// Returns groupings containing all nodes an experiment for a given run. Groups based on run
        /// </summary>
        /// <param name="runs"></param>
        /// <returns></returns>
        public static IEnumerable<IGrouping<Configuration, AlgorithmRun>> GroupByN(IGrouping<Configuration, AlgorithmRun> groups) => groups.GroupBy(Configuration.Include(["n"]));

        /// <summary>
        /// For a group finds the longest AlgorithmRun by amount of iteration snapshots
        /// </summary>
        /// <param name="group"></param>
        /// <returns></returns>
        public static AlgorithmRun SelectLongestRun(IGrouping<Configuration, AlgorithmRun> group) => group.OrderByDescending(x => x.IterationSnapshots.Count).First();

        /// <summary>
        /// Will select best fitness at each iteration snapshot and return an IEnumerable for each algorithm run in the group
        /// </summary>
        /// <param name="group"></param>
        /// <returns></returns>
        public static IEnumerable<IEnumerable<int>> SelectBestFitnesses(IGrouping<Configuration, AlgorithmRun> group) => group.Select(x => x.IterationSnapshots.Select(y => y.BestFitness));

        /// <summary>
        /// Will select best fitness at each iteration snapshot and return an IEnumerable for each algorithm run in the group
        /// </summary>
        /// <param name="group"></param>
        /// <returns></returns>
        public static IEnumerable<IEnumerable<int>> SelectFitnessEvaluations(IGrouping<Configuration, AlgorithmRun> group) => group.Select(x => x.IterationSnapshots.Select(y => y.FitnessEvals));

        /// <summary>
        /// Will select iterations from iteration snapshot
        /// </summary>
        /// <param name="group"></param>
        /// <returns></returns>
        public static int[] SelectIterations(AlgorithmRun run) => run.IterationSnapshots.Select(x => x.Iteration).ToArray();

    }
}
