using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataParser.Data {

    public class Data
    {
        public struct FitnessEvalAndValuePair
        {
            public FitnessEvalAndValuePair(int fitnessEvals, int fitnessValue)
            {
                this.fitnessEvals = fitnessEvals;
                this.fitnessValue = fitnessValue;
            }

            public int fitnessEvals { get; private set; }
            public int fitnessValue { get; private set; }
        }

        public struct FitnessEvalAndBoundCountPair
        {
            public FitnessEvalAndBoundCountPair(double fitnessEvals, double probBoundCount)
            {
                this.fitnessEvals = fitnessEvals;
                ProbBoundCount = probBoundCount;
            }

            public double fitnessEvals { get; private set; }
            public double ProbBoundCount { get; private set; }
        }

        public struct IterationAndProbBoundCountPair {
            public IterationAndProbBoundCountPair(int iteration, double probBoundCount) {
                Iteration = iteration;
                ProbBoundCount = probBoundCount;
            }

            public int Iteration { get; private set; }
            public double ProbBoundCount { get; private set; }
        }

    }
    public class Detail {
        public string VariableName { get; set; }
        public string VariableValue { get; set; }

        public override bool Equals(object? obj) {
            if (obj is Detail det) {
                return det.VariableValue.Equals(VariableValue) && det.VariableName.Equals(VariableName);
            }
            return false;
        }

        public override int GetHashCode() {
            if (VariableName == null || VariableValue == null) {
                return 0;
            }

            unchecked {
                int hash = VariableName.GetHashCode() * 31 + VariableValue.GetHashCode();
                return hash;
            }
        }

    }

    public class IterationSnapshot {
        public int BestFitness { get; set; }
        public int WorstFitness { get; set; }
        public int MedianFitness { get; set; }
        public int Iteration { get; set; }
        public int FitnessEvals { get; set; }
        public int LowerBoundHits { get; set; }
        public int UpperBoundHits { get; set; }
    }

    public class AlgorithmRun {
        public List<Detail> Details { get; set; }
        public int BestFit { get; set; }
        public int BestFitItr { get; set; }
        public List<IterationSnapshot> IterationSnapshots { get; set; }

        public void ReconstructMissingIterations() {
            List<IterationSnapshot> newSnapshots = new List<IterationSnapshot>();
            for(int i = 0; i < IterationSnapshots.Count - 1; i++) {
                newSnapshots.Add(IterationSnapshots[i]);
                newSnapshots.AddRange(FillGap(IterationSnapshots[i], IterationSnapshots[i + 1]));
            }

            newSnapshots.Add(IterationSnapshots[IterationSnapshots.Count - 1]);

            IterationSnapshots = newSnapshots;
        }

        private static List<IterationSnapshot> FillGap(IterationSnapshot start, IterationSnapshot end) {
            List<IterationSnapshot> newSnapshots = new List<IterationSnapshot>();

            double itrDiff = end.Iteration - start.Iteration;
            double bestFitnessDiffIncr = (end.BestFitness - start.BestFitness) / itrDiff;
            double worstFitnessDiffIncr = (end.WorstFitness - start.WorstFitness) / itrDiff;
            double medianFitnessDiffIncr = (end.MedianFitness - start.MedianFitness) / itrDiff;
            double feDiffIncr = (end.FitnessEvals - start.FitnessEvals) / itrDiff;

            for (int i = 1; i < itrDiff; i++) {
                IterationSnapshot filler = new IterationSnapshot();
                filler.Iteration = start.Iteration + i;
                filler.BestFitness = (int)Math.Round(start.BestFitness + i * bestFitnessDiffIncr);
                filler.WorstFitness = (int)Math.Round(start.WorstFitness + i * worstFitnessDiffIncr);
                filler.MedianFitness = (int)Math.Round(start.MedianFitness + i * medianFitnessDiffIncr);
                filler.FitnessEvals = (int)Math.Round(start.FitnessEvals + i * feDiffIncr);
                newSnapshots.Add(filler);
            }

            return newSnapshots;
        }
    }


}
