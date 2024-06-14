using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataParser.Latex {
    public static class TableGenerator {
        public static string GenerateLatexTable(IEnumerable<SummaryCollection> summaries) {
            var rows = string.Join("\n", summaries.SelectMany(s => s.ToRows()));
            return @$"\begin{{table}}[H]
\begin{{tabular}}{{|l|l|l|l|l|l|}}
\hline
Name & Mean & Median & 25th Percentile & 75th Percentile & Standard deviation \\ \hline
{rows}
\end{{tabular}}
\end{{table}}";
        }
        public static string GenerateLatexTable(IEnumerable<JoinedTable> tables) {
            var rows = string.Join("\n", tables.SelectMany(s => s.ToRows()));
            return @$"\begin{{table}}[]
\begin{{tabular}}{{|l|l|l|l|l|l|}}
\hline
Name & Mean & Median & 25th Percentile & 75th Percentile & Standard deviation \\ \hline
{rows}
\end{{tabular}}
\end{{table}}";
        }

        public static string GenerateLatexTable(IEnumerable<StatisticsSummary> summaries) {
            var rows = string.Join("\n", summaries.Select(s => s.ToRow()));
            return @$"\begin{{table}}[H]
\begin{{tabular}}{{|l|l|l|l|l|l|}}
\hline
Name & Mean & Median & 25th Percentile & 75th Percentile & Standard deviation \\ \hline
{rows}
\end{{tabular}}
\end{{table}}";
        }
    }
}
