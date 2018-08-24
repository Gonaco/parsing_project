#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include "qasm_semantic.hpp"

int writeOpenQLCode(compiler::QasmRepresentation& qasm_representation, const std::string& openql_file_name);
int replace(std::string& str, const std::string& from, const std::string& to);
void writeHeaders(std::ofstream& openql_file);
void writeCircuit(std::ofstream& openql_file, compiler::QasmRepresentation& qasm_representation);
void writeSubCircuit(std::ofstream& openql_file, compiler::SubCircuit& subs);
void writeSerialParallelOperations(std::ofstream& openql_file, compiler::OperationsCluster& ops_cluster);
void writeOperations(std::ofstream& openql_file, compiler::Operation& op);
void writeTail(std::ofstream& openql_file);

int main (int argc, const char** argv)
{
  
    #if YYDEBUG == 1
    extern int yydebug;
    yydebug = 1;
    #endif

    if (argc > 1)
        std::cout << "Path to QASM file = " << argv[1] << std::endl;

    // open a file handle to a particular file:
    FILE *myfile = fopen(argv[1], "r");
    // make sure it's valid:
    if (!myfile) {
        std::cout << "File " << argv[1] << " not found!" << std::endl;
        return -1;
    }
    
    compiler::QasmSemanticChecker sm(myfile);

    auto qasm_representation = sm.getQasmRepresentation();
    
    int result = sm.parseResult();

    if (!result)
    {
        std::cout << "Input is valid.\n" << std::endl;
	std::string cqasm_file_name = argv[1];
	replace(cqasm_file_name, ".qasm", ".py");
        writeOpenQLCode(qasm_representation, cqasm_file_name);
    }
    else
    {
        std::cout << "Input is invalid!" << std::endl;
    }

    return result;

}

int writeOpenQLCode(compiler::QasmRepresentation& qasm_representation, const std::string& openql_file_name)
{
  std::ofstream openql_file;
  openql_file.open(openql_file_name);
  // make sure it's valid:
  if (!openql_file.is_open()) {
    std::cout << "File " << openql_file_name << " not found!" << std::endl;
    return -1;
  }

  writeHeaders(openql_file);

  qasm_representation.printMappings();

  // ...
  writeCircuit(openql_file, qasm_representation);

  writeTail(openql_file);
  
  openql_file.close();
  return 0;
}

int replace(std::string& str, const std::string& from, const std::string& to)
{
  size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return -1;
    str.replace(start_pos, from.length(), to);
    return 0;
}

void writeHeaders(std::ofstream& openql_file)
{
  openql_file << "from openql import openql as ql\n";
  openql_file << "import os\n";
  openql_file << "import argparse\n\n";
  openql_file << "def circuit(config_file, scheduler='ASAP', output_dir_name='test_output', optimize='no', log_level='LOG_WARNING'):\n";  
  openql_file << "    curdir = os.path.dirname(__file__)\n";
  openql_file << "    output_dir = os.path.join(curdir, output_dir_name)\n";
  openql_file << "    ql.set_option('output_dir', output_dir)\n";
  openql_file << "    ql.set_option('optimize', optimize)\n";
  openql_file << "    ql.set_option('scheduler', scheduler)\n";
  openql_file << "    ql.set_option('log_level', log_level)\n";
  openql_file << "    config_fn = os.path.join(curdir, config_file)\n";
  openql_file << "    platform  = ql.Platform('platform_none', config_fn)\n";
  openql_file << "    sweep_points = [1,2]\n";
  
}

void writeCircuit(std::ofstream& openql_file, compiler::QasmRepresentation& qasm_representation)
{

  openql_file << "    num_circuits = " << qasm_representation.getSubCircuits().numberOfSubCircuits()  << "\n";
  openql_file << "    num_qubits = " << qasm_representation.numQubits() << "\n";

  const std::vector<compiler::SubCircuit>& found_subcircuits = qasm_representation.getSubCircuits().getAllSubCircuits();
  for (compiler::SubCircuit subcircuit : found_subcircuits){
    writeSubCircuit(openql_file, subcircuit);
  }
}

void writeSubCircuit(std::ofstream& openql_file, compiler::SubCircuit& subs)
{
  openql_file << "    p = ql.Program('" << subs.nameSubCircuit() << "', num_qubits, platform)\n";
  openql_file << "    p.set_sweep_points(sweep_points, num_circuits)\n";
  openql_file << "    k = ql.Kernel('" << subs.nameSubCircuit() << "', platform)\n";
		 
  for (auto elem : subs.getOperationsCluster())
    writeSerialParallelOperations(openql_file, *elem);
  
}
void writeSerialParallelOperations(std::ofstream& openql_file, compiler::OperationsCluster& ops_cluster)
{
  if (ops_cluster.isParallel())
    {
      for (auto elem : ops_cluster.getOperations())
	writeOperations(openql_file, *elem);
    }
    else
      {
	for (auto elem : ops_cluster.getOperations())
	writeOperations(openql_file, *elem);
      }
}

void writeOperations(std::ofstream& openql_file, compiler::Operation& op)
// Potentially a function example where one can create an interface with whatever simulator/eQASM generator class.
{
    std::string type_ = op.getType();
    openql_file << "k.gate('" << type_ << "', [";
    if ( type_ == "rx" || type_ == "ry" || type_ == "rz" )
    {
      openql_file << op.getQubitsInvolved().getSelectedQubits().getIndices();
      std::cout << "Rotations = " << op.getRotationAngle() << std::endl;
    }
    else if (type_ == "measure_parity")
    {
        std::cout << std::endl;
        auto measureParityProperties = op.getMeasureParityQubitsAndAxis();
        measureParityProperties.first.first.printMembers();
	// openql_file << measureParityProperties.first.first.getSelectedQubits().getIndices();??
        std::cout << "With axis " << measureParityProperties.second.first << std::endl;
        measureParityProperties.first.second.printMembers();
        std::cout << "With axis " << measureParityProperties.second.second << std::endl;
    }
    else if (type_ == "cnot" || type_ == "cz" || type_ == "swap")
    {
      openql_file << op.getQubitsInvolved(1).getSelectedQubits().getIndices();
      openql_file << ",";
      openql_file << op.getTwoQubitPairs().second.getSelectedQubits().getIndices();
    }
    else if (type_ == "cr" || type_ == "crk")
    {
        std::cout << std::endl;
        std::cout << "Qubit Pair 1: ";
        op.getTwoQubitPairs().first.printMembers();
        std::cout << "Qubit Pair 2: "; 
        op.getTwoQubitPairs().second.printMembers();
        std::cout << "Rotation = " << op.getRotationAngle() << std::endl; 
    }
    else if (type_ == "toffoli")
    {
      openql_file << op.getToffoliQubitPairs().first.getSelectedQubits().getIndices();
      openql_file << ",";
      openql_file << op.getToffoliQubitPairs().second.first.getSelectedQubits().getIndices();
      openql_file << ",";
      openql_file << op.getToffoliQubitPairs().second.second.getSelectedQubits().getIndices();
    }
    else if (type_ == "wait")
    {
        std::cout << std::endl;
        std::cout << "Wait time (integer) = " << op.getWaitTime() << std::endl;
    }
    else if (type_ == "display")
    {
      op.getDisplayBits().printMembers();
    }
    else op.getQubitsInvolved().getSelectedQubits().getIndices();

    if (op.isBitControlled()){
        std::cout << "Bit controlled with bits: ";
        op.getControlBits().printMembers();
    }
    openql_file << "])";
    
}

void writeTail(std::ofstream& openql_file)
{
  openql_file << "\n    p.add_kernel(k)\n";
  openql_file << "    p.compile()\n\n";
  openql_file << "if __name__ == '__main__':\n";
  openql_file << "    parser = argparse.ArgumentParser(description='OpenQL compilation of a Quantum Algorithm')\n";
  openql_file << "    parser.add_argument('config_file', help='Path to the OpenQL configuration file to compile this algorithm')\n";
  openql_file << "    parser.add_argument('--scheduler', help='Scheduler specification (ASAP (default), ALAP, ...)')\n";
  openql_file << "    parser.add_argument('--out_dir', help='Folder name to store the compilation')\n";
  openql_file << "    args = parser.parse_args()\n";
  openql_file << "    try:\n";
  openql_file << "        if args.out_dir and args.scheduler:\n";
  openql_file << "            circuit(args.config_file, args.scheduler, args.out_dir)\n";
  openql_file << "        elif args.scheduler:\n";
  openql_file << "            circuit(args.config_file, args.scheduler)\n";
  openql_file << "        elif args.out_dir:\n";
  openql_file << "            circuit(args.config_file, out_dir_name=args.out_dir)\n";
  openql_file << "        else:\n";
  openql_file << "            circuit(args.config_file)\n";
  openql_file << "    except TypeError:\n";
  openql_file << "        print('\\nCompiled, but some gate is not defined in the configuration file. \\nThe gate will be invoked like it is.')\n";
  openql_file << "        raise\n";
}

// int writeOperation()?
