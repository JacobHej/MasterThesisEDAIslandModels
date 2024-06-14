using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataParser.Data.Selection
{
    public static class IEnumerableExtensions
    {
        public static IEnumerable<Detail> Exclude(this IEnumerable<Detail>? source, params string[] excludes)
        {
            return source.Where(detail => !excludes.Contains(detail.VariableName));
        }
    }
}
