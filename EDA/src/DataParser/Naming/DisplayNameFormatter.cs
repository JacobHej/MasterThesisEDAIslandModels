using DataParser.Data.Selection;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataParser.Naming {
    public class DisplayNameFormatter {
        private List<VariableFormat> formats;
        private string description;
        public string Description => description;

        public DisplayNameFormatter()
        {
            description = "";
            formats = new List<VariableFormat>();
        }

        public DisplayNameFormatter(string[] names, string[] ids) : this()
        {
            AddFormats(names, ids);
        }

        public DisplayNameFormatter(string description)
        {
            this.description = description;
            formats = new List<VariableFormat>();
        }

        public DisplayNameFormatter(string description, string[] names, string[] ids) : this(description)
        {
            AddFormats(names, ids);
        }

        public static DisplayNameFormatter MigrationIntervalFormatter(string description) {
            return new DisplayNameFormatter(description,["Migration Interval: "], ["mi"]);
        }

        

        public void AddFormats(string[] names, string[] ids) {
            if (names.Length != ids.Length) throw new ArgumentException("Names and IDs was of different length");

            for(int i = 0; i < names.Length; i++) {
                AddFormat(names[i], ids[i]);
            }
        }

        public void AddFormat(string name, string id) {
            formats.Add(new VariableFormat() { VariableName = name, VariableID = id });
        }

        public string FormatName(Configuration config) {
            string name = description;

            formats.ForEach(
                x => {
                    if (config.Vars.Any(v => v.VariableName.Equals(x.VariableID))) {
                        name += $"{x.VariableName}{config.Vars.Single(v => v.VariableName.Equals(x.VariableID)).VariableValue}";
                    }
                });

            return name;
        }

        private class VariableFormat {
            public string VariableName;
            public string VariableID;
        }
    }
}
