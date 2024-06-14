using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataParser.Latex {
    public class StatisticsSummary : IStatisticsSummary {
        string Name;
        public double Mean;
        double Median;
        double Percentile25;
        double Percentile75;
        double StandardDeviation;

        public StatisticsSummary(string name, double mean, double median, double percentile25, double percentile75, double standardDeviation) {
            Name = name;
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
            return @$"{Name} & {formatDouble(Mean)} & {formatDouble(Median)} & {formatDouble(Percentile25)} & {formatDouble(Percentile75)} & {formatDouble(StandardDeviation)} \\";
        }

        public int TableSize() {
            return 6;
        }

        public double GetMean() => Mean;
    }
}
