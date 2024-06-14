using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataParser.Latex {
    public class SummaryCollection {
        public IStatisticsSummary Primary;

        private List<IStatisticsSummary> secondaries;
        public int tableSize;
        private string description;

        public SummaryCollection(IStatisticsSummary primary, string description) {
            secondaries = new List<IStatisticsSummary>();
            Primary = primary;
            tableSize = Primary.TableSize();
            this.description = description;
        }

        public void Add(StatisticsSummary secondary) {
            if (secondary.TableSize() != tableSize) throw new Exception("Wrong table size");
            secondaries.Add(secondary);
        }

        public IEnumerable<string> ToRows() {
            List<string> s = [@$"\multicolumn{{{tableSize}}}{{|l|}}{{{description}}} \\ \hline", Primary.ToRow(), .. secondaries.Select(x => x.ToRow())];
            s[s.Count - 1] += " \\hline";
            return s;
        }
    }
}
