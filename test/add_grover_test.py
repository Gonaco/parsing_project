
N_exp = 10
N_qubits = 9
# CIRCUIT DECLARATION
c = circuit_function(3500, 1500, 0.01, '000000000')

# SIMULATING
sdm = sparsedm.SparseDM(c.get_qubit_names())

measurements = []

# c.apply_to(sdm)
# measurements = [sdm.classical["m0"],
#                 sdm.classical["m1"], sdm.classical["m2"]]

# return np.array(measurements, dtype=float)

for i in range(N_exp):
    c.apply_to(sdm)
    
    for q in range(N_qubits):
        measurements.append(sdm.classical["m"+str(q)])
        
print(measurements)
