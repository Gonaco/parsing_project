#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include "qasm_semantic.hpp"

const int cycle_t = 20;
int t = 0;
int num_qubits = 0;

int writeQuantumsimCode(compiler::QasmRepresentation& qasm_representation, const std::string& quantumsim_file_name);
int replace(std::string& str, const std::string& from, const std::string& to);
void writeHeaders(std::ofstream& quantumsim_file);
void writeCircuit(std::ofstream& quantumsim_file, compiler::QasmRepresentation& qasm_representation);
void writeSubCircuit(std::ofstream& quantumsim_file, compiler::SubCircuit& subs);
void writeSerialParallelOperations(std::ofstream& quantumsim_file, compiler::OperationsCluster& ops_cluster);
int isValidOperation(compiler::Operation& op);
void writeCQOperations(std::ofstream& quantumsim_file, compiler::Operation& op);
void writeOperations(std::ofstream& quantumsim_file, compiler::Operation& op);
void writeTail(std::ofstream& quantumsim_file);

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
        writeQuantumsimCode(qasm_representation, cqasm_file_name);
    }
    else
    {
        std::cout << "Input is invalid!" << std::endl;
    }

    return result;

}

int writeQuantumsimCode(compiler::QasmRepresentation& qasm_representation, const std::string& quantumsim_file_name)
{
  std::ofstream quantumsim_file;
  quantumsim_file.open(quantumsim_file_name);
  // make sure it's valid:
  if (!quantumsim_file.is_open()) {
    std::cout << "File " << quantumsim_file_name << " not found!" << std::endl;
    return -1;
  }

  writeHeaders(quantumsim_file);

  qasm_representation.printMappings();

  writeCircuit(quantumsim_file, qasm_representation);

  writeTail(quantumsim_file);
  
  quantumsim_file.close();
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

void writeHeaders(std::ofstream& quantumsim_file)
{
  quantumsim_file << "import numpy as np\n";
  quantumsim_file << "from quantumsim.circuit import Circuit\n";
  quantumsim_file << "from quantumsim.circuit import uniform_noisy_sampler\n";
  quantumsim_file << "from quantumsim.circuit import IdlingGate as i\n";
  quantumsim_file << "from quantumsim.circuit import RotateY as ry\n";
  quantumsim_file << "from quantumsim.circuit import RotateX as rx\n";
  quantumsim_file << "from quantumsim.circuit import RotateZ as rz\n";
  quantumsim_file << "from quantumsim.circuit import Hadamard as h\n";
  quantumsim_file << "from quantumsim.circuit import CPhase as cz\n";
  quantumsim_file << "from quantumsim.circuit import CNOT as cnot\n";
  quantumsim_file << "from quantumsim.circuit import Swap as swap\n";
  quantumsim_file << "from quantumsim.circuit import CPhaseRotation as cr\n";
  quantumsim_file << "from quantumsim.circuit import ConditionalGate as ConditionalGate\n";
  quantumsim_file << "from quantumsim.circuit import RotateEuler as RotateEuler\n";
  quantumsim_file << "from quantumsim.circuit import ResetGate as ResetGate\n";
  quantumsim_file << "from quantumsim.circuit import Measurement as measure\n";
  quantumsim_file << "import quantumsim.sparsedm as sparsedm\n";
  quantumsim_file << "\n";
  quantumsim_file << "# print('GPU is used:', sparsedm.using_gpu)\n";
  quantumsim_file << "\n";
  quantumsim_file << "\n";
  quantumsim_file << "def t(q, time):\n";
  quantumsim_file << "    return RotateEuler(q, time=time, theta=0, phi=np.pi/4, lamda=0)\n";
  quantumsim_file << "\n";
  quantumsim_file << "def tdag(q, time):\n";
  quantumsim_file << "    return RotateEuler(q, time=time, theta=0, phi=-np.pi/4, lamda=0)\n";
  quantumsim_file << "\n";
  quantumsim_file << "def measure_z(q, time, sampler):\n";
  quantumsim_file << "    return measure(q, time, sampler)\n";
  quantumsim_file << "\n";
  quantumsim_file << "def z(q, time):\n";
  quantumsim_file << "    return rz(q, time, angle=np.pi)\n";
  quantumsim_file << "\n";
  quantumsim_file << "def x(q, time):\n";
  quantumsim_file << "    return rx(q, time, angle=np.pi)\n";
  quantumsim_file << "\n";
  quantumsim_file << "def y(q, time):\n";
  quantumsim_file << "    return ry(q, time, angle=np.pi)\n";
  quantumsim_file << "\n";
  quantumsim_file << "def rx90(q, time):\n";
  quantumsim_file << "    return rx(q, time, angle=np.pi/2)\n";
  quantumsim_file << "\n";
  quantumsim_file << "def ry90(q, time):\n";
  quantumsim_file << "    return ry(q, time, angle=np.pi/2)\n";
  quantumsim_file << "\n";
  quantumsim_file << "def xm90(q, time):\n";
  quantumsim_file << "    return rx(q, time, angle=-np.pi/2)\n";
  quantumsim_file << "\n";
  quantumsim_file << "def ym90(q, time):\n";
  quantumsim_file << "    return ry(q, time, angle=-np.pi/2)\n";
  quantumsim_file << "\n";
  quantumsim_file << "def rx45(q, time):\n";
  quantumsim_file << "    return rx(q, time, angle=np.pi/4)\n";
  quantumsim_file << "\n";
  quantumsim_file << "def mx45(q, time):\n";
  quantumsim_file << "    return rx(q, time, angle=-np.pi/4)\n";
  quantumsim_file << "\n";
  quantumsim_file << "def prep_z(q, time):\n";
  quantumsim_file << "    return ResetGate(q, time, state=0)\n\n";
  // quantumsim_file << "def circuit_function(error=0.01, meas_error=0.03, init_state=''):\n\n";
    quantumsim_file << "def circuit_function(q_t1=3000, q_t2=1500, error=0.01, meas_error=0.03, init_state=''):\n\n";
  
}

void writeCircuit(std::ofstream& quantumsim_file, compiler::QasmRepresentation& qasm_representation)
{

  const std::vector<compiler::SubCircuit>& found_subcircuits = qasm_representation.getSubCircuits().getAllSubCircuits();
  num_qubits = qasm_representation.numQubits();

  quantumsim_file << "    c = Circuit(title='" << found_subcircuits[0].nameSubCircuit() << "')\n\n";

  for (int q = 0; q < num_qubits; q++){
      quantumsim_file << "    c.add_gate(ResetGate('q" << q << "', time=1, state=int(init_state[" << q << "])))\n";
    }

  for (int q = 0; q < num_qubits; q++){
      quantumsim_file << "    c.add_qubit('q" << q << "', q_t1, q_t2)\n";
    }
  
  for (compiler::SubCircuit subcircuit : found_subcircuits){
    
    quantumsim_file << "    subc = Circuit(title='" << subcircuit.nameSubCircuit() << "')\n";

    for (int q = 0; q < num_qubits; q++){
      quantumsim_file << "    subc.add_qubit('q" << q << "', q_t1, q_t2)\n";
    }   
  
    writeSubCircuit(quantumsim_file, subcircuit);

    quantumsim_file << "    c.add_subcircuit(subc, name_map=[";

    for (int q = 0; q < num_qubits; q++){
      if (q != num_qubits-1)
	quantumsim_file << "'q" << q << "', ";
      else
	quantumsim_file << "'q" << q << "'";
    }

    quantumsim_file << "])\n\n";
    
  }
}

void writeSubCircuit(std::ofstream& quantumsim_file, compiler::SubCircuit& subs)
{		 
  for (auto elem : subs.getOperationsCluster()){
    t = t + cycle_t;
    writeSerialParallelOperations(quantumsim_file, *elem);
  }    
  
}
void writeSerialParallelOperations(std::ofstream& quantumsim_file, compiler::OperationsCluster& ops_cluster)
{
  if (ops_cluster.isParallel())
    {
      for (auto elem : ops_cluster.getOperations()){
	if (isValidOperation(*elem))
	  writeCQOperations(quantumsim_file, *elem);
      }
    }
    else
      {
	for (auto elem : ops_cluster.getOperations()){
	  if (isValidOperation(*elem))
	    writeCQOperations(quantumsim_file, *elem);
	}
      }
}

int isValidOperation(compiler::Operation& op)
{

  std::string type_ = op.getType();
  
  if (type_ == "measure_parity")
    {
        std::cout << std::endl;
        auto measureParityProperties = op.getMeasureParityQubitsAndAxis();
        measureParityProperties.first.first.printMembers();
	// quantumsim_file << measureParityProperties.first.first.getSelectedQubits().getIndices();??
        std::cout << "With axis " << measureParityProperties.second.first << std::endl;
        measureParityProperties.first.second.printMembers();
        std::cout << "With axis " << measureParityProperties.second.second << std::endl;

	return 0;
    }
  else if (type_ == "crk")
    {
      std::cout << std::endl;
      std::cout << "Qubit Pair 1: ";
      op.getTwoQubitPairs().first.printMembers();
      std::cout << "Qubit Pair 2: "; 
      op.getTwoQubitPairs().second.printMembers();
      std::cout << "Rotation = " << op.getRotationAngle() << std::endl;

      return 0;
    }
  else if (type_ == "toffoli")
    {
      return 0;
    }
  else if (type_ == "wait")
    {
        std::cout << std::endl;
        std::cout << "Wait time (integer) = " << op.getWaitTime() << std::endl;
	// t = t + cycle_t;
	return 0;
    }
  else if (type_ == "display")
    {
      op.getDisplayBits().printMembers();
      return 0;
    }
  return 1;
}

void writeCQOperations(std::ofstream& quantumsim_file, compiler::Operation& op)
{
  if (op.isBitControlled()){
      for (size_t elems : op.getControlBits().getSelectedBits().getIndices()){
	quantumsim_file << "    subc.add_qubit('b" << elems << "')\n";
      }

      quantumsim_file << "    subc.add_gate(ConditionalGate(control_bit=";

      for (size_t elems : op.getControlBits().getSelectedBits().getIndices()){
	quantumsim_file << "'b" << elems << "', ";
      }

      quantumsim_file << "time = " << t << ", zero_gates=[], one_gates=[";

      writeOperations(quantumsim_file, op);
      quantumsim_file << "])\n";
	
    }
    else {
      quantumsim_file << "    subc.add_gate(";
      writeOperations(quantumsim_file, op);
      quantumsim_file << ")\n";
    }
}

void writeOperations(std::ofstream& quantumsim_file, compiler::Operation& op)
// Potentially a function example where one can create an interface with whatever simulator/eQASM generator class.
{
     std::string type_ = op.getType();
    
    if ( type_ == "rx" || type_ == "ry" || type_ == "rz" )
    {
      quantumsim_file << type_ << "(";
      for (size_t elems : op.getQubitsInvolved().getSelectedQubits().getIndices()){
	quantumsim_file << "'q" << elems << "'";
      }
      quantumsim_file << ", time = " << t;
      quantumsim_file << ", angle = " << op.getRotationAngle();
      quantumsim_file << ")";
    }
    else if (type_ == "measure")
    {
      
      quantumsim_file << type_ << "(";
      for (size_t elems : op.getQubitsInvolved().getSelectedQubits().getIndices()){
	quantumsim_file << "'q" << elems << "'";
      }
      quantumsim_file << ", time = " << t;
      quantumsim_file << ", sampler = uniform_noisy_sampler(readout_error=0.03, seed=42)";
      quantumsim_file << ")";

      // throw "Gate non-existing in quantumsim";
      
    }        
    else if (type_ == "cnot" || type_ == "cz" || type_ == "swap")
    {
      quantumsim_file << type_ << "(";
      
      for (size_t elems : op.getQubitsInvolved(1).getSelectedQubits().getIndices()){
	quantumsim_file << "'q" << elems << "'";
      }
      quantumsim_file << ",";
      for (size_t elems : op.getTwoQubitPairs().second.getSelectedQubits().getIndices()){
	quantumsim_file << "'q" << elems << "'";
      }
      quantumsim_file << ", time = " << t;
      quantumsim_file << ")";
      
    }
    else if (type_ == "cr")
    {

      quantumsim_file << type_ << "(";
      
      for (size_t elems : op.getTwoQubitPairs().first.getSelectedQubits().getIndices()){
	quantumsim_file << "'q" << elems << "'";
      }
      quantumsim_file << ",";
      for (size_t elems : op.getTwoQubitPairs().second.getSelectedQubits().getIndices()){
	quantumsim_file << "'q" << elems << "'";
      }
      quantumsim_file << ", time = " << t;
      quantumsim_file << ", angle = " << op.getRotationAngle();
      quantumsim_file << ")";     

    }
    // else if (type_ == "toffoli")
    // {
    //   quantumsim_file << type_ << "(";
    //   for (size_t elems : op.getToffoliQubitPairs().first.getSelectedQubits().getIndices()){
    // 	quantumsim_file << "'q" << elems << "'";
    //   }
    //   quantumsim_file << ",";
    //   for (size_t elems : op.getToffoliQubitPairs().second.first.getSelectedQubits().getIndices()){
    // 	quantumsim_file << "'q" << elems << "'";
    //   }
    //   quantumsim_file << ",";
    //   for (size_t elems : op.getToffoliQubitPairs().second.second.getSelectedQubits().getIndices()){
    // 	quantumsim_file << "'q" << elems << "'";
    //   }
    //   quantumsim_file << ", time = " << t;
    //   quantumsim_file << ")";
    // }
    else {
      quantumsim_file << type_ << "(";
      for (size_t elems : op.getQubitsInvolved().getSelectedQubits().getIndices()){
	quantumsim_file << "'q" << elems << "'";
      }
      quantumsim_file << ", time = " << t;
      quantumsim_file << ")";
    }
    
}

void writeTail(std::ofstream& quantumsim_file)
{
  // ADDING MEASUREMENT PER QUBIT, BUT THE TIMING IS NOT ACCURATE
  for (int q = 0; q < num_qubits; q++){
    quantumsim_file << "    sampler = uniform_noisy_sampler(readout_error=0.03, seed=42)\n";
    quantumsim_file << "    c.add_qubit('m" << q << "')\n";
    quantumsim_file << "    c.add_measurement('q" << q << "', time=" << t << ", output_bit='m" << q << "', sampler=sampler)\n";
    quantumsim_file << "\n";
  }
  
  quantumsim_file << "\n    return c\n\n";
}

// int writeOperation()?
