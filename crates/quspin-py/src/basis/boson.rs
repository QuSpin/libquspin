use crate::basis::{
    group_n_sites_lhss, parse_seeds, parse_state_str, py_any_to_c64_vec, py_int_to_state_bytes,
    replay_group_into_generic, state_bytes_to_py_int, state_bytes_to_state_string,
    state_vec_to_pyarray,
    validate_op_max_site,
};
use crate::error::Error;
use crate::operator::bond::PyBondOperator;
use crate::operator::boson::PyBosonOperator;
use pyo3::prelude::*;
use pyo3::types::PyType;
use quspin_core::project_to as project_between_bases;
use quspin_core::basis::{BosonBasis, SpaceKind};

/// Python-facing bosonic basis.
///
/// `lhss` is the number of on-site Fock states (≥ 2).
#[pyclass(name = "BosonBasis", module = "quspin._rs")]
pub struct PyBosonBasis {
    pub inner: BosonBasis,
}

fn build_boson_basis(
    basis: &mut BosonBasis,
    ham: &Bound<'_, PyAny>,
    n_sites: usize,
    byte_seeds: &[Vec<u8>],
) -> PyResult<()> {
    if let Ok(op) = ham.cast::<PyBosonOperator>() {
        let op = op.borrow();
        validate_op_max_site(op.inner.max_site(), n_sites)?;
        basis.build(&op.inner, byte_seeds).map_err(Error::from)?;
    } else if let Ok(op) = ham.cast::<PyBondOperator>() {
        let op = op.borrow();
        validate_op_max_site(op.inner.max_site(), n_sites)?;
        basis.build(&op.inner, byte_seeds).map_err(Error::from)?;
    } else {
        return Err(pyo3::exceptions::PyTypeError::new_err(
            "ham must be a BosonOperator or BondOperator",
        ));
    }
    Ok(())
}

#[pymethods]
impl PyBosonBasis {
    /// Full Hilbert space (no projection, no build step required).
    ///
    /// Args:
    ///     n_sites: number of lattice sites.
    ///     lhss:    on-site Fock-state count (≥ 2).
    #[classmethod]
    fn full(_cls: &Bound<'_, PyType>, n_sites: usize, lhss: usize) -> PyResult<Self> {
        let inner = BosonBasis::new(n_sites, lhss, SpaceKind::Full).map_err(Error::from)?;
        Ok(PyBosonBasis { inner })
    }

    /// Particle-number sector subspace.
    ///
    /// Args:
    ///     n_sites: number of lattice sites.
    ///     lhss:    on-site Fock-state count (≥ 2).
    ///     ham:     `BosonOperator` or `BondOperator` used for BFS.
    ///     seeds:   list of seed state strings (one digit per site for LHSS>2).
    #[classmethod]
    fn subspace(
        _cls: &Bound<'_, PyType>,
        n_sites: usize,
        lhss: usize,
        ham: &Bound<'_, PyAny>,
        seeds: Vec<String>,
    ) -> PyResult<Self> {
        let byte_seeds = parse_seeds(&seeds, n_sites, lhss)?;
        let mut basis = BosonBasis::new(n_sites, lhss, SpaceKind::Sub).map_err(Error::from)?;
        build_boson_basis(&mut basis, ham, n_sites, &byte_seeds)?;
        Ok(PyBosonBasis { inner: basis })
    }

    /// Symmetry-reduced subspace.
    ///
    /// Args:
    ///     group: a :class:`SymmetryGroup` describing the symmetry group;
    ///            `n_sites` and `lhss` are read from `group.n_sites` /
    ///            `group.lhss`.
    ///     ham:   `BosonOperator` or `BondOperator` used for BFS.
    ///     seeds: list of seed state strings.
    #[classmethod]
    #[pyo3(signature = (group, ham, seeds))]
    fn symmetric(
        _cls: &Bound<'_, PyType>,
        group: &Bound<'_, PyAny>,
        ham: &Bound<'_, PyAny>,
        seeds: Vec<String>,
    ) -> PyResult<Self> {
        let (n_sites, lhss) = group_n_sites_lhss(group)?;
        let byte_seeds = parse_seeds(&seeds, n_sites, lhss)?;
        let mut basis = BosonBasis::new(n_sites, lhss, SpaceKind::Symm).map_err(Error::from)?;
        replay_group_into_generic(group, &mut basis.inner)?;
        build_boson_basis(&mut basis, ham, n_sites, &byte_seeds)?;
        Ok(PyBosonBasis { inner: basis })
    }

    // ------------------------------------------------------------------
    // Properties
    // ------------------------------------------------------------------

    #[getter]
    fn n_sites(&self) -> usize {
        self.inner.inner.n_sites()
    }

    #[getter]
    fn lhss(&self) -> usize {
        self.inner.inner.lhss()
    }

    #[getter]
    fn size(&self) -> usize {
        self.inner.inner.size()
    }

    #[getter]
    #[pyo3(name = "Ns")]
    fn size_alias(&self) -> usize {
        self.inner.inner.size()
    }

    #[getter]
    fn is_built(&self) -> bool {
        self.inner.inner.is_built()
    }

    // ------------------------------------------------------------------
    // Methods
    // ------------------------------------------------------------------

    /// Return the `i`-th basis state as a string of site occupations.
    fn state_at(&self, i: usize) -> PyResult<String> {
        if i >= self.inner.inner.size() {
            return Err(pyo3::exceptions::PyIndexError::new_err(format!(
                "index {i} out of range for basis of size {}",
                self.inner.inner.size()
            )));
        }
        Ok(self.inner.inner.state_at_str(i))
    }

    /// Return the integer representation of `state_str`.
    fn state_to_int(&self, py: Python<'_>, state_str: &str) -> PyResult<Py<PyAny>> {
        let bytes = parse_state_str(
            state_str,
            self.inner.inner.n_sites(),
            self.inner.inner.lhss(),
        )?;
        state_bytes_to_py_int(py, &bytes, self.inner.inner.lhss())
    }

    #[pyo3(signature = (state_int, bracket_notation = true))]
    fn int_to_state(
        &self,
        state_int: &Bound<'_, PyAny>,
        bracket_notation: bool,
    ) -> PyResult<String> {
        let bytes = py_int_to_state_bytes(
            state_int,
            self.inner.inner.n_sites(),
            self.inner.inner.lhss(),
        )?;
        Ok(state_bytes_to_state_string(&bytes, bracket_notation))
    }

    /// Return the index of `state_str`, or `None` if absent.
    #[pyo3(name = "index_str")]
    fn index_str(&self, state_str: &str) -> PyResult<Option<usize>> {
        let bytes = parse_state_str(
            state_str,
            self.inner.inner.n_sites(),
            self.inner.inner.lhss(),
        )?;
        Ok(self.inner.inner.index_of_bytes(&bytes))
    }

    /// Return the index of an integer-encoded basis state, or `None` if absent.
    ///
    /// `state_int` is the raw integer representation (same convention as the
    /// "integer repr." column in `print(basis)`).
    #[pyo3(name = "index")]
    fn index_int_raw(&self, state_int: &Bound<'_, PyAny>) -> PyResult<Option<usize>> {
        let bytes = py_int_to_state_bytes(
            state_int,
            self.inner.inner.n_sites(),
            self.inner.inner.lhss(),
        )?;
        Ok(self.inner.inner.index_of_bytes(&bytes))
    }

    /// Project a full-basis vector into this basis.
    ///
    /// The `sparse` argument is accepted for API compatibility and currently
    /// ignored; this method returns a dense `numpy.ndarray`.
    #[pyo3(signature = (state, sparse = true))]
    fn project_to(
        &self,
        py: Python<'_>,
        state: &Bound<'_, PyAny>,
        sparse: bool,
    ) -> PyResult<Py<PyAny>> {
        let _ = sparse;

        let in_vec = py_any_to_c64_vec(state)?;
        let full_basis = quspin_core::basis::GenericBasis::new(
            self.inner.inner.n_sites(),
            self.inner.inner.lhss(),
            SpaceKind::Full,
            false,
        )
        .map_err(Error::from)?;

        let out_rows = self.inner.inner.size();
        let mut out_vec = vec![num_complex::Complex::<f64>::new(0.0, 0.0); out_rows * in_vec.ncols];
        for col in 0..in_vec.ncols {
            let in_col = &in_vec.data[col * in_vec.nrows..(col + 1) * in_vec.nrows];
            let out_col = &mut out_vec[col * out_rows..(col + 1) * out_rows];
            project_between_bases(&full_basis, &self.inner.inner, in_col, out_col, true)
                .map_err(Error::from)?;
        }

        Ok(state_vec_to_pyarray(py, &out_vec, in_vec.is_complex, out_rows, in_vec.ncols, in_vec.is_matrix))
    }

    /// Expand a vector in this basis to the full Hilbert-space basis.
    ///
    /// The `sparse` argument is accepted for API compatibility and currently
    /// ignored; this method returns a dense `numpy.ndarray`.
    #[pyo3(signature = (state, sparse = true))]
    fn project_from(
        &self,
        py: Python<'_>,
        state: &Bound<'_, PyAny>,
        sparse: bool,
    ) -> PyResult<Py<PyAny>> {
        let _ = sparse;

        let in_vec = py_any_to_c64_vec(state)?;
        let full_basis = quspin_core::basis::GenericBasis::new(
            self.inner.inner.n_sites(),
            self.inner.inner.lhss(),
            SpaceKind::Full,
            false,
        )
        .map_err(Error::from)?;

        let out_rows = full_basis.size();
        let mut out_vec = vec![num_complex::Complex::<f64>::new(0.0, 0.0); out_rows * in_vec.ncols];
        for col in 0..in_vec.ncols {
            let in_col = &in_vec.data[col * in_vec.nrows..(col + 1) * in_vec.nrows];
            let out_col = &mut out_vec[col * out_rows..(col + 1) * out_rows];
            project_between_bases(&self.inner.inner, &full_basis, in_col, out_col, true)
                .map_err(Error::from)?;
        }

        Ok(state_vec_to_pyarray(py, &out_vec, in_vec.is_complex, out_rows, in_vec.ncols, in_vec.is_matrix))
    }

    fn __str__(&self) -> String {
        format!("{}", self.inner.inner)
    }

    fn __repr__(&self) -> String {
        format!(
            "BosonBasis(n_sites={}, lhss={}, size={}, kind={})",
            self.inner.inner.n_sites(),
            self.inner.inner.lhss(),
            self.inner.inner.size(),
            self.inner.inner.kind(),
        )
    }
}
