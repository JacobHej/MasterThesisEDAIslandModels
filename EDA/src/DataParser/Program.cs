using DataParser.Data;
using DataParser.Naming;
using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Compression;
using System.Linq;
using System.Numerics;
using System.Reflection.Emit;
using System.Runtime.InteropServices;
using System.Security.Cryptography;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;
using static System.Runtime.InteropServices.JavaScript.JSType;

namespace DataParser
{
    public static class Program
    {
        public static void Main(string[] args)
        {
            ParseTwoPeaks();
            //ParseStatisticTestTable();
            //ParseOneMaxLeadingOnes();
            return;
            Parser parser;
            string description;
            DisplayNameFormatter formatter;

            int problem = 2;
            int algorithm = 0;
            string[] algorithms = ["UMDA", "CGA"];
            string[] problems = ["OneMax", "LeadingOnes", "TwoPeaks", "MaxSat", "Jumpk"];
            string[] topologies = ["Complete", "Taurus", "KCircle", "Star"];
            string[] migrationStrategies = ["Sample", "MuSample", "Prob"];
            string folder = problems[problem];
            //folder = "JumpK";
            folder = "Collected";
            string[] lg_paradigm = ["lg", "sqrt_lg"];

            string[] extraFormat = [];
            string[] extraDetail = [];


            //extraFormat = ["jumpK: "];
            //extraDetail = ["jumpK"];


            //cga_jumpk_experiment2();
            //cga_jumpk_experiment3();


            //extraFormat = ["_N_","_Lambda_"];
            //extraDetail = ["n","lambda"];
            //parseStatistics(folder, algorithms[0], problems[0], topologies[0], "NoMigration", extraFormat, extraDetail,"NTest");

            //extraFormat = ["_Lambda_", "_MI_"];
            //extraDetail = ["lambda","mi"];
            //parseStatistics(folder, algorithms[0], problems[0], topologies[0], migrationStrategies[0], extraFormat, extraDetail, "MiTest");
            //parseStatistics(folder, algorithms[0], problems[0], topologies[0], migrationStrategies[1], extraFormat, extraDetail, "MiTest");

            //extraFormat = ["_N_"];
            //extraDetail = ["n"];
            //parseStatistics(folder, algorithms[0], problems[1], topologies[0], "NoMigration", extraFormat, extraDetail, "NTest");
            //extraFormat = ["_MI_"];
            //extraDetail = ["mi"];
            //parseStatistics(folder, algorithms[0], problems[1], topologies[0], migrationStrategies[0], extraFormat, extraDetail, "MiTest");



            //extraFormat = ["_N_"];
            //extraDetail = ["n"];
            //parseStatistics(folder, algorithms[0], problems[2], topologies[0], "NoMigration", extraFormat, extraDetail, "NTest");
            //extraFormat = ["_MI_"];
            //extraDetail = ["mi"];
            //parseStatistics(folder, algorithms[0], problems[2], topologies[0], migrationStrategies[0], extraFormat, extraDetail, "MiTest");

            //dirCleaner(folder, "*cluster_test*");
            //dirLister(folder, "*jumpKTest*");
            //dirCleaner(folder, "*jumpKTest*");
            //jumpKMover(folder, algorithms[algorithm], problems[problem]);
            //fileCleaner(folder);
            //ErikFileUnzipper(folder);

            //parseLambda(folder, algorithms[algorithm], problems[problem], extraFormat, extraDetail);
            //parseK(folder, algorithms[algorithm], problems[problem], extraFormat, extraDetail);
            //parseLambdaOverN(folder, algorithms[algorithm], problems[problem]);

            //parseMi(folder, algorithms[0], problems[0], "Complete", "Sample");

            //parseIterations(folder, algorithms[0], problems[0], topologies[0], migrationStrategies[0], extraFormat, extraDetail);
            //parseN(folder, algorithms[0], problems[1], topologies[0], migrationStrategies[1]);
            //parseLessN(folder, algorithms[0], problems[1], topologies[0], migrationStrategies[1]);
            //parseNSeriel(folder, algorithms[0], problems[problem]);


            //parseLatexData(folder, algorithms[algorithm], problems[problem], "LambdaTestSpecific40");
            //parseNSeriel(folder, algorithms[algorithm], problems[problem], "Mu1Lambda13NTest");
            //parseN(folder, algorithms[algorithm], problems[problem], topologies[0], migrationStrategies[1], "Mu1Lambda13NTest");
            //parseN(folder, algorithms[algorithm], problems[problem], topologies[0], migrationStrategies[2], "Mu1Lambda13NTest");


            //parseMi(folder, algorithms[algorithm], problems[problem], topologies[0], "NoMigration");

            //parseN(folder, algorithms[algorithm], problems[problem], topologies[0], "NoMigration");
            //parseCluster(folder, algorithms[algorithm], problems[problem], topologies[0], "NoMigration");
            //parseJumpK(folder, algorithms[algorithm], problems[problem], topologies[0], "NoMigration");

            //parseSpecificMi(folder, algorithms[algorithm], problems[problem], "SpecificMiTest");
            //parseSpecificMi(folder, algorithms[algorithm], problems[problem], topologies[0], "NoMigration", "SpecificMiTest");
            string path = "D:/Workspaces/MasterEDA2/CSVResults/RawDataCGATwoPeaks.txt";
            //System.IO.File.WriteAllText(path, string.Empty);

            //extraFormat = ["_N_","_Lambda_"];
            //extraDetail = ["n","lambda"];
            //string n = "100";
            //string lambda = "16";
            //parseStatistics(folder, algorithms[algorithm], problems[problem], topologies[0], "NoMigration", extraFormat, extraDetail, "NTest", "*n_"+ n +"*lambda_"+"15"+"*.txt");

            string[] miValues = [
                "1","1","1",
                "1","1","1",
                "1","1","1",
                "1","1","1",
            ];
            int miIndex = 0;

            //foreach (string topology in topologies)
            //{
            //    foreach (string migrationStrategy in migrationStrategies)
            //    {


            //        extraFormat = ["_Mi_", "_Lambda_"];
            //        extraDetail = ["mi", "lambda"];
            //        parseStatistics(folder, algorithms[algorithm], problems[problem], topology, migrationStrategy, extraFormat, extraDetail, "MiTest", "*lambda_"+ lambda+"_*mi_" + miValues[miIndex] +"_*.txt");
            //        miIndex++;

            //        //parseSpecificMi(folder, algorithms[algorithm], problems[problem], topology, migrationStrategy, "SpecificMiTest");
            //        //parseMi(folder, algorithms[algorithm], problems[problem], topology, migrationStrategy);
            //        //parseN(folder, algorithms[algorithm], problems[problem], topology, migrationStrategy);
            //        //parseCluster(folder, algorithms[algorithm], problems[problem], topology, migrationStrategy);
            //        //parseJumpK(folder, algorithms[algorithm], problems[problem], topology, migrationStrategy);

            //        //dirListerNoHelp("Collected/Results/UMDA/"+problems[problem] + "/" +topology+"/"+migrationStrategy+"/SpecificMiTest/", "*");
            //        //dirCleanerNoHelp("Collected/Results/UMDA/" + problems[problem] + "/" + topology + "/" + migrationStrategy + "/SpecificMiTest/", "*");
            //    }
            //}

            //RCommandsErik(algorithms[algorithm], problems[problem], topologies, migrationStrategies,
            //    [
            //    "_Mi_1_Lambda_367", "_Mi_1_Lambda_367", "_Mi_1_Lambda_367",
            //    "_Mi_1_Lambda_367", "_Mi_1_Lambda_367", "_Mi_1_Lambda_367",
            //    "_Mi_1_Lambda_367", "_Mi_1_Lambda_367", "_Mi_1_Lambda_367",
            //    "_Mi_3_Lambda_367", "_Mi_16_Lambda_367", "_Mi_3_Lambda_367"
            //    ], "1000_Lambda_367", "sqrt(n)log(n) paradigm");

            //CGA_TwoPeaks_Complete_Sample_MI_985_LargestFitness

            //RCommandsErik(path,algorithms[algorithm], problems[problem], topologies, migrationStrategies,
            //    [
            //    "_Mi_"+miValues[0]+"_Lambda_"+ lambda,"_Mi_"+miValues[1]+"_Lambda_"+ lambda,"_Mi_"+miValues[2]+"_Lambda_"+ lambda,
            //    "_Mi_"+miValues[3]+"_Lambda_"+ lambda,"_Mi_"+miValues[4]+"_Lambda_"+ lambda,"_Mi_"+miValues[5]+"_Lambda_"+ lambda,
            //    "_Mi_"+miValues[6]+"_Lambda_"+ lambda,"_Mi_"+miValues[7]+"_Lambda_"+ lambda,"_Mi_"+miValues[8]+"_Lambda_"+ lambda,
            //    "_Mi_"+miValues[9]+"_Lambda_"+ lambda,"_Mi_"+miValues[10]+"_Lambda_"+ lambda,"_Mi_"+miValues[11]+"_Lambda_"+ lambda,
            //    ], "_N_"+n+"_Lambda_"+"15", "");


            //parseNull(folder, algorithms[algorithm], problems[problem]);
            //string[] migrationStrategies = ["Prob"];
            //string[] migrationStrategies = ["MuSample"];
            //parser = new Parser("CGA" + "\\" + problems[problem] + "\\" + "Ck");
            //string description = "CGA" + "-" + problems[problem] + "-" + "Ck";
            //formatter = new DisplayNameFormatter(description, [" cK: "], ["cK"]);
            //parser.ParseTruncatedDataFitnessEval(true, formatter);
            //parser.ParseTruncatedDataFitnessEval(false, formatter);
            //parser.LatexData(10, formatter, true);

            //string description = "UMDA-" + problems[1] + "-" + topologies[0] + "-" + migrationStrategies[0] + "";
            //parser = new Parser("UMDA" + "/" + problems[1] + "/" + topologies[0] + "/" + migrationStrategies[0] + "/" + "MiTest", "holder\\");
            //DisplayNameFormatter formatter = new DisplayNameFormatter(description, ["Mi: "], ["mi"]);
            //parser.ParseTruncatedDataFitnessEval(true, formatter);
            //parser.ParseTruncatedDataFitnessEval(false, formatter);
            //parser.LatexData(10, formatter, true);
            //parser.ParseDataFitnessEval(true, formatter);
            //parser.ParseDataFitnessEval(false, formatter);
            //parser.LatexData(10, formatter, true);

            //Parser mitestparse = new Parser("UMDAOneMaxNullTest");
            //mitestparse.ParseDataFitnessEval(true, new DisplayNameFormatter("Parallel"));
            //mitestparse.ParseDataFitnessEval(false, new DisplayNameFormatter("Parallel"));
            //mitestparse.LatexData(10,new DisplayNameFormatter("Parallel"));



            //parser = new Parser("");
            //description = "Test";
            //formatter = new DisplayNameFormatter(description, ["K: ", " N: "], ["mi", "n"]);
            //parser.parseProbDist(formatter);



            //parser = new Parser(algorithms[algorithm] + "\\" + problems[problem] + "\\" + "Ck");
            //description = algorithms[algorithm] + "-" + problems[problem] + "-" + "Ck";
            //formatter = new DisplayNameFormatter(description, ["K: "], ["cK"]);
            //parser.parseConst(formatter, "cK");
            //parser.LatexData(200, formatter, true);
            //parser.ParseMaxSatLatexData(formatter, ["1", "2", "3", "4", "5", "6",], 10);

            //foreach (string paradigm in lg_paradigm) {

            //description = algorithms[algorithm] + "-" + paradigm + "-" + problems[problem] + "NullMigration ";
            //parser = new Parser(algorithms[algorithm] + "/" + paradigm + "/" + problems[problem] + "/" + "NullMigration");
            //formatter = new DisplayNameFormatter(description, ["Mi: "], ["mi"]);
            //parser.LatexData(10, formatter, true);

            //string dir = "D:/Workspaces/MasterEDA2/Results/" + algorithms[algorithm] + "/" + paradigm + "/" + problems[problem] + "/" + "NTestSeriel";
            //if (!Directory.Exists(dir)) continue;
            //description = algorithms[algorithm] + "-" + paradigm + "-" + problems[problem] + "NTestSeriel ";
            //parser = new Parser(algorithms[algorithm] + "/" + paradigm + "/" + problems[problem] + "/" + "NTestSeriel");
            //formatter = new DisplayNameFormatter(description, ["N: "], ["n"]);
            //parser.LatexData(int.MaxValue, formatter, true);
            //parser.parseNTest(formatter);

            //foreach (string topology in topologies) {
            //    foreach (string migrationStrategy in migrationStrategies) {
            //        description = algorithms[algorithm] + "-" + paradigm + "-" + problems[problem] + "-" + topology + "-" + migrationStrategy + " ";

            //        string dir = "D:/Workspaces/MasterEDA2/Results/" + algorithms[algorithm] + "/" + paradigm + "/" + problems[problem] + "/" + topology + "/" + migrationStrategy + "/" + "ClusterSizeTest";
            //        if (!Directory.Exists(dir)) continue;

            //        parser = new Parser(algorithms[algorithm] + "/" + paradigm + "/" + problems[problem] + "/" + topology + "/" + migrationStrategy + "/" + "ClusterSizeTest");
            //        formatter = new DisplayNameFormatter(description, ["Cluster Size: ", " N: ", " K: ", " Mi: "], ["cluster_size", "n", "cK", "mi"]);
            //        //parser.parseProbDist(formatter);
            //        parser.LatexData(10, formatter, true);
            //        parser.parseConst(formatter, "cluster_size");
            //        //parser.parseNTest(formatter);
            //    }
            //}










            //foreach (string topology in topologies)
            //{

            //    foreach (string migrationStrategy in migrationStrategies)
            //    {
            //        string directory = problems[2] + "\\" + topology + "\\" + migrationStrategy + "\\" + "MiTest";
            //        string dir = Path.Combine("D:\\Workspaces\\Master\\Results\\", directory);
            //        if (!Directory.Exists(dir))
            //        {
            //            Console.WriteLine($"Directory not found {{{dir}}}");
            //            continue;
            //        }
            //        parser = new Parser(directory);
            //        //parser = new Parser("UMDAOneMax" + topology + "" + migrationStrategy + "Test");
            //        //parser = new Parser("UMDA" + "\\" + problems[1] + "\\" + topology + "\\" + migrationStrategy + "\\" + "NTest");

            //        string description = problems[2] + "-" + topology + "-" + migrationStrategy;

            //        DisplayNameFormatter formatter = new DisplayNameFormatter(description, ["Mi: "], ["mi"]);
            //        //DisplayNameFormatter formatter = new DisplayNameFormatter(description, ["Cluster size: "], ["cluster_size"]);
            //        //DisplayNameFormatter formatter = new DisplayNameFormatter(description, [], []);
            //        //parser.ParseDataFitnessEval(true, formatter);
            //        //parser.ParseDataFitnessEval(false, formatter);
            //        //parser.LatexData(10, formatter, true);
            //        parser.ParseMaxSatLatexData(formatter, ["1", "3", "5"], 2);
            //        //parser.ParseDataOverN(formatter);
            //    }
            //}


            //parser = new Parser("UMDAOneMaxCompleteNoMigrationNTest");
            //string description = "UMDA-OneMax-Complete-NoMigration";

            ////DisplayNameFormatter formatter = new DisplayNameFormatter(description, ["size: "], ["cluster_size"]);
            ////parser.ParseDataFitnessEval(true, formatter);
            ////parser.ParseDataFitnessEval(false, formatter);
            ////parser.LatexData(10, formatter);

            //DisplayNameFormatter formatter = new DisplayNameFormatter(description, [], []);
            //parser.ParseDataOverN(true, formatter);
            //parser.ParseDataOverN(false, formatter);
            //DataParser.ParseDataOverN("UMDAOneMaxCompleteSampleMigrationStrategyNTest", true);
            //DataParser.ParseDataOverN("UMDAOneMaxCompleteSampleMigrationStrategyNTest", false);

            //DataParser.ParseDataFitnessEval("UMDAcMcLessLeadingOnes", true);//ClCm
            //DataParser.ParseDataFitnessEval("UMDAcMcLessTwoPeaks", true);//ClCm
            //DataParser.ParseDataFitnessEval("UMDAcMcLessTwoPeaks", false);//ClCm
        }

        public static void ParseStatisticTestTable() {
            int problem = 3;
            int algorithm = 1;
            string[] algorithms = ["UMDA", "CGA"];
            string[] problems = ["OneMax", "LeadingOnes", "TwoPeaks", "MaxSat", "Jumpk"];
            string[] topologies = ["Complete", "KCircle", "Star", "Taurus"];
            string[] migrationStrategies = ["Sample", "MuSample", "Prob"];
            string[] lg_paradigm = ["lg", "sqrt_lg"];
            string dir;

            string path = "D:/Workspaces/MasterEDA2/CSVResults/RawData.txt";

            // twopeaks
            //string[] miValues = [
            //    "1", "2216", "38",
            //    "1", "2216", "3325",
            //    "194", "2216", "2",
            //    "1", "437", "3325",
            //];

            //max sat cga instance 4
            string[] miValues = [
                "8", "8", "128",
                "4", "4", "128",
                "256", "2048", "65536",
                "4", "16", "128",
            ];


            RCommandsErik(path, algorithms[algorithm], problems[problem], topologies, migrationStrategies,
                [
                "_MI_" + miValues[0] + "_Instance_4",
                    "_MI_" + miValues[1] + "_Instance_4",
                    "_MI_" + miValues[2] + "_Instance_4",
                    "_MI_" + miValues[3] + "_Instance_4",
                    "_MI_" + miValues[4] + "_Instance_4",
                    "_MI_" + miValues[5] + "_Instance_4",
                    "_MI_" + miValues[6] + "_Instance_4",
                    "_MI_" + miValues[7] + "_Instance_4",
                    "_MI_" + miValues[8] + "_Instance_4",
                    "_MI_" + miValues[9] + "_Instance_4",
                    "_MI_" + miValues[10] + "_Instance_4",
                    "_MI_" + miValues[11] + "_Instance_4",
                ], "_MI_1_Instance_4", "");


            //LeadingOnes
            //string[] miValues = [
            //    "1", "1", "129",
            //    "1", "1", "57",
            //    "1", "656", "985",
            //    "1", "1", "129",
            //];

            ////CGA_LeadingOnes_Taurus_Prob_sqrt_lg_MI_3325_LargestFitness
            //RCommandsErik(path, algorithms[algorithm], problems[problem], topologies, migrationStrategies,
            //    [
            //        "_sqrt_lg_MI_" + miValues[0],
            //        "_sqrt_lg_MI_" + miValues[1],
            //        "_sqrt_lg_MI_" + miValues[2],
            //        "_sqrt_lg_MI_" + miValues[3],
            //        "_sqrt_lg_MI_" + miValues[4],
            //        "_sqrt_lg_MI_" + miValues[5],
            //        "_sqrt_lg_MI_" + miValues[6],
            //        "_sqrt_lg_MI_" + miValues[7],
            //        "_sqrt_lg_MI_" + miValues[8],
            //        "_sqrt_lg_MI_" + miValues[9],
            //        "_sqrt_lg_MI_" + miValues[10],
            //        "_sqrt_lg_MI_" + miValues[11],
            //    ], "_sqrt_lg", "");


            //CGA_OneMax_Complete_Sample_lg_MI_5_LargestFitness
            //OneMax
            // sqrt lg

            //string[] miValues = [
            //    "1", "11", "129",
            //    "1", "3", "57",
            //    "1", "11", "291",
            //    "2", "7", "86",
            //];
            //string[] miValuessqrtlg = [
            //    "1", "3325", "25",
            //    "1", "3325", "25",
            //    "1", "194", "656",
            //    "1", "3325", "17",
            //];

            //CGA_LeadingOnes_Taurus_Prob_sqrt_lg_MI_3325_LargestFitness
            //CGA_OneMax_Complete_NoMigration_lg_LargestFitness
            //RCommandsErik(path, algorithms[algorithm], problems[problem], topologies, migrationStrategies,
            //    [
            //        "_lg_MI_" + miValues[0],
            //        "_lg_MI_" + miValues[1],
            //        "_lg_MI_" + miValues[2],
            //        "_lg_MI_" + miValues[3],
            //        "_lg_MI_" + miValues[4],
            //        "_lg_MI_" + miValues[5],
            //        "_lg_MI_" + miValues[6],
            //        "_lg_MI_" + miValues[7],
            //        "_lg_MI_" + miValues[8],
            //        "_lg_MI_" + miValues[9],
            //        "_lg_MI_" + miValues[10],
            //        "_lg_MI_" + miValues[11],
            //    ], "_lg", "log(n) paradigm");
        }

        public static void parseStatistics(string folder, string algorithm, string problem, string topology, string migrationStrategy, string[] extraFormat, string[] extraDetail, string testName = "NTest", string searchPattern = "*lambda_17*.txt")
        {
            string description = algorithm + "-" + problem + "-" + topology + "-" + migrationStrategy;
            Parser parser = new Parser(algorithm + "/" + problem + "/" + topology + "/" + migrationStrategy + "/" + testName, folder + "/", searchPattern);
            var formats = extraFormat.ToList();
            var details = extraDetail.ToList();
            DisplayNameFormatter formatter = new DisplayNameFormatter(description, formats.ToArray(), details.ToArray());
            parser.LatexData(500, formatter, new DisplayNameFormatter(description.Replace("-", "_"), formats.ToArray(), details.ToArray()), true);
        }

        public static void RCommandsErik(string path, string algorithm, string problem, string[] topologies, string[] migrationStrategies, string[] extras, string n, string extraTitelText)
        {
            //df1 < -CGA_TwoPeaks_NullMigration_LargestFitness
            //df2 < -CGA_TwoPeaks_Complete_Sample_MI_1_LargestFitness

            //wilcox.test_result < -wilcox.test(df1, df2)
            //print(wilcox.test_result$p.value)
            //myVar < -wilcox.test_result$p.value
            //cat(myVar, "Hej med dig \n ny linje", myVar)
            int i = 0; int j = 0;

            foreach (string topology1 in topologies)
            {
                foreach (string migrationStrategy1 in migrationStrategies)
                {
                    //UMDA_TwoPeaks_Complete_NoMigration_n_70_LargestFitness
                    DataWriter.AppendText(path, $"df1 <-{algorithm}_{problem}_{topology1}_{migrationStrategy1}{extras[i]}_LargestFitness \n");
                    DataWriter.AppendText(path, $"df2 <-{algorithm}_{problem}_Complete_NoMigration{n}_LargestFitness \n");
                    DataWriter.AppendText(path, $"wilcox.test_result <- wilcox.test(df1, df2) \n");
                    DataWriter.AppendText(path, $"{topology1}{migrationStrategy1}VNoMigration <- wilcox.test_result$p.value \n\n\n");
                    foreach (string topology2 in topologies)
                    {
                        foreach (string migrationStrategy2 in migrationStrategies)
                        {
                            if (topology1 != topology2 && migrationStrategy1 != migrationStrategy2)
                            {
                                j++;
                                continue;
                            }
                            //UMDA_TwoPeaks_Complete_Sample_MI_42_LargestFitness
                            DataWriter.AppendText(path, $"df1 <-{algorithm}_{problem}_{topology1}_{migrationStrategy1}{extras[i]}_LargestFitness \n");
                            DataWriter.AppendText(path, $"df2 <-{algorithm}_{problem}_{topology2}_{migrationStrategy2}{extras[j]}_LargestFitness \n");
                            DataWriter.AppendText(path, $"wilcox.test_result <- wilcox.test(df1, df2) \n");
                            DataWriter.AppendText(path, $"{topology1}{migrationStrategy1}V{topology2}{migrationStrategy2} <- wilcox.test_result$p.value \n\n\n");
                            //wilcox.test_result <- wilcox.test(df1, df2)
                            //myVar < -wilcox.test_result$p.value
                            j++;
                        }
                    }
                    i++;
                    j = 0;
                }
            }

            //Print topology tables

            foreach (string topology in topologies)
            {
                //Start table
                string topologyName = topology == "Taurus" ? "Torus" : topology;
                string s = "cat(\"";
                s += "\\\\begin{table}[H]\\n";
                s += "\\\\begin{tabular}{|llll|}\\n";
                s += "\\\\hline\\n";
                string TableName = algorithm + " " + problem + " " + topologyName + " " + extraTitelText;
                s += "\\\\multicolumn{4}{|c|}{" + TableName + "} \\\\\\\\ \\\\hline \\n";

                //line of strategy names
                s += "\\\\multicolumn{1}{|l|}{} ";
                foreach (string migrationStrategy in migrationStrategies)
                {
                    s += "& \\\\multicolumn{1}{|l|}{" + migrationStrategy + "} ";
                }
                s += "\\\\\\\\ \\\\hline \\n";

                //No migration line
                s += "\\\\multicolumn{1}{|l|}{NoMigration} ";
                foreach (string migrationStrategy2 in migrationStrategies)
                {
                    s += "& \\\\multicolumn{1}{|l|}{\"," + topology + migrationStrategy2 + "VNoMigration,\"} ";
                }
                s += "\\\\\\\\ \\\\hline \\n";


                foreach (string migrationStrategy1 in migrationStrategies)
                {
                    s += "\\\\multicolumn{1}{|l|}{" + migrationStrategy1 + "} ";
                    foreach (string migrationStrategy2 in migrationStrategies)
                    {
                        s += "& \\\\multicolumn{1}{|l|}{\"," + topology + migrationStrategy1 + "V" + topology + migrationStrategy2 + ",\"} ";
                    }
                    s += "\\\\\\\\ \\\\hline \\n";
                }

                s += "\\\\end{tabular}\\n";
                s += "\\\\caption{ Table of p-values calculated with Mann-Whitney U-Tests comparing different migration strategies against eachother with topology: "+ topologyName + "}\\n";
                s += "\\\\label{table:" + TableName.Replace(" ","_") + "_MannWittTable}\\n";
                s += "\\\\end{table}\\n";
                s += "\") \n\n\n";
                DataWriter.AppendText(path, s);

                //\caption{ Table of success rates}
                //\label{ umdajumpKSuccessRate}
            }

            foreach (string migrationStrategy in migrationStrategies)
            {
                //Start table
                string s = "cat(\"";
                s += "\\\\begin{table}[H]\\n";
                s += "\\\\begin{tabular}{|lllll|}\\n";
                s += "\\\\hline\\n";
                string TableName = algorithm + " " + problem + " " + migrationStrategy + " " + extraTitelText;
                s += "\\\\multicolumn{5}{|c|}{" + TableName + "} \\\\\\\\ \\\\hline \\n";

                //line of strategy names
                s += "\\\\multicolumn{1}{|l|}{} ";
                foreach (string topology in topologies)
                {
                    string topologyName = topology == "Taurus" ? "Torus" : topology;
                    s += "& \\\\multicolumn{1}{|l|}{" + topologyName + "} ";
                }
                s += "\\\\\\\\ \\\\hline \\n";

                //No migration line
                s += "\\\\multicolumn{1}{|l|}{NoMigration} ";
                foreach (string topology in topologies)
                {
                    string topologyName = topology == "Taurus" ? "Torus" : topology;
                    s += "& \\\\multicolumn{1}{|l|}{\"," + topology + migrationStrategy + "VNoMigration,\"} ";
                }
                s += "\\\\\\\\ \\\\hline \\n";


                foreach (string topology1 in topologies)
                {
                    string topologyName1 = topology1 == "Taurus" ? "Torus" : topology1;
                    s += "\\\\multicolumn{1}{|l|}{" + topologyName1 + "} ";
                    foreach (string topology2 in topologies)
                    {
                        string topologyName2 = topology2 == "Taurus" ? "Torus" : topology2;
                        s += "& \\\\multicolumn{1}{|l|}{\"," + topology1 + migrationStrategy + "V" + topology2 + migrationStrategy + ",\"} ";
                    }
                    s += "\\\\\\\\ \\\\hline \\n";
                }

                s += "\\\\end{tabular}\\n";
                s += "\\\\caption{ Table of p-values calculated with Mann-Whitney U-Tests comparing different topologies against eachother with migration strategy: " + migrationStrategy + "}\\n";
                s += "\\\\label{table:" + TableName.Replace(" ","_") + "_MannWittTable}\\n";
                s += "\\\\end{table}\\n";
                s += "\") \n\n\n";
                DataWriter.AppendText(path, s);
            }
            //\begin{table}[]
            //\begin{tabular}{|llll|}
            //\hline
            //\multicolumn{4}{|c|}{Algorithm Problem Complete}                                                                        \\ \hline
            //\multicolumn{1}{|l|}{}            & \multicolumn{1}{l|}{Sample} & \multicolumn{1}{l|}{MuSample} & Probability           \\ \hline
            //\multicolumn{1}{|l|}{NoMigration} & \multicolumn{1}{l|}{}       & \multicolumn{1}{l|}{}         &                       \\ \hline
            //\multicolumn{1}{|l|}{Sample}      & \multicolumn{1}{l|}{}       & \multicolumn{1}{l|}{}         &                       \\ \hline
            //\multicolumn{1}{|l|}{MuSample}    & \multicolumn{1}{l|}{}       & \multicolumn{1}{l|}{}         &                       \\ \hline
            //\multicolumn{1}{|l|}{Probability} & \multicolumn{1}{l|}{}       & \multicolumn{1}{l|}{}         &                       \\ \hline
            //\multicolumn{1}{|c|}{Sample}      & \multicolumn{1}{c|}{}       & \multicolumn{1}{c|}{}         & \multicolumn{1}{c|}{} \\ \hline
            //\end{tabular}
            //\end{table}

            //\begin{table}[]
            //\begin{tabular}{|lllll|}
            //\hline
            //\multicolumn{5}{|c|}{Algorithm Problem Sample}                                                                                      \\ \hline
            //\multicolumn{1}{|l|}{}            & \multicolumn{1}{l|}{Complete} & \multicolumn{1}{l|}{Torus} & \multicolumn{1}{l|}{Circle} & Star \\ \hline
            //\multicolumn{1}{|l|}{NoMigration} & \multicolumn{1}{l|}{}         & \multicolumn{1}{l|}{}      & \multicolumn{1}{l|}{}       &      \\ \hline
            //\multicolumn{1}{|l|}{Complete}    & \multicolumn{1}{l|}{}         & \multicolumn{1}{l|}{}      & \multicolumn{1}{l|}{}       &      \\ \hline
            //\multicolumn{1}{|l|}{Torus}       & \multicolumn{1}{l|}{}         & \multicolumn{1}{l|}{}      & \multicolumn{1}{l|}{}       &      \\ \hline
            //\multicolumn{1}{|l|}{Circle}      & \multicolumn{1}{l|}{a}        & \multicolumn{1}{l|}{b}     & \multicolumn{1}{l|}{c}      & d    \\ \hline
            //\multicolumn{1}{|l|}{Star}        & \multicolumn{1}{c|}{a}        & \multicolumn{1}{c|}{b}     & \multicolumn{1}{c|}{c}      & d    \\ \hline
            //\end{tabular}
            //\end{table}


        }

        //parser = new Parser(algorithms[algorithm] + "/" + problems[problem] + "/" + topology + "/" + migrationStrategy + "/" + "MiTest");
        //formatter = new DisplayNameFormatter(description, [" N: ", " K: ", " MI: "], ["n", "cK", "mi"]);
        //parser.LatexData(100, formatter, new DisplayNameFormatter(description.Replace("-", "_"), ["_MI_"], ["mi"]), true);
        public static void parseIterations(string folder, string algorithm, string problem, string topology, string migrationStrategy, string[] extraFormat, string[] extraDetail)
        {
            string description = algorithm + "-" + problem;
            Parser parser = new Parser(algorithm + "/" + problem + "/" + topology + "/" + migrationStrategy + "/" + "SpecificMiTest", folder + "/");
            var formats = extraFormat.ToList();
            formats.Add(" Mi: ");
            var details = extraDetail.ToList();
            details.Add("mi");
            DisplayNameFormatter formatter = new DisplayNameFormatter(description, formats.ToArray(), details.ToArray());
            parser.ParseDataFitnessEval(false, formatter);
            parser.LatexData(500, formatter, null, true);
        }

        public static void parseLambda(string folder, string algorithm, string problem, string[] extraFormat, string[] extraDetail)
        {
            string description = algorithm + "-" + problem;
            Parser parser = new Parser(algorithm + "/" + problem + "/" + "Lambda", folder + "/");
            var formats = extraFormat.ToList();
            formats.Add(" Lambda: ");
            formats.Add(" Mu: ");
            var details = extraDetail.ToList();
            details.Add("lambda");
            details.Add("mu");
            DisplayNameFormatter formatter = new DisplayNameFormatter(description, formats.ToArray(), details.ToArray());
            parser.parseConst(formatter, "lambda", ["mu"]);
            parser.LatexData(500, formatter, null, true);
        }
        public static void parseK(string folder, string algorithm, string problem, string[] extraFormat, string[] extraDetail)
        {
            string description = algorithm + "-" + problem;
            Parser parser = new Parser(algorithm + "/" + problem + "/" + "Lambda", folder + "/");
            var formats = extraFormat.ToList();
            formats.Add(" CGA_k: ");
            var details = extraDetail.ToList();
            details.Add("cK");
            DisplayNameFormatter formatter = new DisplayNameFormatter(description, formats.ToArray(), details.ToArray());
            parser.parseConst(formatter, "cK");
            parser.LatexData(500, formatter, null, true);
        }

        public static void parseNull(string folder, string algorithm, string problem)
        {
            string description = algorithm + "-" + problem;
            Parser parser = new Parser(algorithm + "/" + problem + "/" + "NullTest", folder + "/");
            DisplayNameFormatter formatter = new DisplayNameFormatter(description, [], []);
            parser.LatexData(3, formatter, null, true);
        }

        public static void parseMi(string folder, string algorithm, string problem, string topology, string migrationStrategy)
        {
            string description = algorithm + "-" + problem + "-" + topology + "-" + migrationStrategy;
            Parser parser = new Parser(algorithm + "/" + problem + "/" + topology + "/" + migrationStrategy + "/" + "MiTest", folder + "/");
            DisplayNameFormatter formatter = new DisplayNameFormatter(description, ["Mi: ", " Instace"], ["mi", "instance"]);
            parser.parseConst(formatter, "mi");
            parser.LatexData(500, formatter, null, true);
        }

        public static void parseN(string folder, string algorithm, string problem, string topology, string migrationStrategy, string testName = "NTest")
        {
            string description = algorithm + "-" + problem + "-" + topology + "-" + migrationStrategy;
            Parser parser = new Parser(algorithm + "/" + problem + "/" + topology + "/" + migrationStrategy + "/" + testName, folder + "/");
            DisplayNameFormatter formatter = new DisplayNameFormatter(description, ["N: "], ["n"]);
            parser.parseConst(formatter, "n", ["mu", "lambda"]);
            parser.LatexData(500, formatter, null, true);
        }
        public static void parseNSeriel(string folder, string algorithm, string problem, string testName = "NTest")
        {
            string description = algorithm + "-" + problem;
            Parser parser = new Parser(algorithm + "/" + problem + "/" + testName, folder + "/");
            DisplayNameFormatter formatter = new DisplayNameFormatter(description, ["N: "], ["n"]);
            parser.parseConst(formatter, "n", ["mu", "lambda"]);
            parser.LatexData(500, formatter, null, true);
        }

        public static void parseLessN(string folder, string algorithm, string problem, string topology, string migrationStrategy)
        {
            string description = algorithm + "-" + problem + "-" + topology + "-" + migrationStrategy;
            Parser parser = new Parser(algorithm + "/" + problem + "/" + topology + "/" + migrationStrategy + "/" + "LessNTest", folder + "/");
            DisplayNameFormatter formatter = new DisplayNameFormatter(description, ["N: "], ["n"]);
            parser.parseConst(formatter, "n", ["mu", "lambda"]);
            parser.LatexData(500, formatter, null, true);
        }

        public static void parseCluster(string folder, string algorithm, string problem, string topology, string migrationStrategy)
        {
            string description = algorithm + "-" + problem + "-" + topology + "-" + migrationStrategy;
            Parser parser = new Parser(algorithm + "/" + problem + "/" + topology + "/" + migrationStrategy + "/" + "cluster_test", folder + "/");
            DisplayNameFormatter formatter = new DisplayNameFormatter(description, ["ClusterSize: "], ["cluster_size"]);
            parser.parseConst(formatter, "cluster_size");
            parser.LatexData(500, formatter, null, true);
        }

        public static void parseJumpK(string folder, string algorithm, string problem, string topology, string migrationStrategy)
        {
            string description = algorithm + "-" + problem + "-" + topology + "-" + migrationStrategy;
            Parser parser = new Parser(algorithm + "/" + problem + "/" + topology + "/" + migrationStrategy + "/" + "jumpKTest", folder + "/");
            DisplayNameFormatter formatter = new DisplayNameFormatter(description, ["jumpK: "], ["jumpK"]);
            parser.parseConst(formatter, "jumpK", []);
            parser.LatexData(500, formatter, null, true);
        }

        public static void parseLambdaOverN(string folder, string algorithm, string problem)
        {
            string description = algorithm + "-" + problem;
            Parser parser = new Parser(algorithm + "/" + problem + "/" + "Lambda", folder + "/");
            DisplayNameFormatter formatter = new DisplayNameFormatter(description, [], []);
            parser.parseXoverYBestYs(formatter, ["n"], ["lambda"], [], ["mu"]);
            parser.LatexData(500, formatter, null, true);
        }
        public static void parseLatexData(string folder, string algorithm, string problem, string testName)
        {
            string description = algorithm + "-" + problem;
            Parser parser = new Parser(algorithm + "/" + problem + "/" + testName, folder + "/");
            DisplayNameFormatter formatter = new DisplayNameFormatter(description, [" mu: "," lambda: "], ["mu","lambda"]);
            parser.LatexData(500, formatter, null, true);
        }
        public static void parseSpecificMi(string folder, string algorithm, string problem, string topology, string migrationStrategy, string testName)
        {
            string description = algorithm + "-" + problem + "-" + topology + "-" + migrationStrategy;
            Parser parser = new Parser(algorithm + "/" + problem + "/" + topology + "/" + migrationStrategy + "/" + testName, folder + "/");
            DisplayNameFormatter formatter = new DisplayNameFormatter(description, [" mi: "], ["mi"]);
            parser.ParseTruncatedDataFitnessEval(false, formatter);
            parser.parseBoundHits(formatter, true);
            parser.parseBoundHits(formatter, false);
            parser.LatexData(500, formatter, null, true);
        }
        public static void parseSpecificMi(string folder, string algorithm, string problem, string testName)
        {
            string description = algorithm + "-" + problem;
            Parser parser = new Parser(algorithm + "/" + problem + "/" + testName, folder + "/");
            DisplayNameFormatter formatter = new DisplayNameFormatter(description, [" mi: "], ["mi"]);
            parser.ParseTruncatedDataFitnessEval(false, formatter);
            parser.parseBoundHits(formatter, true);
            parser.parseBoundHits(formatter, false);
            parser.LatexData(500, formatter, null, true);
        }

        //public static void parseLambdaOverN(string folder, string algorithm, string problem)
        //{
        //    string description = algorithm + "-" + problem;
        //    Parser parser = new Parser(algorithm + "/" + problem + "/" + "Lambda", folder + "/");
        //    DisplayNameFormatter formatter = new DisplayNameFormatter(description, [], []);
        //    parser.parseXoverYBestYs(formatter, ["n"], ["lambda"], [], ["mu"]);
        //    parser.LatexData(3, formatter, true);
        //}

        public static void ErikFileUnzipper(string folder)
        {
            string inDirectory = "C:\\Users\\erikv\\Downloads\\zips1";
            string outDirectory = "C:\\Users\\erikv\\OneDrive\\Skrivebord\\MASTERS SPECIALE\\MasterEDA\\"+ folder;
            Parallel.ForEach(Directory.GetFiles(inDirectory, "*.zip", SearchOption.AllDirectories), fileName => {
                using (ZipArchive archive = ZipFile.OpenRead(fileName))
                {
                    int amount = archive.Entries.Count();
                    int i = 0;
                    int j = 0;
                    foreach (ZipArchiveEntry entry in archive.Entries)
                    {
                        i++;
                        string entryPath = Path.Combine(outDirectory, entry.FullName);
                        if (entry.FullName.StartsWith("../"))
                        {
                            entryPath = Path.Combine(outDirectory, entry.FullName.Substring(3));
                        }
                        Directory.CreateDirectory(Path.GetDirectoryName(entryPath));
                        if (entry.Name == "")
                        {
                            continue;
                        }
                        entry.ExtractToFile(entryPath,overwrite:true);
                        if (i*100/amount >j)
                        {
                            j++;
                            Console.WriteLine(fileName + " is "+j+"% done");
                        }
                    }
                }
            });
        }
        public static void fileCleaner(string folder)
        {
            string directory = "C:\\Users\\erikv\\OneDrive\\Skrivebord\\MASTERS SPECIALE\\MasterEDA\\" + folder;
            Parallel.ForEach(Directory.GetFiles(directory, "*.txt", SearchOption.AllDirectories), fileName => {
                if (!fileName.Contains("paradigm"))
                {
                    //Console.Out.WriteLine(fileName);
                    File.Delete(fileName);
                }
            });
        }

        public static void dirCleaner(string folder, string searchTerm)
        {
            string directory = "C:\\Users\\erikv\\OneDrive\\Skrivebord\\MASTERS SPECIALE\\MasterEDA\\" + folder + "\\Results";
            Console.Out.WriteLine(directory);
            var dirs = Directory.GetDirectories(directory, searchTerm, SearchOption.AllDirectories);
            Console.Out.WriteLine("Found directories");
            Parallel.ForEach(dirs, dirName => {
                try
                {
                    Directory.Delete(dirName, true);
                    Console.Out.WriteLine("success with" + dirName);
                }
                catch (Exception ex)
                {
                    Console.Out.WriteLine("problems with" + dirName);
                    Console.Out.WriteLine(ex);
                }
            });
        }
        public static void dirCleanerNoHelp(string folder, string searchTerm)
        {
            string directory = "C:\\Users\\erikv\\OneDrive\\Skrivebord\\MASTERS SPECIALE\\MasterEDA\\" + folder;
            Console.Out.WriteLine(directory);
            var dirs = Directory.GetDirectories(directory, searchTerm, SearchOption.AllDirectories);
            Console.Out.WriteLine("Found directories");
            Parallel.ForEach(dirs, dirName => {
                try
                {
                    Directory.Delete(dirName, true);
                    Console.Out.WriteLine("success with" + dirName);
                }
                catch (Exception ex)
                {
                    Console.Out.WriteLine("problems with" + dirName);
                    Console.Out.WriteLine(ex);
                }
            });
        }
        public static void dirLister(string folder, string searchTerm)
        {
            string directory = "C:\\Users\\erikv\\OneDrive\\Skrivebord\\MASTERS SPECIALE\\MasterEDA\\" + folder + "\\Results";
            Console.Out.WriteLine(directory);
            var dirs = Directory.GetDirectories(directory, searchTerm, SearchOption.AllDirectories);
            Console.Out.WriteLine("Found directories");
            Parallel.ForEach(dirs, dirName => {
                Console.Out.WriteLine("Dir: " + dirName);
            });
        }
        public static void dirListerNoHelp(string folder, string searchTerm)
        {
            string directory = "C:\\Users\\erikv\\OneDrive\\Skrivebord\\MASTERS SPECIALE\\MasterEDA\\" + folder;
            Console.Out.WriteLine(directory);
            var dirs = Directory.GetDirectories(directory, searchTerm, SearchOption.AllDirectories);
            Console.Out.WriteLine("Found directories");
            Parallel.ForEach(dirs, dirName => {
                Console.Out.WriteLine("Dir: " + dirName);
            });
        }

        public static void jumpKMover(string folder, string algorithm, string problem)
        {
            string directory = "C:\\Users\\erikv\\OneDrive\\Skrivebord\\MASTERS SPECIALE\\MasterEDA\\" + folder + "\\Results\\"+ algorithm + "/" + problem + "/" + "Lambda";
            Console.Out.WriteLine(directory);
            var files = Directory.GetFiles(directory, "*.txt", SearchOption.AllDirectories);
            Console.Out.WriteLine("Found directories");
            Parallel.ForEach(files, fileName => {
                try
                {
                    int startIndex = fileName.IndexOf("_jumpK_") + "_jumpK_".Length;
                    int endIndex = fileName.IndexOf(".", startIndex);
                    string jumpK = fileName.Substring(startIndex, endIndex - startIndex);
                    string newPath = directory + "/jumpK_" + jumpK;
                    Directory.CreateDirectory(newPath);
                    string fileNameOnly = Path.GetFileName(fileName);
                    File.Move(fileName, newPath+"/"+fileNameOnly);
                    Console.Out.WriteLine("success with" + jumpK);
                }
                catch (Exception ex)
                {
                    Console.Out.WriteLine(ex);
                }
            });
        }

        public static void ParseMaxSat() {
            Parser parser;
            string description;
            DisplayNameFormatter formatter;

            int problem = 3;
            int algorithm = 0;
            string[] algorithms = ["UMDA", "CGA"];
            string[] problems = ["OneMax", "LeadingOnes", "TwoPeaks", "MaxSat", "Jumpk"];
            string[] topologies = ["Complete", "KCircle", "Star", "Taurus"];
            string[] migrationStrategies = ["NoMigration"];
            string[] lg_paradigm = ["lg", "sqrt_lg"];
            string dir;

            parser = new Parser(algorithms[algorithm] + "\\" + problems[problem] + "\\" + "Ck");
            description = algorithms[algorithm] + "-" + problems[problem] + "-" + "KTest";
            formatter = new DisplayNameFormatter(description, ["K: "], ["cK"]);
            //parser.parseConst(formatter, "cK");
            //parser.LatexData(200, formatter, true);
            parser.ParseMaxSatLatexData(formatter, ["1", "2", "3", "4", "5"], 10);

            foreach (string topology in topologies) {
                foreach (string migrationStrategy in migrationStrategies) {
                    //string directory = problems[problem] + "/" + topology + "/" + migrationStrategy + "/" + "MiTest";
                    //dir = Path.Combine("D:\\Workspaces\\MasterEDA2\\Results\\", directory);
                    //if (!Directory.Exists(dir)) { continue; }

                    //parser = new Parser(directory);

                    //description = algorithms[algorithm] + "-" + problems[problem] + "-" + topology + "-" + migrationStrategy;

                    //formatter = new DisplayNameFormatter(description, ["MI: "], ["mi"]);
                    //parser.ParseMaxSatLatexData(formatter, ["1", "2", "3", "4", "5"], 19, new DisplayNameFormatter(description.Replace("-", "_"), ["_MI_", "_Instance_"], ["mi", "instance"]));

                    //formatter = new DisplayNameFormatter(description, ["Instance: ", " MI: "], ["instance", "mi" ]);
                    //parser.parseMaxSatConst(formatter, "mi");
                }
            }
        }

        public static void ParseOneMaxLeadingOnes() {
            Parser parser;
            string description;
            DisplayNameFormatter formatter;

            int problem = 0;
            int algorithm = 1;
            string[] algorithms = ["UMDA", "CGA"];
            string[] problems = ["OneMax", "LeadingOnes", "TwoPeaks", "MaxSat", "Jumpk"];
            string[] topologies = ["Complete", "KCircle", "Star", "Taurus"];
            string[] migrationStrategies = ["Sample", "MuSample", "Prob", "NoMigration"];
            string[] lg_paradigm = ["lg", "sqrt_lg"];
            string dir;

            //parser = new Parser(algorithms[algorithm] + "\\" + problems[problem] + "\\" + "Ck");
            //description = algorithms[algorithm] + "-" + problems[problem] + "-" + "Ck";
            //formatter = new DisplayNameFormatter(description, ["K: "], ["cK"]);
            //parser.parseConst(formatter, "cK");
            //parser.LatexData(200, formatter, true);
            //parser.ParseMaxSatLatexData(formatter, ["1", "2", "3", "4", "5", "6",], 10);

            foreach (string paradigm in lg_paradigm) {

                description = algorithms[algorithm] + "-" + problems[problem] + "-Complete-NoMigration-" + paradigm;
                parser = new Parser(algorithms[algorithm] + "/" + paradigm + "/" + problems[problem] + "/" + "NullMigration");
                formatter = new DisplayNameFormatter(description, ["Mi: "], ["mi"]);
                parser.LatexData(10, formatter, new DisplayNameFormatter(description.Replace("-", "_")), true);

                //string dir = "D:/Workspaces/MasterEDA2/Results/" + algorithms[algorithm] + "/" + paradigm + "/" + problems[problem] + "/" + "NTestSeriel";
                //if (!Directory.Exists(dir)) continue;
                //description = algorithms[algorithm] + "-" + paradigm + "-" + problems[problem] + "NTestSeriel ";
                //parser = new Parser(algorithms[algorithm] + "/" + paradigm + "/" + problems[problem] + "/" + "NTestSeriel");
                //formatter = new DisplayNameFormatter(description, ["N: "], ["n"]);
                //parser.LatexData(int.MaxValue, formatter, true);
                //parser.parseNTest(formatter);

                foreach (string topology in topologies) {
                    foreach (string migrationStrategy in migrationStrategies) {
                        description = algorithms[algorithm] + "-" + problems[problem] + "-" + topology + "-" + migrationStrategy + "-" + paradigm;

                        dir = "D:/Workspaces/MasterEDA2/Results/" + algorithms[algorithm] + "/" + paradigm + "/" + problems[problem] + "/" + topology + "/" + migrationStrategy + "/" + "MiTest";
                        if (!Directory.Exists(dir)) continue;

                        parser = new Parser(algorithms[algorithm] + "/" + paradigm + "/" + problems[problem] + "/" + topology + "/" + migrationStrategy + "/" + "MiTest");
                        formatter = new DisplayNameFormatter(description, [" K: ", " Mi: "], ["cK", "mi"]);
                        //parser.parseProbDist(formatter);
                        parser.LatexData(10, formatter, new DisplayNameFormatter(description.Replace("-", "_"), ["_MI_"], ["mi"]), true);
                        //parser.parseConst(formatter, "cluster_size");
                        //parser.parseNTest(formatter);
                    }
                }
            }
        }


        public static void ParseTwoPeaks() {
            Parser parser;
            string description;
            DisplayNameFormatter formatter;

            int problem = 2;
            int algorithm = 1;
            string[] algorithms = ["UMDA", "CGA"];
            string[] problems = ["OneMax", "LeadingOnes", "TwoPeaks", "MaxSat", "Jumpk"];
            string[] topologies = [ "Taurus"];
            string[] migrationStrategies = [ "MuSample"];
            string dir;

            //description = algorithms[algorithm] + "-" + problems[problem] + "NullMigration ";
            //parser = new Parser(algorithms[algorithm] + "\\" + problems[problem] + "\\" + "NullTest");
            //formatter = new DisplayNameFormatter(description, ["Mi: "], ["mi"]);
            //parser.LatexData(10, formatter, new DisplayNameFormatter(description.Replace("-", "_")), true);
            //parser.parseProbDist(formatter, true);
            //parser.parseProbDist(formatter, false);
            //parser.ParseTruncatedDataFitnessEval(false, formatter);

            //dir = "D:/Workspaces/MasterEDA2/Results/" + algorithms[algorithm] + "/" + problems[problem] + "/" + "NTestSeriel";
            //if (Directory.Exists(dir)) {
            //    description = algorithms[algorithm] + "-" + problems[problem] + "NTestSeriel ";
            //    parser = new Parser(algorithms[algorithm] + "/" + problems[problem] + "/" + "NTestSeriel");
            //    formatter = new DisplayNameFormatter(description, ["N: "], ["n"]);
            //    parser.LatexData(int.MaxValue, formatter, true);
            //    parser.parseNTest(formatter);
            //}

            foreach (string topology in topologies) {
                foreach (string migrationStrategy in migrationStrategies) {
                    dir = "D:/Workspaces/MasterEDA2/ResultsNewHello/" + algorithms[algorithm] + "/" + problems[problem] + "/" + topology + "/" + migrationStrategy + "/";

                    description = algorithms[algorithm] + "-" + problems[problem] + "-" + topology + "-" + migrationStrategy;

                    //if (Directory.Exists(dir + "MiTest")) {
                    //    parser = new Parser(algorithms[algorithm] + "/" + problems[problem] + "/" + topology + "/" + migrationStrategy + "/" + "MiTest");
                    //    formatter = new DisplayNameFormatter(description, [" N: ", " K: ", " MI: "], ["n", "cK", "mi"]);
                    //    parser.LatexData(100, formatter, new DisplayNameFormatter(description.Replace("-", "_"), ["_MI_"], ["mi"]), true);
                    //parser.parseConst(formatter, "mi");

                    //parser.parseProbDist(formatter, true);
                    //parser.parseProbDist(formatter, false);
                    //parser.ParseTruncatedDataFitnessEval(false, formatter);
                    //}

                    if (Directory.Exists(dir + "NTest")) {
                        parser = new Parser(algorithms[algorithm] + "/" + problems[problem] + "/" + topology + "/" + migrationStrategy + "/" + "NTest");
                        formatter = new DisplayNameFormatter(description, [" N: ", " K: ", " MI: "], ["n", "cK", "mi"]);
                        parser.parseNTest(formatter);
                        parser.LatexData(int.MaxValue, formatter, null, true);

                        //parser.parseProbDist(formatter, true);
                        //parser.parseProbDist(formatter, false);
                        //parser.ParseTruncatedDataFitnessEval(false, formatter);
                    }

                    //if (Directory.Exists(dir + "NTest2")) {
                    //    parser = new Parser(algorithms[algorithm] + "/" + problems[problem] + "/" + topology + "/" + migrationStrategy + "/" + "NTest2");
                    //    formatter = new DisplayNameFormatter(description, [" N: ", " K: ", " MI: "], ["n", "cK", "mi"]);

                    //    parser.parseProbDist(formatter, true);
                    //    parser.parseProbDist(formatter, false);
                    //    parser.ParseTruncatedDataFitnessEval(false, formatter);
                    //}

                    if (Directory.Exists(dir + "ClusterSizeTest")) {
                        parser = new Parser(algorithms[algorithm] + "/" + problems[problem] + "/" + topology + "/" + migrationStrategy + "/" + "ClusterSizeTest");
                        formatter = new DisplayNameFormatter(description, ["Cluster Size: ", " N: ", " K: ", " MI: "], ["cluster_size", "n", "cK", "mi"]);
                        parser.LatexData(10, formatter, null, true);
                        parser.parseConst(formatter, "cluster_size");
                    }
                }
            }
        }

        public static void ParseLeadingOnes() {
            Parser parser;
            string description;
            DisplayNameFormatter formatter;

            int problem = 1;
            int algorithm = 1;
            string[] algorithms = ["UMDA", "CGA"];
            string[] problems = ["OneMax", "LeadingOnes", "TwoPeaks", "MaxSat", "Jumpk"];
            string[] topologies = ["Complete", "KCircle", "Star", "Star", "Taurus"];
            string[] migrationStrategies = ["Sample", "MuSample", "Prob", "NoMigration"];
            string[] lg_paradigm = ["lg", "sqrt_lg"];
            string dir;

            //parser = new Parser(algorithms[algorithm] + "\\" + problems[problem] + "\\" + "Ck");
            //description = algorithms[algorithm] + "-" + problems[problem] + "-" + "Ck";
            //formatter = new DisplayNameFormatter(description, ["K: "], ["cK"]);
            //parser.parseConst(formatter, "cK");
            //parser.LatexData(200, formatter, true);
            //parser.ParseMaxSatLatexData(formatter, ["1", "2", "3", "4", "5", "6",], 10);

            foreach (string paradigm in lg_paradigm) {
                dir = "D:/Workspaces/MasterEDA2/ResultsLeadingOnesInv/" + algorithms[algorithm] + "/" + paradigm + "/" + problems[problem] + "/" + "NullMigration";
                if (Directory.Exists(dir)) {
                    description = algorithms[algorithm] + "-" + paradigm + "-" + problems[problem] + "NullMigration ";
                    parser = new Parser(algorithms[algorithm] + "/" + paradigm + "/" + problems[problem] + "/" + "NullMigration");
                    formatter = new DisplayNameFormatter(description, ["Mi: "], ["mi"]);
                    parser.parseProbDist(formatter, true);
                    parser.parseProbDist(formatter, false);
                    parser.ParseTruncatedDataFitnessEval(false, formatter);
                }

                //string dir = "D:/Workspaces/MasterEDA2/Results/" + algorithms[algorithm] + "/" + paradigm + "/" + problems[problem] + "/" + "NTestSeriel";
                //if (!Directory.Exists(dir)) continue;
                //description = algorithms[algorithm] + "-" + paradigm + "-" + problems[problem] + "NTestSeriel ";
                //parser = new Parser(algorithms[algorithm] + "/" + paradigm + "/" + problems[problem] + "/" + "NTestSeriel");
                //formatter = new DisplayNameFormatter(description, ["N: "], ["n"]);
                //parser.LatexData(int.MaxValue, formatter, true);
                //parser.parseNTest(formatter);

                foreach (string topology in topologies) {
                    foreach (string migrationStrategy in migrationStrategies) {
                        description = algorithms[algorithm] + "-" + paradigm + "-" + problems[problem] + "-" + topology + "-" + migrationStrategy + " ";

                        dir = "D:/Workspaces/MasterEDA2/ResultsLeadingOnesInv/" + algorithms[algorithm] + "/" + paradigm + "/" + problems[problem] + "/" + topology + "/" + migrationStrategy + "/" + "MiTest";
                        if (!Directory.Exists(dir)) continue;

                        parser = new Parser(algorithms[algorithm] + "/" + paradigm + "/" + problems[problem] + "/" + topology + "/" + migrationStrategy + "/" + "MiTest");
                        formatter = new DisplayNameFormatter(description, ["Cluster Size: ", " N: ", " K: ", " Mi: "], ["cluster_size", "n", "cK", "mi"]);
                        parser.parseProbDist(formatter, true);
                        parser.parseProbDist(formatter, false);
                        parser.ParseTruncatedDataFitnessEval(false, formatter);
                        parser.LatexData(10, formatter, null, true);
                        parser.parseConst(formatter, "mi");
                        //parser.parseNTest(formatter);
                    }
                }
            }

        }

        public static void cga_jumpk_experiment1()
        {
            int runs = 100000;
            double dist1 = 0.5;
            double dist2 = 0.5;
            int plus = 0;
            int minus = 0;
            int highProbPlus = 0;
            int highProbMinus = 0;
            int lowProbPlus = 0;
            int lowProbMinus = 0;
            int n = 100;
            int jump = 5;
            var rand = new Random();
            int solved = 0;
            for (int j = 0; j < runs; j++)
            {
                int løs1Fitness = 0;

                int løs2Fitness = 0;

                int[] løsTotal = new int[n];
                int[] løsHigh = new int[n];
                int[] løsLow = new int[n];

                for (int i = 0; i < n - jump; i++)
                {
                    int løs1 = 0;
                    int løs2 = 0;
                    if (rand.NextDouble() <= dist1)
                    {
                        løs1 = 1;
                        løs1Fitness++;
                    }
                    if (rand.NextDouble() <= dist1)
                    {
                        løs2 = 1;
                        løs2Fitness++;
                    }
                    løsTotal[i] = løs1 - løs2;
                    løsHigh[i] = løs1 - løs2;
                }


                for (int i = n - jump; i < n; i++)
                {
                    int løs1 = 0;
                    int løs2 = 0;
                    if (rand.NextDouble() <= dist2)
                    {
                        løs1 = 1;
                        løs1Fitness++;
                    }
                    if (rand.NextDouble() <= dist2)
                    {
                        løs2 = 1;
                        løs2Fitness++;
                    }
                    løsTotal[i] = løs1 - løs2;
                    løsLow[i] = løs1 - løs2;
                }

                if (løs1Fitness == n || løs2Fitness == n)
                {
                    solved++;
                    continue;
                }

                if (løs1Fitness > n - jump)
                {
                    løs1Fitness = n - løs1Fitness;
                }
                if (løs2Fitness > n - jump)
                {
                    løs2Fitness = n - løs2Fitness;
                }
                if (løs1Fitness < løs2Fitness)
                {
                    løsTotal = løsTotal.Select(x => -x).ToArray();
                    løsHigh = løsHigh.Select(x => -x).ToArray();
                    løsLow = løsLow.Select(x => -x).ToArray();
                }
                plus += løsTotal.Where(x => x == 1).Sum();
                minus += løsTotal.Where(x => x == -1).Sum();
                highProbPlus += løsHigh.Where(x => x == 1).Sum();
                highProbMinus += løsHigh.Where(x => x == -1).Sum();
                lowProbPlus += løsLow.Where(x => x == 1).Sum();
                lowProbMinus += løsLow.Where(x => x == -1).Sum();
            }


            Console.WriteLine("\n\n\n\n\n\n Details");

            Console.WriteLine(solved);
            if (solved == 100)
            {
                return;
            }
            Console.WriteLine(plus + " , " + minus);
            Console.WriteLine(highProbPlus + " , " + highProbMinus);
            Console.WriteLine(highProbPlus / (n - jump) + " , " + highProbMinus / (n - jump));
            Console.WriteLine(lowProbPlus + " , " + lowProbMinus);
            Console.WriteLine(lowProbPlus / jump + " , " + lowProbMinus / jump);

            Console.WriteLine("\n\n\n\n\n\n Changes");
            Console.WriteLine(dist1 + "   ,   " + dist2);

            Console.WriteLine("HighPlusAvgPos   " + ((double)highProbPlus) / runs);
            Console.WriteLine("HighMinusAvgPos   " + ((double)highProbMinus) / runs);
            Console.WriteLine("HighPropAvgPos   " + ((double)highProbPlus) / ((double)highProbMinus));
            Console.WriteLine("LowPlusAvgPos   " + ((double)lowProbPlus) / runs);
            Console.WriteLine("LowMinusAvgPos   " + ((double)lowProbMinus) / runs);
            Console.WriteLine("LowPropAvgPos   " + ((double)lowProbPlus) / ((double)lowProbMinus));
        }

        public static void cga_jumpk_experiment2()
        {
            int runs = 1;
            int n = 100;
            int jump = 5;
            double[] prob = new double[n];
            prob = prob.Select(x => 0.5).ToArray();
            int printItr = 0;
            List<string> changeGraphs = new List<string>();
            List<string> probGraphs = new List<string>();
            List<string> highestProbPoints = new List<string>();
            List<string> lowestProbPoints = new List<string>();
            for (int k = 0; k<=5000; k++)
            {
                int plus = 0;
                int minus = 0;
                var rand = new Random();
                int solved = 0;
                double[] løsYdre = new double[n];
                int fitnessHøj = 0;
                int fitnessLav = 0;
                int bestFit = 0;
                for (int j = 0; j < runs; j++)
                {
                    int løs1Fitness = 0;

                    int løs2Fitness = 0;

                    int[] løsTotal = new int[n];

                    for (int i = 0; i < n - jump; i++)
                    {
                        int løs1 = 0;
                        int løs2 = 0;
                        if (rand.NextDouble() <= prob[i])
                        {
                            løs1 = 1;
                            løs1Fitness++;
                        }
                        if (rand.NextDouble() <= prob[i])
                        {
                            løs2 = 1;
                            løs2Fitness++;
                        }
                        løsTotal[i] = løs1 - løs2;
                    }


                    for (int i = n - jump; i < n; i++)
                    {
                        int løs1 = 0;
                        int løs2 = 0;
                        if (rand.NextDouble() <= prob[i])
                        {
                            løs1 = 1;
                            løs1Fitness++;
                        }
                        if (rand.NextDouble() <= prob[i])
                        {
                            løs2 = 1;
                            løs2Fitness++;
                        }
                        løsTotal[i] = løs1 - løs2;
                    }

                    if (løs1Fitness == n || løs2Fitness == n)
                    {
                        solved++;
                        continue;
                    }

                    if (løs1Fitness > n - jump)
                    {
                        løs1Fitness = n - løs1Fitness;
                    }
                    if (løs2Fitness > n - jump)
                    {
                        løs2Fitness = n - løs2Fitness;
                    }

                    bestFit = Math.Max(bestFit, Math.Max(løs1Fitness, løs2Fitness));
                    if (løs1Fitness < løs2Fitness)
                    {
                        løsTotal = løsTotal.Select(x => -x).ToArray();
                        fitnessHøj += løs2Fitness;
                        fitnessLav += løs1Fitness;
                    }
                    else
                    {
                        fitnessHøj += løs1Fitness;
                        fitnessLav += løs2Fitness;
                    }
                    plus += løsTotal.Where(x => x == 1).Sum();
                    minus += løsTotal.Where(x => x == -1).Sum();
                    double[] temp = prob.Select((x, i) => Math.Min(0.99,Math.Max(0.01,(x + løsTotal[i] / 100.0)))).OrderDescending().ToArray();

                    løsYdre = løsYdre.Select((x, i) => x + temp[i]).ToArray();


                }
                var temp1 = løsYdre.Select((x, i) => x / runs - prob[i]).OrderDescending().ToArray();
                prob = løsYdre.Select((x, i) => x  / runs).OrderDescending().ToArray();

                if (printItr > 0 && k% printItr != 0)
                {
                    continue;
                }
                printItr = Math.Max(1,Math.Min(1000, printItr * 2));

                //Console.WriteLine("\n\n\n\n\n\n Details");
                //Console.WriteLine("plus,minus = "+((double)plus) / runs + " , " + ((double)minus) / runs);
                //Console.WriteLine("bestfit,avgtopfit,avgbottomfit = " + bestFit + " , " +  ((double)fitnessHøj) / runs + " , " + ((double)fitnessLav) / runs);

                //Console.WriteLine(solved);
                //for (int i = 0; i < 10; i++)
                //{
                //    for (int j = 0; j<10; j++)
                //    {
                //        Console.Write(prob[i*10+j].ToString("0.000") + "|");
                //    }
                //    Console.WriteLine();
                //}

                Console.Write("changes" + k + " := plot([");
                for (int i = 0; i < n - 1; i++)
                {
                    Console.Write("[" + i + "," + temp1[i].ToString("0.000").Replace(",", ".") + "],");
                }
                Console.Write("[" + (n - 1) + "," + temp1[n - 1].ToString("0.000").Replace(",", ".") + "]");
                Console.Write("],legend = \"change, itr:"+k+"\"):");
                changeGraphs.Add("changes" + k);
                Console.WriteLine();

                Console.Write("probs"+k+" := plot([");
                for (int i = 0; i < n - 1; i++)
                {
                    Console.Write("[" + i + "," + prob[i].ToString("0.000").Replace(",", ".") + "],");
                }
                Console.Write("[" + (n-1) + "," + prob[n-1].ToString("0.000").Replace(",", ".") + "]");
                Console.Write("],legend = \"change, itr:" + k + "\"):");
                probGraphs.Add("probs" + k);
                Console.WriteLine();
                highestProbPoints.Add("[" + k + "," + prob[0].ToString("0.000").Replace(",", ".") + "]");
                lowestProbPoints.Add("[" + k + "," + prob[n-1].ToString("0.000").Replace(",", ".") + "]");
                //Console.WriteLine(plus + " , " + minus);
            }

            Console.WriteLine("display(" + System.String.Join(",", changeGraphs) + ");");
            Console.WriteLine("display(" + System.String.Join(",", probGraphs) + ");");
            Console.WriteLine("plot([" + System.String.Join(",", highestProbPoints) + "]);");
            Console.WriteLine("plot([" + System.String.Join(",", lowestProbPoints) + "]);");




        }

        public static void cga_jumpk_experiment3()
        {
            int runs = 1000;
            int n = 100;
            int jump = 50;
            double[][] probs = new double[runs][];
            for (int i = 0; i < runs; i++)
            {
                probs[i] = new double[n];
                for (int j = 0; j < n; j++)
                {
                    probs[i][j] = 0.5;
                }
            }

            double[] prevprobs = new double[n];
            for (int i = 0; i < n; i++)
            {
                prevprobs[i] = 0.5;
            }

            int printItr = 0;
            List<string> changeGraphs = new List<string>();
            List<string> probGraphs = new List<string>();
            List<string> highestProbPoints = new List<string>();
            List<string> lowestProbPoints = new List<string>();
            List<string> probEnd = new List<string>();
            for (int itr = 0; itr <= 50000; itr++)
            {
                int plus = 0;
                int minus = 0;
                var rand = new Random();
                int solved = 0;
                double[] løsYdre = new double[n];
                int fitnessHøj = 0;
                int fitnessLav = 0;
                int bestFit = 0;
                for (int j = 0; j < runs; j++)
                {
                    int løs1Fitness = 0;

                    int løs2Fitness = 0;

                    int[] løsTotal = new int[n];

                    for (int i = 0; i < n - jump; i++)
                    {
                        int løs1 = 0;
                        int løs2 = 0;
                        if (rand.NextDouble() <= probs[j][i])
                        {
                            løs1 = 1;
                            løs1Fitness++;
                        }
                        if (rand.NextDouble() <= probs[j][i])
                        {
                            løs2 = 1;
                            løs2Fitness++;
                        }
                        løsTotal[i] = løs1 - løs2;
                    }


                    for (int i = n - jump; i < n; i++)
                    {
                        int løs1 = 0;
                        int løs2 = 0;
                        if (rand.NextDouble() <= probs[j][i])
                        {
                            løs1 = 1;
                            løs1Fitness++;
                        }
                        if (rand.NextDouble() <= probs[j][i])
                        {
                            løs2 = 1;
                            løs2Fitness++;
                        }
                        løsTotal[i] = løs1 - løs2;
                    }

                    if (løs1Fitness == n || løs2Fitness == n)
                    {
                        solved++;
                        continue;
                    }

                    if (løs1Fitness > n - jump)
                    {
                        løs1Fitness = n - løs1Fitness;
                    }
                    if (løs2Fitness > n - jump)
                    {
                        løs2Fitness = n - løs2Fitness;
                    }

                    bestFit = Math.Max(bestFit, Math.Max(løs1Fitness, løs2Fitness));
                    if (løs1Fitness < løs2Fitness)
                    {
                        løsTotal = løsTotal.Select(x => -x).ToArray();
                        fitnessHøj += løs2Fitness;
                        fitnessLav += løs1Fitness;
                    }
                    else
                    {
                        fitnessHøj += løs1Fitness;
                        fitnessLav += løs2Fitness;
                    }
                    plus += løsTotal.Where(x => x == 1).Sum();
                    minus += løsTotal.Where(x => x == -1).Sum();
                    for(int i = 0; i<n; i++)
                    {
                        probs[j][i]= Math.Min(0.99, Math.Max(0.01, (probs[j][i] + løsTotal[i] / 100.0)));
                    }
                }

                
                if (printItr > 0 && itr % printItr != 0)
                {
                    continue;
                }
                printItr = Math.Max(1, Math.Min(5000, printItr * 2));

                double[][] holder = probs.Select(x => x.OrderDescending().ToArray()).ToArray();
                var probAvg = Enumerable.Range(0, n).Select(i => holder.Select(x => x[i]).Average()).ToArray();
                var changesAvg = probAvg.Select((x, i) => x - prevprobs[i]).ToArray();
                prevprobs = probAvg;
                //Console.WriteLine("\n\n\n\n\n\n Details");
                //Console.WriteLine("plus,minus = "+((double)plus) / runs + " , " + ((double)minus) / runs);
                //Console.WriteLine("bestfit,avgtopfit,avgbottomfit = " + bestFit + " , " +  ((double)fitnessHøj) / runs + " , " + ((double)fitnessLav) / runs);

                //Console.WriteLine(solved);
                //for (int i = 0; i < 10; i++)
                //{
                //    for (int j = 0; j<10; j++)
                //    {
                //        Console.Write(prob[i*10+j].ToString("0.000") + "|");
                //    }
                //    Console.WriteLine();
                //}
                Console.Write("changes" + itr + " := plot([");
                for (int i = 0; i < n - 1; i++)
                {
                    Console.Write("[" + i + "," + changesAvg[i].ToString("0.000").Replace(",", ".") + "],");
                }
                Console.Write("[" + (n - 1) + "," + changesAvg[n - 1].ToString("0.000").Replace(",", ".") + "]");
                Console.Write("],legend = \"change, itr:" + itr + "\"):");
                changeGraphs.Add("changes" + itr);
                Console.WriteLine();

                Console.Write("probs" + itr + " := plot([");
                for (int i = 0; i < n - 1; i++)
                {
                    Console.Write("[" + i + "," + probAvg[i].ToString("0.000").Replace(",", ".") + "],");
                }
                Console.Write("[" + (n - 1) + "," + probAvg[n - 1].ToString("0.000").Replace(",", ".") + "]");
                Console.Write("],legend = \"prob, itr:" + itr + "\"):");
                probGraphs.Add("probs" + itr);
                Console.WriteLine();
                highestProbPoints.Add("[" + itr + "," + probAvg[0].ToString("0.000").Replace(",", ".") + "]");
                lowestProbPoints.Add("[" + itr + "," + probAvg[n - 1].ToString("0.000").Replace(",", ".") + "]");
                //Console.WriteLine(plus + " , " + minus);

                if (itr == 50000)
                {
                    foreach (var item in probAvg)
                    {
                        probEnd.Add(item.ToString("0.000").Replace(",", "."));
                    }
                }

            }

            Console.WriteLine("display(" + System.String.Join(",", changeGraphs) + ");");
            Console.WriteLine("display(" + System.String.Join(",", probGraphs) + ");");
            Console.WriteLine("plot([" + System.String.Join(",", highestProbPoints) + "]);");
            Console.WriteLine("plot([" + System.String.Join(",", lowestProbPoints) + "]);");
            Console.WriteLine("" + System.String.Join("*", probEnd) + ";");
            Console.WriteLine("" + System.String.Join("+", probEnd) + ";");




        }
    }
}
