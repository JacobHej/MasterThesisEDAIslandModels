using DataParser.Maple;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataParser.Data {
    public static class DataWriter {
        public static void AppendText(string path, string text) {
            if(!File.Exists(path)) TouchFile(path);
            File.AppendAllText(path, text);
        }

        public static void WriteText(string path, string text) {
            TouchFile(path);
            File.WriteAllText(path, text);
        }

        public static void WriteMapleMatrix(string path, MapleMatrix matrix) {
            TouchFile(path);
            foreach (var row in matrix.Matrix) File.AppendAllText(path, string.Join(", ", row)+"\n");
        }

        private static void TouchFile(string path) {
            EnsurePath(path);
            File.WriteAllText(path, string.Empty);
        }

        private static void EnsurePath(string path) {
            if (!Directory.Exists(Path.GetDirectoryName(path))) {
                Directory.CreateDirectory(Path.GetDirectoryName(path));
            }
        }
    }
}
