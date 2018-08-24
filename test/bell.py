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

    subc = Circuit(title='default')
    subc.add_qubit('q0', q_t1, q_t2)
    subc.add_qubit('q1', q_t1, q_t2)
    subc.add_gate(prep_z('q0', time = 0))
    subc.add_gate(prep_z('q1', time = 0))
    subc.add_gate(h('q0', time = 20))
    subc.add_gate(cnot('q0','q1', time = 40))
    subc.add_gate(measure('q0', time = 60, sampler = uniform_noisy_sampler(readout_error=0.03, seed=42)))
    subc.add_gate(measure('q1', time = 80, sampler = uniform_noisy_sampler(readout_error=0.03, seed=42)))
    subc.add_gate(measure('q0', time = 100, sampler = uniform_noisy_sampler(readout_error=0.03, seed=42)))
    subc.add_gate(measure('q1', time = 100, sampler = uniform_noisy_sampler(readout_error=0.03, seed=42)))
    c.add_subcircuit(subc)

    subc = Circuit(title='display_bits')
    subc.add_qubit('q0', q_t1, q_t2)
    subc.add_qubit('q1', q_t1, q_t2)
    c.add_subcircuit(subc)


    return c

