#include "SpectrumComponent.hpp"
#include <pybind11/pybind11.h>

namespace py = pybind11;

// Bind the SpectrumComponent class to Python using pybind11
PYBIND11_MODULE(spectrum_component, m) {
  py::class_<SpectrumComponent>(m, "SpectrumComponent")
      .def(py::init<>())
      .def("init", &SpectrumComponent::init)
      .def("loss_fit", &SpectrumComponent::loss_fit)
      .def("loss_fit_beta", &SpectrumComponent::loss_fit_beta)
      .def("get_beta_size", &SpectrumComponent::get_beta_size)
      .def("get_beta_init", &SpectrumComponent::get_beta_init)
      .def("get_loss_fit", &SpectrumComponent::get_loss_fit);
}
