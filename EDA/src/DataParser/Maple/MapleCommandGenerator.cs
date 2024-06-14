using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataParser.Maple {
    public static class MapleCommandGenerator {
        private static int colorIndex = 0;
        private static string[] colors = {
            "\"Aquamarine\"",
            "\"Black\"",
            "\"BlanchedAlmond\"",
            "\"Blue\"",
            "\"Violet\"",
            "\"Brown\"",
            "\"Burlywood\"",
            "\"CadetBlue\"",
            "\"Chartreuse\"",
            "\"Chocolate\"",
            "\"Coral\"",
            "\"Cornflowerblue\"",
            "\"Crimson\"",
            "\"DarkBlue\"",
            "\"DarkCyan\"",
            "\"DarkGoldenRod\"",
            "\"DarkGray\"",
            "\"DarkGreen\"",
            "\"DarkMagenta\"",
            "\"DarkOrange\"",
            "\"DarkOrchid\"",
        };

        public static string GenerateMapleCommands(IEnumerable<string> paths, IEnumerable<string> displayNames, string xaxis, string yaxis, int xindex, int yindex) {
            if(paths.Count() != displayNames.Count()) throw new ArgumentException("paths and displaynames are of different length");
            
            string fullCommand = "";

            for (int i = 0; i < paths.Count(); i++) {
                string graphName = Path.ChangeExtension(Path.GetFileName(paths.ElementAt(i)), null).Replace("__", "_");
                fullCommand += $"data := Import(base = worksheetdir, \"{paths.ElementAt(i)}\", format = \"CSV\", output = Matrix):\n" +
                $"x := transpose(data[{xindex}]):\n" +
                $"y := transpose(data[{yindex}]):\n" +
                $"{graphName} := " +
                $"dataplot(x[2 .. ()], y[2 .. ()], labels = [\"{xaxis}\", \"{yaxis}\"], " +
                $"legend = \"{displayNames.ElementAt(i)}\", style = line, color = {colors[colorIndex++ % colors.Length]}):";
                fullCommand += "\n";
            }

            fullCommand += "\n\n";
            fullCommand += "display(\n";

            foreach (string path in paths) {
                string graphName = Path.ChangeExtension(Path.GetFileName(path), null).Replace("__", "_");
                fullCommand += graphName + ",\n";
            }

            fullCommand += ")";
            return fullCommand;
        }
    }
}
