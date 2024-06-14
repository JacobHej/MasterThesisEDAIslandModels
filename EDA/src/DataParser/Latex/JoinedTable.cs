using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataParser.Latex {
    public class JoinedTable { 
        private List<SummaryCollection> collections;
        private string description;
        private int tableSize;

        public JoinedTable(string description, List<SummaryCollection> collections) {
            this.collections = collections;
            this.description = description;
            this.tableSize = collections.First().tableSize;
        }

        public void Add(SummaryCollection secondary) {
            collections.Add(secondary);
        }

        public IEnumerable<string> ToRows() {
            List<string> s = [@$"\multicolumn{{{tableSize}}}{{|l|}}{{{description}}} \\ \hline", .. collections.SelectMany(x => x.ToRows())];
            s[s.Count - 1] += " \\hline";
            return s;
        }
    }
}
