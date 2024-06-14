using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataParser.Maple {
    public class MapleMatrix {
        public readonly string Name;
        public readonly string DisplayName;
        public readonly List<List<string>> Matrix;

        public MapleMatrix(string name, string displayname) {
            DisplayName = displayname;
            Name = name;
            Matrix = new List<List<string>>();
        }

        public void AppendRow(List<string> row) => Matrix.Add(row);

        public void AppendRow<T>(string name, IEnumerable<T> values) {
            if (!(values.ElementAt(0) is int) && !(values.ElementAt(0) is string) && !(values.ElementAt(0) is double)) {
                throw new Exception("Unsupported datatype: " + typeof(T).FullName);
            }

            AppendRow([name, .. values.Select(ParseValue)]);
        }

        private string ParseValue<T>(T value) {
            return value switch {
                int x => x.ToString(System.Globalization.CultureInfo.InvariantCulture),
                double x => x.ToString(System.Globalization.CultureInfo.InvariantCulture),
                string x => x.ToString(System.Globalization.CultureInfo.InvariantCulture),
                _ => throw new Exception("Cant parse type: " + typeof(T).FullName)
            };
        }
    }
}
