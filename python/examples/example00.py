#
import sys, os

os.environ["KMP_DUPLICATE_LIB_OK"] = (
    "True"  # uncomment this line if omp error occurs on OSX for python 3
)
os.environ["OMP_NUM_THREADS"] = "1"  # set number of OpenMP threads to run in parallel
os.environ["MKL_NUM_THREADS"] = "1"  # set number of MKL threads to run in parallel
#

#
# line 12 and line 13 below are for development purposes and can be removed
qspin_path = os.path.join(os.getcwd(), "../../")
sys.path.insert(0, qspin_path)
#####################################################################
#                            example 00                             #
#    In this script we demonstrate how to use QuSpin's              #
#    `basis_general` routines to construct, interpret,              #
#    and use basis objects.                                         #
#####################################################################
import numpy as np  # generic math functions
from quspin_rs._rs import SpinBasis # Hilbert space spin basis
from quspin_rs import SymmetryGroup, Lattice # symmetry group and lattice objects
from quspin_rs import PauliOperator # operator objects
#
L = 2  # system size
#
#############################################
print("\n----------------------------")
print("---  NO  SYMMETRIES  -------")
print("----------------------------\n")
#############################################
#
##### basis objects without symmetries
#
basis = SpinBasis.full(L)
Ns = basis.Ns  # number of states in the basis
#
print(basis)
#
# states in integer representation
states = basis.states  # = third column when printing the basis object (not consecutive if there are symmetries --> see below)
array_inds = np.arange(basis.Ns)  # = first column when printing the basis object (always consecutive)
#
print("\n'array index' and 'states' columns when printing the basis:")
print("array indices:", array_inds)
print("states in int rep:", states)
#
# find array index of a state from its integer representation; Note: the array index is crucial for reading-off matrix elements
s = basis.states[2]
array_ind_s = basis.index(s)  # = array_inds[2] whenever there are no symmetries defined in the basis
print("\nprint array index of s, and s (in int rep); Note: the array index is crucial for reading-off matrix elements")
print(array_ind_s, s)
# --------------------------------------------
##### States: ket and integer representations
# --------------------------------------------
# find integer representation from Fock state string
fock_state_str_s = "|01>"  # works also if the ket-forming strings | > are omitted
int_rep_s = basis.state_to_int(fock_state_str_s)
print("\nprint Fock state string of s, and s (in int rep):")
print(fock_state_str_s, int_rep_s)
#
# find Fock state string from integer representation
fock_s = basis.int_to_state(int_rep_s, bracket_notation=True)
print("\nprint Fock state string of s, and s (in int rep):")
print(fock_s, int_rep_s)
# same as above but dropping the ket-forming strings | >
fock_s = basis.int_to_state(int_rep_s, bracket_notation=False)
print("print Fock state string (without | and >) of s, and s (in int rep):")
print(fock_s, int_rep_s)
#
# find Fock state from array index
array_ind_s = 2
int_rep_s = basis.states[array_ind_s]
fock_s = basis.int_to_state(int_rep_s, bracket_notation=True)
print("\nprint array index, int rep, and fock state rep of s:")
print(array_ind_s, int_rep_s, fock_s)  # compare with print(basis) output
# --------------------------------------------
##### States: array/vector representation
# --------------------------------------------
# define a zero vector of size set by the basis dimenion
psi_s = np.zeros(basis.Ns)
# obtain array index for the fock state |01>
array_ind_s = basis.index(basis.state_to_int("01"))
# construct the pure state |01>
psi_s[array_ind_s] = 1.0
print("\nprint state psi_s in the basis:")
print(psi_s)
#
#############################################
print("\n----------------------------")
print("-------  OPERATORS  -------")
print("----------------------------\n")
#############################################
#
##### PauliOperator objects are required to generate basis objects with symmetries
#
# example of two-site Pauli operator which breaks all symmetries except reflection
twosite_op = PauliOperator(
        [
            ("XX", [[0.73, 0, 1]]),                # XX interaction between sites 0 and 1 with strength 0.73
            ("X",  [[0.31, 0], [0.31, 1]]),        # transverse field in X direction on sites 0 and 1 with strength 0.31 
            ("Z",  [[0.17, 0], [0.17, 1]]),        # longitudinal field in Z direction on sites 0 and 1 with strength 0.17 
            ("XZ", [[0.23, 0, 1], [0.23, 1, 0]]),  # XZ interaction between sites 0 and 1 with strength 0.23 and XZ interaction between sites 1 and 0 with strength 0.23
        ]
    )
#
print("\nThe PauliOperator object is not yet a Hamiltonian matrix; printing it returns:")
print(twosite_op)
#
# example of XX chain PauliOperator with periodic boundary conditions: has more symmetries than just reflection/parity
bonds = [[1.0, i, (i + 1) % L] for i in range(L)] # nearest-neighbor chain with periodic boundary conditions
XX_chain_op = PauliOperator([("XX", bonds)])
#
#############################################
print("\n\n\n----------------------------")
print("-------  SYMMETRIES  -------")
print("----------------------------\n")
#############################################
#
##### basis objects with symmetries are generated by specifying a symmetry group and a PauliOperator which generates the states in a given symmetry sector by a repeated application on a set of seed states. 
#
sites = np.arange(L)  # define lattice sites
P = sites[::-1]  # define action of parity/reflection on the lattice sites
#
# generate triplet group with parity symmetry sector +1
group_triplet = SymmetryGroup(n_sites=L, lhss=2) # lhss = local Hilbert space size
group_triplet.add_cyclic(Lattice(P), eta=+1)  # add parity/reflection symmetry to the group; eta = +1 specifies the +1 symmetry sector
#
# generate triplet group with parity symmetry sector +1
group_singlet = SymmetryGroup(n_sites=L, lhss=2) # lhss = local Hilbert space size
group_singlet.add_cyclic(Lattice(P), eta=-1)  # add parity/reflection symmetry to the group; eta = -1 specifies the -1 symmetry sector
group_singlet.validate()  # check that the group is closed
#
# --------------------------------------------------
##### minimal demo: why operator + seed choice matters
# --------------------------------------------------
print("\nOperator/seed demo (triplet sector, eta=+1):\n")
#
# XX_chain_op has extra symmetries; with one seed it may not reach all triplet states.
triplet_xx_one_seed = SpinBasis.symmetric(group_triplet, XX_chain_op, seeds=['01'])
print("1) XX_chain_op + seeds=['01'] -> size:", triplet_xx_one_seed.size)
print("   states:", triplet_xx_one_seed.states)
print("XX_chain_op has extra symmetries; with one seed it may not reach all triplet states.\n")
#
# Adding a second seed reaches the disconnected component and recovers the full triplet block.
triplet_xx_two_seeds = SpinBasis.symmetric(group_triplet, XX_chain_op, seeds=['01', '00'])
print("2) XX_chain_op + seeds=['01','00'] -> size:", triplet_xx_two_seeds.size)
print("   states:", triplet_xx_two_seeds.states)
print("Adding a second seed reaches the disconnected component and recovers the full triplet block.\n")
#
# twosite_op breaks extra symmetries; one seed is enough to generate the full triplet block.
triplet_twosite_one_seed = SpinBasis.symmetric(group_triplet, twosite_op, seeds=['01'])
print("3) twosite_op + seeds=['01'] -> size:", triplet_twosite_one_seed.size)
print("   states:", triplet_twosite_one_seed.states)
print("twosite_op breaks extra symmetries; one seed is enough to generate the full triplet block.\n\n")
# --------------------------------------------------
#
# build singlet and triplet bases
basis_triplet = SpinBasis.symmetric(group_triplet, twosite_op, seeds=['01']) # use twosite_op to reach all states in the triplet sector 
basis_singlet = SpinBasis.symmetric(group_singlet, twosite_op, seeds=['00']) # acting on |00> with twosite_op reaches states in the 01/10 orbit which form the singlet
#
print("print full basis:")
print(basis)
#
print("\n\nprint pblock=+1 basis:\n")
print(basis_triplet)
#
print("\n  * integer rep column no longer consecutive! (|01> falls outside symmetry sector)")
print("  * array index column still consecutive! (but indices differ compared to full basis, e.g. for |00>)")
print("  * |11> and |00> invariant under parity, so they correspond to physical states |11> and |00>")
print("  * |10> not invariant under parity! It represents the physical symmetric superposition 1/sqrt(2)(|10> + |01>) [see bottom note when printing the symmetry-reduced basis]; quspin keeps track of the coefficient 1/sqrt(2) under the hood.")
print("\n\nprint pblock=-1 basis:\n")
#
print(basis_singlet)
print(
    "  * |10> here represents the physical ANTI-symmetric superposition 1/sqrt(2)(|10> - |01>) [see bottom note when printing the symmetry-reduced basis]"
)
print(
    "  *  NOTE: same state |01> is used to label both the symmetric and antisymmetric superposition because in this cases quspin uses the smallest integer from the integer representations of the states comprising the superposition states.\n"
)
#
# --------------------------------------------------
##### transform states from one basis to the other
# --------------------------------------------------
#
array_ind_s = basis_triplet.index(basis.state_to_int("01")) ### FLIP BACK TO 10 ONCE BASIS ORDER IS FIXED
psi_symm_s = np.zeros(basis_triplet.Ns)
psi_symm_s[array_ind_s] = 1.0  # create the state |10> + |01> in basis_triplet
print("print state psi_symm_s in the symmetry-reduced basis_triplet:")
print(psi_symm_s)
#
# compute corresponding state in the full basis
psi_s = basis_triplet.project_from(psi_symm_s, sparse=False)
print("\nprint state psi_s in the full basis: (note the factor 1/sqrt(2) which comes out correct.")
print(psi_s)
#
# one can also project a full-basis state to a symmetry-reduced basis
psi_s = np.zeros(basis.Ns)
array_ind_s = basis.index(basis.state_to_int("01"))
psi_s[array_ind_s] = 1.0  # create the state |01> in the full basis
#
psi_symm_s = basis_singlet.project_to(psi_s, sparse=False)  # projects |01> to 1/sqrt(2) (|01> - |10>) in basis_singlet
print("\nprint state psi_symm_s in the symmetry-reduced basis_singlet; NOTE: projection does not give a normalized state!")
print(psi_symm_s)
# normalize
psi_symm_s = psi_symm_s / np.linalg.norm(psi_symm_s)
#
# lift the projected state back to full basis
psi_lifted_s = basis_singlet.project_from(psi_symm_s, sparse=False)  # corresponds to the projection 1/sqrt(2) (|01> - |10>) in the full basis
print("\nprint state psi_lifted_s = 1/sqrt(2) (|01> - |10>) in the full basis; NOTE: info lost by the first projection!")
print(psi_lifted_s)