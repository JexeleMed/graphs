#include "Parameters.h"
#include <iostream>
#include <vector>

#define LOG(x) "[" #x "=" << (x) << "]"
#define LOG_ENUM(x) "[" #x "=" << static_cast<int>((x)) << "]"

namespace Parameters
{

//
// Setting default values for parameters.
//

RunModes runMode = RunModes::undefined;
Problems problem = Problems::undefined;
Algorithms algorithm = Algorithms::undefined;
Structures structure = Structures::undefined;

std::string inputFile = {};
std::string outputFile = {};
std::string resultsFile = {};

int vertexStart = -1;
int vertexEnd = -1;

int vertexCount = -1;
int density = -1;
int iterations = -1;

//
// Additional internal variables.
//

bool debug = false;
bool errorFlag = false;

bool check(std::string parameter, std::string key1,
               std::string key2 = {}, std::string key3 = {},
                   std::string key4 = {}, std::string key5 = {})
{
  std::vector<std::string> keys {key1, key2, key3, key4, key5};

  for (size_t i = 0; i < keys.size(); i++)
  {
    if (parameter == keys[i])
    {
      return true;
    }
  }

  return false;
}

template <typename T>
int updateEnumParameter(T &parameter, T count, std::string valueStr)
{
  int newValue = std::stoi(valueStr);

  if ((newValue < 0) ||
          (newValue >= static_cast<int>(count)))
  {
    std::cerr << "ERROR! Value is out of enum bounds!\n";

    errorFlag = true;
    return -1;
  }

  parameter = static_cast<T>(newValue);

  return 0;
}

int readParameters(int argc, char **argv)
{
  if (argc <= 0)
  {
    std::cerr << "ERROR! No parameters or wrong number!\n";
    return -1;
  }

  for (int i = 0; i < argc; i++)
  {
    std::string arg = std::string(argv[i]);

    if (debug)
    {
      std::cout << "DEBUG: Analysing parameter on index " << LOG(i)
                << " " << LOG(arg) << ".\n";
    }

    //
    // Run modes.
    //

    if (check(arg, "--singleFile", "-f"))
    {
      runMode = RunModes::singleFile;
      continue;
    }
    else if (check(arg, "--benchmark", "-b"))
    {
      runMode = RunModes::benchmark;
      continue;
    }
    else if (check(arg, "--help", "-h"))
    {
      runMode = RunModes::help;
      continue;
    }

    //
    // Parmeters that require passed value.
    //
    // Check if you can safely use the next value.
    //

    if (i + 1 >= argc)
    {
      std::cerr << "ERROR! No value has been passed for "
                << LOG(arg) << " or parameter name is wrong!\n";
      return -1;
    }

    std::string value = std::string(argv[++i]);

    if (debug)
    {
      std::cout << "DEBUG: Possible new value for " << LOG(arg)
                << " will be " << LOG(value) << ".\n";
    }

    if (check(arg, "--problem", "-p"))
    {
      updateEnumParameter(problem, Problems::count, value);
      continue;
    }

    if (check(arg, "--algorithm", "-a"))
    {
      updateEnumParameter(algorithm, Algorithms::count, value);
      continue;
    }

    if (check(arg, "--structure", "-s"))
    {
      updateEnumParameter(structure, Structures::count, value);
      continue;
    }

    if (check(arg, "--inputFile", "-i"))
    {
      inputFile = value;
      continue;
    }

    if (check(arg, "--outputFile", "-o"))
    {
      outputFile = value;
      continue;
    }

    if (check(arg, "--vertexStart", "-c"))
    {
      vertexStart = std::stoi(value);
      continue;
    }

    if (check(arg, "--vertexEnd", "-e"))
    {
      vertexEnd = std::stoi(value);
      continue;
    }

    if (check(arg, "--resultsFile", "-r"))
    {
      resultsFile = value;
      continue;
    }

    if (check(arg, "--vertexCount", "-l"))
    {
      vertexCount = std::stoi(value);
      continue;
    }

    if (check(arg, "--density", "-d"))
    {
      density = std::stoi(value);
      continue;
    }

    if (check(arg, "--iterations", "-n"))
    {
      iterations = std::stoi(value);
      continue;
    }

    //
    // Unknown parameter.
    //

    std::cerr << "ERROR! Parameter " << LOG(arg) << " was not recognized!\n";
    return -1;
  }

  if (errorFlag)
  {
    std::cerr << "ERROR! Something went wrong with reading parameters!\n";
    return -1;
  }

  return 0;
}

void enableDebug()
{
  std::cout << "DEBUG: Parameters debugging enabled.\n";
  debug = true;
}

void disableDebug()
{
  std::cout << "DEBUG: Parameters debugging disabled.\n";
  debug = false;
}

void help()
{
  std::cout << "Options:\n";
  std::cout << "\n";
  std::cout << "Run modes (mutually exclusive):\n";
  std::cout << "  -f, --singleFile   Test a single file\n";
  std::cout << "  -b, --benchmark    Benchmark test (random values every iteration)\n";
  std::cout << "  -h, --help         Show help message\n";
  std::cout << "\n";
  std::cout << "Common options: \n";
  std::cout << "  -p, --problem VAL     0 - Minimum spanning tree\n";
  std::cout << "                        1 - Shortest path\n";
  std::cout << "                        2 - Max flow\n";
  std::cout << "  -a, --algorithm VAL   0 - Run all algorithms available for given problem\n";
  std::cout << "                        1 - Prim's algorithm\n";
  std::cout << "                        2 - Kruskal's algorithm\n";
  std::cout << "                        3 - Dijkstra's algorithm\n";
  std::cout << "                        4 - Bellman–Ford algorithm\n";
  std::cout << "                        5 - Ford–Fulkerson algorithm\n";
  std::cout << "  -s, --structure VAL   0 - Run for all structures\n";
  std::cout << "                        1 - Incidence matrix\n";
  std::cout << "                        2 - Adjacency list\n";
  std::cout << "\n";
  std::cout << "Single file options:\n";
  std::cout << "  -i, --inputFile FILE     File with graph\n";
  std::cout << "  -o, --outputFile FILE    Save here graph structure and solution/solutions for given parameters (there is no defined way to do it, as long as you print everything it will be fine)\n";
  std::cout << "  -c, --vertexStart VAL    Current/starting vertex (SP/MF problems)\n";
  std::cout << "  -e, --vertexEnd VAL      End vertex (SP/MF problems)\n";
  std::cout << "\n";
  std::cout << "Benchmark options:\n";
  std::cout << "  -r, --resultsFile FILE   Results (time and parameters) will be saved to this file\n";
  std::cout << "  -l, --vertexCount VAL    How many vertices in the generated graph\n";
  std::cout << "  -d, --density VAL        Density of generated graph as ineger (e.g. 25%)\n";
  std::cout << "  -n, --iterations ITE :)  How many repetitions of the research with the given parameters\n";
  std::cout << "\n";
  std::cout << "Examples:\n";
  std::cout << "  ./project --singleFile --inputFile \"in.txt\" --outputFile \"out.txt\" -p 1 -a 2 -s 1 -c 2 -e 5\n";
  std::cout << "  Load graph from file \"in.txt\" and load it into the incidence matrix. Using Kruskal's algorithm solve the shortest path problem from vertex 2 to vertex 5. Save solution to the \"out.txt\".\n";
  std::cout << "  ./project --benchmark -p 0 -a 0 -s 0 -l 35 -d 75 -n 50 -r \"res.txt\"\n";
  std::cout << "  Repeat following test 50 times: Generate a graph with 35 vertices and 75% density. Solve MST problem for that graph using every implemented algorithm, and every implemented structure seperately. Results save in \"res.txt\"\n";
  std::cout << "\n";
}

void printParameters()
{
  std::cout << "Parameters values:\n";
  std::cout << LOG_ENUM(runMode) << "\n";
  std::cout << "Common options:\n"
            << LOG_ENUM(problem) << ", "
            << LOG_ENUM(algorithm) << ", "
            << LOG_ENUM(structure) << "\n";
  std::cout << "Single file options:\n";
  std::cout << LOG(inputFile) << ", "
            << LOG(outputFile) << ", "
            << LOG(vertexStart) << ", "
            << LOG(vertexEnd) << "\n";
  std::cout << "Benchmark options:\n";
  std::cout << LOG(resultsFile) << ", "
            << LOG(vertexCount) << ", "
            << LOG(density) << ", "
            << LOG(iterations) << "\n";
}

const char *getVersion()
{
  return LIB_VERSION;
}

}
