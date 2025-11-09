#include<pybind11/pybind11.h>
#include<pybind11/stl.h>
#include"TAC.hpp"
#include"../lexical/lexical.hpp"

namespace py = pybind11;
using namespace pybind11::literals;

PYBIND11_MODULE(TAC,m){
    py::class_<TACGenerator>(m,"TACGenerator")
        .def(py::init<Lexer&>())
        .def("generate",&TACGenerator::generate,"debugMode"_a=true)
        .def("getDebugInfo",&TACGenerator::getDebugInfo)
        .def("getOutputInfo",&TACGenerator::getOutputInfo)
        .def("getTACTable",&TACGenerator::getTACTable);
    
    py::class_<midCodeTable>(m,"midCodeTable")
        .def(py::init<>())
        .def("NXQ",&midCodeTable::NXQ)
        .def("generate",&midCodeTable::generate)
        .def("backpatch",&midCodeTable::backpatch)
        .def("dump",&midCodeTable::dump)
        .def("getTable",&midCodeTable::getTable);
}