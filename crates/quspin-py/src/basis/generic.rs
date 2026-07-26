use crate::basis::{
    group_n_sites_lhss, parse_seeds, parse_state_str, py_any_to_c64_vec, py_int_to_state_bytes,
    replay_group_into_generic, state_bytes_to_py_int, state_bytes_to_state_string,
    state_vec_to_pyarray,
    validate_op_max_site,
};
use crate::error::Error;
use crate::operator::monomial::PyMonomialOperator;
use pyo3::prelude::*;
use pyo3::types::PyType;
use quspin_core::project_to as project_between_bases;
use quspin_core::basis::{GenericBasis, SpaceKind};

/// Python-facing generic basis for any on-site Hilbert-space size.
///
/// Supports both lattice (site-permutation) and local (dit-permutation)
/// symmetries.  Paired with `MonomialOperator` for Hamiltonian construction.
#[pyclass(name = "GenericBasis", module = "quspin._rs")]
pub struct PyGenericBasis {
    pub inner: GenericBasis,
}

#[pymethods]
impl PyGenericBasis {
    /// Full Hilbert space (no projection, no build step required).
    ///
    /// Args:
    ///     n_sites: number of lattice sites.
    ///     lhss:    on-site state count (≥ 2).
    #[classmethod]
    fn full(_cls: &Bound<'_, PyType>, n_sites: usize, lhss: usize) -> PyResult<Self> {
        let inner =
            GenericBasis::new(n_sites, lhss, SpaceKind::Full, false).map_err(Error::from)?;
        Ok(PyGenericBasis { inner })
    }

    /// Subspace built by BFS from seed states using a `MonomialOperator`.
    ///
    /// Args:
    ///     n_sites: number of lattice sites.
    ///     lhss:    on-site state count (≥ 2).
    ///     ham:     `MonomialOperator` used for BFS.
    ///     seeds:   list of seed state strings (one digit per site).
    #[classmethod]
    fn subspace(
        _cls: &Bound<'_, PyType>,
        n_sites: usize,
        lhss: usize,
        ham: &PyMonomialOperator,
        seeds: Vec<String>,
    ) -> PyResult<Self> {
        validate_op_max_site(ham.inner.max_site(), n_sites)?;
        let byte_seeds = parse_seeds(&seeds, n_sites, lhss)?;
        let mut basis =
            GenericBasis::new(n_sites, lhss, SpaceKind::Sub, false).map_err(Error::from)?;
        basis.build(&ham.inner, &byte_seeds).map_err(Error::from)?;
        Ok(PyGenericBasis { inner: basis })
    }

    /// Symmetry-reduced subspace.
    ///
    /// Args:
    ///     group: a :class:`SymmetryGroup` describing the symmetry group;
    ///            `n_sites` and `lhss` are read from `group.n_sites` /
    ///            `group.lhss`.
    ///     ham:   `MonomialOperator` used for BFS.
    ///     seeds: list of seed state strings.
    #[classmethod]
    #[pyo3(signature = (group, ham, seeds))]
    fn symmetric(
        _cls: &Bound<'_, PyType>,
        group: &Bound<'_, PyAny>,
        ham: &PyMonomialOperator,
        seeds: Vec<String>,
    ) -> PyResult<Self> {
        let (n_sites, lhss) = group_n_sites_lhss(group)?;
        validate_op_max_site(ham.inner.max_site(), n_sites)?;
        let byte_seeds = parse_seeds(&seeds, n_sites, lhss)?;
        let mut basis =
            GenericBasis::new(n_sites, lhss, SpaceKind::Symm, false).map_err(Error::from)?;
        replay_group_into_generic(group, &mut basis)?;
        basis.build(&ham.inner, &byte_seeds).map_err(Error::from)?;
        Ok(PyGenericBasis { inner: basis })
    }

    // ------------------------------------------------------------------
    // Properties
    // ------------------------------------------------------------------

    #[getter]
    fn n_sites(&self) -> usize {
        self.inner.n_sites()
    }

    #[getter]
    fn lhss(&self) -> usize {
        self.inner.lhss()
    }

    #[getter]
    fn size(&self) -> usize {
        self.inner.size()
    }

    #[getter]
    #[pyo3(name = "Ns")]
    fn n_size_alias(&self) -> usize {
        self.inner.size()
    }

    #[getter]
    fn is_built(&self) -> bool {
        self.inner.is_built()
    }

    // ------------------------------------------------------------------
    // Methods
    // ------------------------------------------------------------------

    /// Return the `i`-th basis state as a string of site occupations.
    fn state_at(&self, i: usize) -> PyResult<String> {
        if i >= self.inner.size() {
            return Err(pyo3::exceptions::PyIndexError::new_err(format!(
                "index {i} out of range for basis of size {}",
                self.inner.size()
            )));
        }
        Ok(self.inner.state_at_str(i))
    }

    /// Return the integer representation of `state_str`.
    fn state_to_int(&self, py: Python<'_>, state_str: &str) -> PyResult<Py<PyAny>> {
        let bytes = parse_state_str(state_str, self.inner.n_sites(), self.inner.lhss())?;
        state_bytes_to_py_int(py, &bytes, self.inner.lhss())
    }

    #[pyo3(signature = (state_int, bracket_notation = true))]
    fn int_to_state(
        &self,
        state_int: &Bound<'_, PyAny>,
        bracket_notation: bool,
    ) -> PyResult<String> {
        let bytes = py_int_to_state_bytes(state_int, self.inner.n_sites(), self.inner.lhss())?;
        Ok(state_bytes_to_state_string(&bytes, bracket_notation))
    }

    /// Return the index of `state_str`, or `None` if absent.
    #[pyo3(name = "index_str")]
    fn index_str(&self, state_str: &str) -> PyResult<Option<usize>> {
        let bytes = parse_state_str(state_str, self.inner.n_sites(), self.inner.lhss())?;
        Ok(self.inner.index_of_bytes(&bytes))
    }

    /// Return the index of an integer-encoded basis state, or `None` if absent.
    ///
    /// `state_int` is the raw integer representation (same convention as the
    /// "integer repr." column in `print(basis)`).
    #[pyo3(name = "index")]
    fn index_int_raw(&self, state_int: &Bound<'_, PyAny>) -> PyResult<Option<usize>> {
        let bytes = py_int_to_state_bytes(state_int, self.inner.n_sites(), self.inner.lhss())?;
        Ok(self.inner.index_of_bytes(&bytes))
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
        let full_basis = GenericBasis::new(
            self.inner.n_sites(),
            self.inner.lhss(),
            SpaceKind::Full,
            false,
        )
        .map_err(Error::from)?;

        let out_rows = self.inner.size();
        let mut out_vec = vec![num_complex::Complex::<f64>::new(0.0, 0.0); out_rows * in_vec.ncols];
        for col in 0..in_vec.ncols {
            let in_col = &in_vec.data[col * in_vec.nrows..(col + 1) * in_vec.nrows];
            let out_col = &mut out_vec[col * out_rows..(col + 1) * out_rows];
            project_between_bases(&full_basis, &self.inner, in_col, out_col, true)
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
        let full_basis = GenericBasis::new(
            self.inner.n_sites(),
            self.inner.lhss(),
            SpaceKind::Full,
            false,
        )
        .map_err(Error::from)?;

        let out_rows = full_basis.size();
        let mut out_vec = vec![num_complex::Complex::<f64>::new(0.0, 0.0); out_rows * in_vec.ncols];
        for col in 0..in_vec.ncols {
            let in_col = &in_vec.data[col * in_vec.nrows..(col + 1) * in_vec.nrows];
            let out_col = &mut out_vec[col * out_rows..(col + 1) * out_rows];
            project_between_bases(&self.inner, &full_basis, in_col, out_col, true)
                .map_err(Error::from)?;
        }

        Ok(state_vec_to_pyarray(py, &out_vec, in_vec.is_complex, out_rows, in_vec.ncols, in_vec.is_matrix))
    }

    fn __str__(&self) -> String {
        format!("{}", self.inner)
    }

    fn __repr__(&self) -> String {
        format!(
            "GenericBasis(n_sites={}, lhss={}, size={}, kind={})",
            self.inner.n_sites(),
            self.inner.lhss(),
            self.inner.size(),
            self.inner.kind(),
        )
    }
}
