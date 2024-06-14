using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataParser.Data.Selection
{
    public class Configuration {
        public List<Detail> Vars;

        public Configuration(AlgorithmRun algo, List<string> parameters) {
            Vars = parameters.Select(param => algo.Details.First(detail => detail.VariableName == param)).OrderBy(detail => detail.VariableName).ToList();
        }

        public override bool Equals(object? obj) {
            if (obj is Configuration conf) {

                if (conf.Vars.Count() != Vars.Count()) return false;

                for (int i = 0; i < Vars.Count(); i++) {
                    if (!Vars[i].Equals(conf.Vars[i]))  return false;
                }

                return true;
            }

            return false;
        }

        public override int GetHashCode() {
            if (Vars == null) {
                return 0;
            }

            unchecked {
                int hash = 19;

                foreach (Detail detail in Vars) {
                    hash = hash * 31 + detail.GetHashCode();
                }

                return hash;
            }
        }

        public override string ToString() {
            return string.Join("_", Vars.Select(var => $"{var.VariableName}_{var.VariableValue}"));
        }

        public string ToSimpleString(bool algorithm = false, bool problem = false, bool topology = false, bool migrationStrategy = false)
        {
            string name = "";
            foreach(var variable in Vars)
            {
                switch (variable.VariableName)
                {
                    case "optimizer":
                        if (!algorithm) continue;
                        break;
                    case "fitnessFunction":
                        if (!problem) continue;
                        break;
                    case "topology":
                        if (!topology) continue;
                        break;
                    case "ms":
                        if (!migrationStrategy) continue;
                        break;
                }
                name += variable.VariableName + "_" + variable.VariableValue;
            }
            name = name.Replace(" ", "");
            name = name.Replace("(", "");
            name = name.Replace(")", "");
            return name;
        }

        public string ToSmallString()
        {
            return string.Join("_", Vars.Select(var => $"{var.VariableName.Substring(0,Math.Min(3,var.VariableName.Length))}_{var.VariableValue.Substring(0, Math.Min(3, var.VariableValue.Length))}"));
        }

        public static Func<AlgorithmRun, Configuration> Exclude(params string[] excluding) {
            return new Func<AlgorithmRun, Configuration>((run) => new Configuration(run, run.Details.Exclude(excluding).Select(detail => detail.VariableName).ToList()));
        }

        public static Func<AlgorithmRun, Configuration> Include(params string[] including) {
            return new Func<AlgorithmRun, Configuration>((run) => new Configuration(run, new List<string>(including)));
        }
    }
}
