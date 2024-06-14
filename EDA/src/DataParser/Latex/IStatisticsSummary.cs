using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataParser.Latex {
    public interface IStatisticsSummary {
        public int TableSize();
        public string ToRow();

        public double GetMean();
    }
}
