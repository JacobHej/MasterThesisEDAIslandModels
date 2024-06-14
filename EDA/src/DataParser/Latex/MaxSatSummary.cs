using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataParser.Latex {
    public class MaxSatSummary : IStatisticsSummary {
        string Name;
        int SolvedCount;
        public double Mean;
        double Median;
        double Percentile25;
        double Percentile75;
        double StandardDeviation;

        public MaxSatSummary(string name, int solvedCount, double mean, double median, double percentile25, double percentile75, double standardDeviation) {
            Name = name;
            SolvedCount = solvedCount;
            Mean = mean;
            Median = median;
            Percentile25 = percentile25;
            Percentile75 = percentile75;
            StandardDeviation = standardDeviation;
        }

        private string formatDouble(double x) {
            return x.ToString("N", CultureInfo.InvariantCulture);
        }

        public string ToRow() {
            return @$"{Name} & {formatDouble(Mean)} & {formatDouble(Mean)} & {formatDouble(Median)} & {formatDouble(Percentile25)} & {formatDouble(Percentile75)} & {formatDouble(StandardDeviation)} \\";
        }

        public int TableSize() {
            return 7;
        }

        public double GetMean() => Mean;
    }
}
