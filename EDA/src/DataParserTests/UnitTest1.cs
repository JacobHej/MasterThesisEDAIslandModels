using DataParser.Data;
using DataParser.Maple;
using DataParser.Naming;
using DataParserTests;
using Microsoft.VisualBasic;
using System.Collections.Generic;

namespace DataParserTests {
    [TestClass]
    public class UnitTest1 {

        [TestMethod]
        public void TestReconstructMissingIterations()
        {
            IterationSnapshot snapshot1 = new IterationSnapshot();
            snapshot1.BestFitness = 100;
            snapshot1.Iteration = 1;
            IterationSnapshot snapshot2 = new IterationSnapshot();
            snapshot2.BestFitness = 1000;
            snapshot2.Iteration = 10;

            AlgorithmRun algorithmRun = new AlgorithmRun();
            algorithmRun.IterationSnapshots = [snapshot1, snapshot2];

            algorithmRun.ReconstructMissingIterations();
            CollectionAssert.AreEqual(algorithmRun.IterationSnapshots.Select(i => i.BestFitness).ToList(), Enumerable.Range(1, 10).Select(i => i * 100).ToList());
        }

        [TestMethod]
        public void TestReconstructMissingIteration()
        {
            IterationSnapshot snapshot1 = new IterationSnapshot();
            snapshot1.BestFitness = 0;
            snapshot1.FitnessEvals = 0;
            snapshot1.Iteration = 0;
            IterationSnapshot snapshot2 = new IterationSnapshot();
            snapshot2.BestFitness = 1;
            snapshot2.FitnessEvals = 1;
            snapshot2.Iteration = 1;
            IterationSnapshot snapshot3 = new IterationSnapshot();
            snapshot3.BestFitness = 2;
            snapshot3.FitnessEvals = 2;
            snapshot3.Iteration = 2;

            CopiedFunctions.InfoHolder result = CopiedFunctions.recreatePoint([snapshot1, snapshot3], 1, 1);

            Assert.AreEqual(result.fitnessValue, 1);
            Assert.AreEqual(result.fitnessEvals, 1);
        }

        [TestMethod]
        public void TestParseTruncatedData()
        {
            IterationSnapshot snapshot1 = new IterationSnapshot();
            snapshot1.BestFitness = 0;
            snapshot1.FitnessEvals = 0;
            snapshot1.Iteration = 0;
            IterationSnapshot snapshot2 = new IterationSnapshot();
            snapshot2.BestFitness = 1;
            snapshot2.FitnessEvals = 1;
            snapshot2.Iteration = 1;
            IterationSnapshot snapshot3 = new IterationSnapshot();
            snapshot3.BestFitness = 2;
            snapshot3.FitnessEvals = 2;
            snapshot3.Iteration = 2;

            AlgorithmRun algorithmRun1 = new AlgorithmRun();
            Detail detail1 = new Detail(); detail1.VariableName = "baba"; detail1.VariableValue = "1";
            Detail detail1run = new Detail(); detail1run.VariableName = "run"; detail1run.VariableValue = "1";
            algorithmRun1.Details = [detail1, detail1run];
            algorithmRun1.IterationSnapshots = [snapshot1, snapshot2, snapshot3];
            AlgorithmRun algorithmRun2 = new AlgorithmRun();
            Detail detail2 = new Detail(); detail2.VariableName = "baba"; detail2.VariableValue = "1";
            Detail detail2run = new Detail(); detail2run.VariableName = "run"; detail2run.VariableValue = "2";
            algorithmRun2.Details = [detail2, detail2run];
            algorithmRun2.IterationSnapshots = [snapshot1, snapshot3];

            DisplayNameFormatter formatter = new DisplayNameFormatter("bubu", [], []);

            List<AlgorithmRun> data = [algorithmRun1,algorithmRun2];

            List<MapleMatrix> result = CopiedFunctions.ParseTruncatedDataFitnessEval(data,false,formatter);

            Assert.AreEqual(result[0].Matrix[0][2], "1");
            Assert.AreEqual(result[0].Matrix[1][2], "1");
            Assert.AreEqual(result[0].Matrix[2][2], "1");

        }
    }

}