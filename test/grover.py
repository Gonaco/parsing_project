import numpy as np
from quantumsim.circuit import Circuit
from quantumsim.circuit import uniform_noisy_sampler
from quantumsim.circuit import IdlingGate as i
from quantumsim.circuit import RotateY as ry
from quantumsim.circuit import RotateX as rx
from quantumsim.circuit import RotateZ as rz
from quantumsim.circuit import Hadamard as h
from quantumsim.circuit import CPhase as cz
from quantumsim.circuit import CNOT as cnot
from quantumsim.circuit import Swap as swap
from quantumsim.circuit import CPhaseRotation as cr
from quantumsim.circuit import ConditionalGate as ConditionalGate
from quantumsim.circuit import RotateEuler as RotateEuler
from quantumsim.circuit import ResetGate as ResetGate
from quantumsim.circuit import Measurement as measure
import quantumsim.sparsedm as sparsedm

# print('GPU is used:', sparsedm.using_gpu)


def t(q, time):
    return RotateEuler(q, time=time, theta=0, phi=np.pi/4, lamda=0)

def tdag(q, time):
    return RotateEuler(q, time=time, theta=0, phi=-np.pi/4, lamda=0)

def measure_z(q, time, sampler):
    return measure(q, time, sampler)

def z(q, time):
    return rz(q, time, angle=np.pi)

def x(q, time):
    return rx(q, time, angle=np.pi)

def y(q, time):
    return ry(q, time, angle=np.pi)

def x90(q, time):
    return rx(q, time, angle=np.pi/2)

def y90(q, time):
    return ry(q, time, angle=np.pi/2)

def mx90(q, time):
    return rx(q, time, angle=-np.pi/2)

def my90(q, time):
    return ry(q, time, angle=-np.pi/2)

def prep_z(q, time):
    return ResetGate(q, time, state=0)

def circuit_function(q_t1=3000, q_t2=1500, error=0.01, init_state=''):

    c = Circuit(title='default')

    c.add_gate(ResetGate('q0', time=1, state=int(init_state[0])))
    c.add_gate(ResetGate('q1', time=1, state=int(init_state[1])))
    c.add_gate(ResetGate('q2', time=1, state=int(init_state[2])))
    c.add_gate(ResetGate('q3', time=1, state=int(init_state[3])))
    c.add_gate(ResetGate('q4', time=1, state=int(init_state[4])))
    c.add_gate(ResetGate('q5', time=1, state=int(init_state[5])))
    c.add_gate(ResetGate('q6', time=1, state=int(init_state[6])))
    c.add_gate(ResetGate('q7', time=1, state=int(init_state[7])))
    c.add_gate(ResetGate('q8', time=1, state=int(init_state[8])))
    c.add_qubit('q0', q_t1, q_t2)
    c.add_qubit('q1', q_t1, q_t2)
    c.add_qubit('q2', q_t1, q_t2)
    c.add_qubit('q3', q_t1, q_t2)
    c.add_qubit('q4', q_t1, q_t2)
    c.add_qubit('q5', q_t1, q_t2)
    c.add_qubit('q6', q_t1, q_t2)
    c.add_qubit('q7', q_t1, q_t2)
    c.add_qubit('q8', q_t1, q_t2)
    subc = Circuit(title='default')
    subc.add_qubit('q0', q_t1, q_t2)
    subc.add_qubit('q1', q_t1, q_t2)
    subc.add_qubit('q2', q_t1, q_t2)
    subc.add_qubit('q3', q_t1, q_t2)
    subc.add_qubit('q4', q_t1, q_t2)
    subc.add_qubit('q5', q_t1, q_t2)
    subc.add_qubit('q6', q_t1, q_t2)
    subc.add_qubit('q7', q_t1, q_t2)
    subc.add_qubit('q8', q_t1, q_t2)
    c.add_subcircuit(subc, name_map=['q0', 'q1', 'q2', 'q3', 'q4', 'q5', 'q6', 'q7', 'q8'])

    subc = Circuit(title='init')
    subc.add_qubit('q0', q_t1, q_t2)
    subc.add_qubit('q1', q_t1, q_t2)
    subc.add_qubit('q2', q_t1, q_t2)
    subc.add_qubit('q3', q_t1, q_t2)
    subc.add_qubit('q4', q_t1, q_t2)
    subc.add_qubit('q5', q_t1, q_t2)
    subc.add_qubit('q6', q_t1, q_t2)
    subc.add_qubit('q7', q_t1, q_t2)
    subc.add_qubit('q8', q_t1, q_t2)
    subc.add_gate(x('q4', time = 20))
    subc.add_gate(h('q0', time = 40))
    subc.add_gate(h('q1', time = 40))
    subc.add_gate(h('q2', time = 40))
    subc.add_gate(h('q3', time = 40))
    subc.add_gate(h('q4', time = 40))
    c.add_subcircuit(subc, name_map=['q0', 'q1', 'q2', 'q3', 'q4', 'q5', 'q6', 'q7', 'q8'])

    subc = Circuit(title='grover')
    subc.add_qubit('q0', q_t1, q_t2)
    subc.add_qubit('q1', q_t1, q_t2)
    subc.add_qubit('q2', q_t1, q_t2)
    subc.add_qubit('q3', q_t1, q_t2)
    subc.add_qubit('q4', q_t1, q_t2)
    subc.add_qubit('q5', q_t1, q_t2)
    subc.add_qubit('q6', q_t1, q_t2)
    subc.add_qubit('q7', q_t1, q_t2)
    subc.add_qubit('q8', q_t1, q_t2)
    subc.add_gate(x('q2', time = 60))
    subc.add_gate(cnot('q8','q4', time = 160))
    subc.add_gate(x('q2', time = 260))
    subc.add_gate(h('q0', time = 280))
    subc.add_gate(h('q1', time = 280))
    subc.add_gate(h('q2', time = 280))
    subc.add_gate(h('q3', time = 280))
    subc.add_gate(x('q0', time = 300))
    subc.add_gate(x('q1', time = 300))
    subc.add_gate(x('q2', time = 300))
    subc.add_gate(x('q3', time = 300))
    subc.add_gate(h('q3', time = 320))
    subc.add_gate(cnot('q7','q3', time = 400))
    subc.add_gate(h('q3', time = 480))
    subc.add_gate(x('q0', time = 500))
    subc.add_gate(x('q1', time = 500))
    subc.add_gate(x('q2', time = 500))
    subc.add_gate(x('q3', time = 500))
    subc.add_gate(h('q0', time = 520))
    subc.add_gate(h('q1', time = 520))
    subc.add_gate(h('q2', time = 520))
    subc.add_gate(h('q3', time = 520))
    c.add_subcircuit(subc, name_map=['q0', 'q1', 'q2', 'q3', 'q4', 'q5', 'q6', 'q7', 'q8'])

    subc = Circuit(title='final_measurement')
    subc.add_qubit('q0', q_t1, q_t2)
    subc.add_qubit('q1', q_t1, q_t2)
    subc.add_qubit('q2', q_t1, q_t2)
    subc.add_qubit('q3', q_t1, q_t2)
    subc.add_qubit('q4', q_t1, q_t2)
    subc.add_qubit('q5', q_t1, q_t2)
    subc.add_qubit('q6', q_t1, q_t2)
    subc.add_qubit('q7', q_t1, q_t2)
    subc.add_qubit('q8', q_t1, q_t2)
    subc.add_gate(h('q4', time = 560))
    subc.add_gate(measure('q4', time = 580, sampler = uniform_noisy_sampler(readout_error=0.03, seed=42)))
    c.add_subcircuit(subc, name_map=['q0', 'q1', 'q2', 'q3', 'q4', 'q5', 'q6', 'q7', 'q8'])

    sampler = uniform_noisy_sampler(readout_error=0.03, seed=42)
    c.add_qubit('m0')
    c.add_measurement('q0', time=600, output_bit='m0', sampler=sampler)

    sampler = uniform_noisy_sampler(readout_error=0.03, seed=42)
    c.add_qubit('m1')
    c.add_measurement('q1', time=600, output_bit='m1', sampler=sampler)

    sampler = uniform_noisy_sampler(readout_error=0.03, seed=42)
    c.add_qubit('m2')
    c.add_measurement('q2', time=600, output_bit='m2', sampler=sampler)

    sampler = uniform_noisy_sampler(readout_error=0.03, seed=42)
    c.add_qubit('m3')
    c.add_measurement('q3', time=600, output_bit='m3', sampler=sampler)

    sampler = uniform_noisy_sampler(readout_error=0.03, seed=42)
    c.add_qubit('m4')
    c.add_measurement('q4', time=600, output_bit='m4', sampler=sampler)

    sampler = uniform_noisy_sampler(readout_error=0.03, seed=42)
    c.add_qubit('m5')
    c.add_measurement('q5', time=600, output_bit='m5', sampler=sampler)

    sampler = uniform_noisy_sampler(readout_error=0.03, seed=42)
    c.add_qubit('m6')
    c.add_measurement('q6', time=600, output_bit='m6', sampler=sampler)

    sampler = uniform_noisy_sampler(readout_error=0.03, seed=42)
    c.add_qubit('m7')
    c.add_measurement('q7', time=600, output_bit='m7', sampler=sampler)

    sampler = uniform_noisy_sampler(readout_error=0.03, seed=42)
    c.add_qubit('m8')
    c.add_measurement('q8', time=600, output_bit='m8', sampler=sampler)


    return c

