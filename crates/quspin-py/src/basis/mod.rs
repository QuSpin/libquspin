pub mod boson;
pub mod fermion;
pub mod generic;
pub mod spin;
pub mod sym_element;

pub use boson::PyBosonBasis;
pub use fermion::PyFermionBasis;
pub use generic::PyGenericBasis;
pub use spin::PySpinBasis;
pub use sym_element::PySymElement;

// ---------------------------------------------------------------------------
// Shared basis helpers
// ---------------------------------------------------------------------------

use crate::error::Error;
use ndarray::Array2;
use numpy::{Complex64, PyArray1, PyArrayMethods, ToPyArray};
use num_complex::Complex;
use pyo3::prelude::*;
use pyo3::types::PyInt;
use quspin_core::basis::seed::{dit_seed_from_str, seed_from_str};

pub(crate) struct PyStateVector {
    pub data: Vec<Complex<f64>>,
    pub is_complex: bool,
    pub nrows: usize,
    pub ncols: usize,
    pub is_matrix: bool,
}

/// Parse seed strings into byte vectors.
///
/// Each seed must have length `n_sites`. For `lhss == 2` uses binary
/// `seed_from_str`; for `lhss > 2` uses `dit_seed_from_str`.
pub(crate) fn parse_seeds(seeds: &[String], n_sites: usize, lhss: usize) -> PyResult<Vec<Vec<u8>>> {
    seeds
        .iter()
        .map(|s| parse_state_str(s, n_sites, lhss))
        .collect()
}

/// Reject an operator that references a site index `>= n_sites`. Called
/// from the basis builder helpers after the operator has been downcast.
pub(crate) fn validate_op_max_site(op_max_site: usize, n_sites: usize) -> PyResult<()> {
    if op_max_site >= n_sites {
        return Err(pyo3::exceptions::PyValueError::new_err(format!(
            "operator references site {op_max_site} but basis has only \
             {n_sites} sites (max valid index is {})",
            n_sites.saturating_sub(1),
        )));
    }
    Ok(())
}

/// Read `(n_sites, lhss)` from a `SymmetryGroup`-like Python object via
/// attribute access. Used by all four `*Basis.symmetric(group, ...)`
/// constructors so they don't have to take `n_sites` / `lhss` as
/// separate arguments.
pub(crate) fn group_n_sites_lhss(group: &Bound<'_, PyAny>) -> PyResult<(usize, usize)> {
    let n_sites: usize = group.getattr("n_sites")?.extract()?;
    let lhss: usize = group.getattr("lhss")?.extract()?;
    Ok((n_sites, lhss))
}

/// Iterate `(element, character)` pairs from a `SymmetryGroup`-like Python
/// iterable, downcast each tuple, and forward the borrowed [`PySymElement`]
/// and complex character to `f`. Shared by [`replay_group_into_generic`] and
/// [`replay_group_into_bit`].
fn replay_group<F>(group: &Bound<'_, PyAny>, mut f: F) -> PyResult<()>
where
    F: FnMut(&crate::basis::sym_element::PySymElement, Complex<f64>) -> PyResult<()>,
{
    for item in group.try_iter()? {
        let item = item?;
        let tup = item.cast::<pyo3::types::PyTuple>()?;
        let elem_obj = tup.get_item(0)?;
        let elem = elem_obj.cast::<crate::basis::sym_element::PySymElement>()?;
        let chi: Complex<f64> = tup.get_item(1)?.extract()?;
        f(&elem.borrow(), chi)?;
    }
    Ok(())
}

/// Replay each `(element, character)` pair from a `SymmetryGroup`-like
/// Python iterable into a [`GenericBasis`](quspin_core::basis::GenericBasis)
/// via [`PySymElement::add_to_basis`](crate::basis::sym_element::PySymElement::add_to_basis).
/// Used by spin / boson / generic Python wrappers.
pub(crate) fn replay_group_into_generic(
    group: &Bound<'_, PyAny>,
    basis: &mut quspin_core::basis::GenericBasis,
) -> PyResult<()> {
    replay_group(group, |elem, chi| {
        elem.add_to_basis(basis, chi).map_err(Error::from)?;
        Ok(())
    })
}

/// Replay each `(element, character)` pair from a `SymmetryGroup`-like
/// Python iterable into a [`BitBasis`](quspin_core::basis::dispatch::BitBasis)
/// via [`PySymElement::add_to_bit_basis`](crate::basis::sym_element::PySymElement::add_to_bit_basis).
/// Used by the fermion Python wrapper.
pub(crate) fn replay_group_into_bit(
    group: &Bound<'_, PyAny>,
    basis: &mut quspin_core::basis::dispatch::BitBasis,
) -> PyResult<()> {
    replay_group(group, |elem, chi| {
        elem.add_to_bit_basis(basis, chi).map_err(Error::from)?;
        Ok(())
    })
}

fn strip_ket_notation(state_str: &str) -> PyResult<&str> {
    let trimmed = state_str.trim();
    if let Some(without_prefix) = trimmed.strip_prefix('|') {
        if let Some(inner) = without_prefix.strip_suffix('>') {
            return Ok(inner.trim());
        }
        return Err(pyo3::exceptions::PyValueError::new_err(
            "state string must either include both '|' and '>' or neither",
        ));
    }
    match trimmed.strip_suffix('>') {
        Some(_) => Err(pyo3::exceptions::PyValueError::new_err(
            "state string must either include both '|' and '>' or neither",
        )),
        None => Ok(trimmed),
    }
}

fn parse_state_tokens(state_str: &str, n_sites: usize, lhss: usize) -> PyResult<Option<Vec<u8>>> {
    if !state_str.contains(char::is_whitespace) && !state_str.contains(',') {
        return Ok(None);
    }

    let tokens: Vec<&str> = state_str
        .split(|c: char| c.is_whitespace() || c == ',')
        .filter(|tok| !tok.is_empty())
        .collect();

    if tokens.len() != n_sites {
        return Err(pyo3::exceptions::PyValueError::new_err(format!(
            "state string has {0} site values, expected {1}",
            tokens.len(),
            n_sites
        )));
    }

    let mut bytes = Vec::with_capacity(n_sites);
    for (site, tok) in tokens.into_iter().enumerate() {
        let value: usize = tok.parse().map_err(|_| {
            pyo3::exceptions::PyValueError::new_err(format!(
                "invalid site value '{tok}' at site {site}; expected a non-negative integer"
            ))
        })?;
        if value >= lhss {
            return Err(pyo3::exceptions::PyValueError::new_err(format!(
                "invalid site value {value} at site {site} for lhss={lhss}"
            )));
        }
        bytes.push(value as u8);
    }
    Ok(Some(bytes))
}

/// Parse a state string to bytes, handling LHSS=2 (binary) and LHSS>2 (dit).
///
/// Accepts plain strings like `0101`, ket notation like `|0101>`, and for
/// multi-digit per-site occupations also tokenized forms like `|0 10 0 1>` or
/// `0,10,0,1`.
pub(crate) fn parse_state_str(state_str: &str, n_sites: usize, lhss: usize) -> PyResult<Vec<u8>> {
    let state_str = strip_ket_notation(state_str)?;

    if let Some(bytes) = parse_state_tokens(state_str, n_sites, lhss)? {
        return Ok(bytes);
    }

    if lhss == 2 {
        seed_from_str(state_str, n_sites)
            .map_err(Error::from)
            .map_err(PyErr::from)
    } else {
        dit_seed_from_str(state_str, n_sites, lhss)
            .map_err(Error::from)
            .map_err(PyErr::from)
    }
}

pub(crate) fn state_bytes_to_py_int(
    py: Python<'_>,
    bytes: &[u8],
    lhss: usize,
) -> PyResult<Py<PyAny>> {
    let int_type = py.get_type::<PyInt>();
    let mut acc = int_type.call1((0u8,))?;
    let bits_per_site = if lhss == 2 {
        1
    } else {
        usize::BITS as usize - (lhss.saturating_sub(1)).leading_zeros() as usize
    };

    for (site, &value) in bytes.iter().enumerate() {
        if value == 0 {
            continue;
        }
        let shifted = int_type
            .call1((value as usize,))?
            .call_method1("__lshift__", (site * bits_per_site,))?;
        acc = acc.call_method1("__or__", (shifted,))?;
    }

    Ok(acc.unbind())
}

pub(crate) fn py_int_to_state_bytes(
    state_int: &Bound<'_, PyAny>,
    n_sites: usize,
    lhss: usize,
) -> PyResult<Vec<u8>> {
    if !state_int.is_instance_of::<PyInt>() {
        return Err(pyo3::exceptions::PyTypeError::new_err(
            "state_int must be a Python int",
        ));
    }

    let is_negative: bool = state_int.call_method1("__lt__", (0i32,))?.extract()?;
    if is_negative {
        return Err(pyo3::exceptions::PyValueError::new_err(
            "state_int must be non-negative",
        ));
    }

    let mut bytes = Vec::with_capacity(n_sites);
    if lhss == 2 {
        for site in 0..n_sites {
            let bit: u8 = state_int
                .call_method1("__rshift__", (site,))?
                .call_method1("__and__", (1u8,))?
                .extract()?;
            bytes.push(bit);
        }
    } else {
        let bits_per_site =
            usize::BITS as usize - (lhss.saturating_sub(1)).leading_zeros() as usize;
        let mask = (1u64 << bits_per_site) - 1;
        for site in 0..n_sites {
            let shift = site * bits_per_site;
            let val: usize = state_int
                .call_method1("__rshift__", (shift,))?
                .call_method1("__and__", (mask,))?
                .extract()?;
            if val >= lhss {
                return Err(pyo3::exceptions::PyValueError::new_err(format!(
                    "state_int encodes invalid local value {val} at site {site} for lhss={lhss}"
                )));
            }
            bytes.push(val as u8);
        }
    }

    Ok(bytes)
}

pub(crate) fn state_bytes_to_state_string(bytes: &[u8], bracket_notation: bool) -> String {
    let multi_digit = bytes.iter().any(|&v| v >= 10);
    let body = if multi_digit {
        bytes
            .iter()
            .map(|v| v.to_string())
            .collect::<Vec<_>>()
            .join(" ")
    } else {
        bytes
            .iter()
            .map(|&v| char::from_digit(v as u32, 10).unwrap_or('?'))
            .collect()
    };

    if bracket_notation {
        format!("|{body}>")
    } else {
        body
    }
}

pub(crate) fn py_any_to_c64_vec(vec_obj: &Bound<'_, PyAny>) -> PyResult<PyStateVector> {
    if let Ok(arr) = vec_obj.cast::<PyArray1<Complex64>>() {
        let data = unsafe {
            arr.as_array()
                .iter()
                .map(|c| Complex::new(c.re, c.im))
                .collect()
        };
        return Ok(PyStateVector {
            data,
            is_complex: true,
            nrows: arr.len()?,
            ncols: 1,
            is_matrix: false,
        });
    }

    if let Ok(arr) = vec_obj.cast::<PyArray1<f64>>() {
        let data = unsafe {
            arr.as_array()
                .iter()
                .map(|&x| Complex::new(x, 0.0))
                .collect()
        };
        return Ok(PyStateVector {
            data,
            is_complex: false,
            nrows: arr.len()?,
            ncols: 1,
            is_matrix: false,
        });
    }

    if let Ok(arr) = vec_obj.cast::<numpy::PyArray2<Complex64>>() {
        let view = unsafe { arr.as_array() };
        let (nrows, ncols) = view.dim();
        let mut data = Vec::with_capacity(nrows * ncols);
        for col in 0..ncols {
            for row in 0..nrows {
                let c = view[(row, col)];
                data.push(Complex::new(c.re, c.im));
            }
        }
        return Ok(PyStateVector {
            data,
            is_complex: true,
            nrows,
            ncols,
            is_matrix: true,
        });
    }

    if let Ok(arr) = vec_obj.cast::<numpy::PyArray2<f64>>() {
        let view = unsafe { arr.as_array() };
        let (nrows, ncols) = view.dim();
        let mut data = Vec::with_capacity(nrows * ncols);
        for col in 0..ncols {
            for row in 0..nrows {
                data.push(Complex::new(view[(row, col)], 0.0));
            }
        }
        return Ok(PyStateVector {
            data,
            is_complex: false,
            nrows,
            ncols,
            is_matrix: true,
        });
    }

    if let Ok(v) = vec_obj.extract::<Vec<Complex<f64>>>() {
        let is_complex = v.iter().any(|z| z.im != 0.0);
        let nrows = v.len();
        return Ok(PyStateVector {
            data: v,
            is_complex,
            nrows,
            ncols: 1,
            is_matrix: false,
        });
    }

    if let Ok(v) = vec_obj.extract::<Vec<f64>>() {
        let nrows = v.len();
        return Ok(PyStateVector {
            data: v.into_iter().map(|x| Complex::new(x, 0.0)).collect(),
            is_complex: false,
            nrows,
            ncols: 1,
            is_matrix: false,
        });
    }

    if let Ok(rows) = vec_obj.extract::<Vec<Vec<Complex<f64>>>>() {
        let nrows = rows.len();
        let ncols = rows.first().map_or(0, Vec::len);
        if rows.iter().any(|row| row.len() != ncols) {
            return Err(pyo3::exceptions::PyValueError::new_err(
                "2-D state array must be rectangular",
            ));
        }
        let is_complex = rows.iter().flatten().any(|z| z.im != 0.0);
        let mut data = Vec::with_capacity(nrows * ncols);
        for col in 0..ncols {
            for row in &rows {
                data.push(row[col]);
            }
        }
        return Ok(PyStateVector {
            data,
            is_complex,
            nrows,
            ncols,
            is_matrix: true,
        });
    }

    if let Ok(rows) = vec_obj.extract::<Vec<Vec<f64>>>() {
        let nrows = rows.len();
        let ncols = rows.first().map_or(0, Vec::len);
        if rows.iter().any(|row| row.len() != ncols) {
            return Err(pyo3::exceptions::PyValueError::new_err(
                "2-D state array must be rectangular",
            ));
        }
        let mut data = Vec::with_capacity(nrows * ncols);
        for col in 0..ncols {
            for row in &rows {
                data.push(Complex::new(row[col], 0.0));
            }
        }
        return Ok(PyStateVector {
            data,
            is_complex: false,
            nrows,
            ncols,
            is_matrix: true,
        });
    }

    Err(pyo3::exceptions::PyTypeError::new_err(
        "state vector must be a 1-D or 2-D sequence or numpy array of real/complex numbers",
    ))
}

pub(crate) fn c64_vec_to_pyarray<'py>(
    py: Python<'py>,
    data: &[Complex<f64>],
) -> Bound<'py, PyArray1<Complex64>> {
    let out: Vec<Complex64> = data.iter().map(|c| Complex64::new(c.re, c.im)).collect();
    out.to_pyarray(py)
}

pub(crate) fn state_vec_to_pyarray<'py>(
    py: Python<'py>,
    data: &[Complex<f64>],
    prefer_complex: bool,
    nrows: usize,
    ncols: usize,
    is_matrix: bool,
) -> Py<PyAny> {
    let max_abs = data.iter().map(|z| z.norm()).fold(0.0_f64, f64::max);
    let imag_tol = (max_abs * 64.0 * f64::EPSILON).max(1e-14);
    let is_numerically_real = data.iter().all(|z| z.im.abs() <= imag_tol);

    if !prefer_complex && is_numerically_real {
        if is_matrix {
            let out = Array2::from_shape_fn((nrows, ncols), |(row, col)| data[col * nrows + row].re);
            out.to_pyarray(py).unbind().into_any()
        } else {
            let out: Vec<f64> = data.iter().map(|z| z.re).collect();
            out.to_pyarray(py).unbind().into_any()
        }
    } else {
        if is_matrix {
            let out = Array2::from_shape_fn((nrows, ncols), |(row, col)| {
                let z = data[col * nrows + row];
                Complex64::new(z.re, z.im)
            });
            out.to_pyarray(py).unbind().into_any()
        } else {
            c64_vec_to_pyarray(py, data).unbind().into_any()
        }
    }
}
