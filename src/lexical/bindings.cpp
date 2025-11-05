#include"lexical.hpp"
#include<pybind11/pybind11.h>
#include<pybind11/stl.h>
namespace py = pybind11;
using namespace pybind11::literals;

PYBIND11_MODULE(lexical,m){
    py::class_<Lexer>(m,"Lexer")
        .def(py::init<std::string>())
        .def("nextInput",&Lexer::nextInput)
        .def("getDebugInfo",&Lexer::getDebugInfo)
        .def("clearDebugInfo",&Lexer::clearDebugInfo);
    
    py::class_<Token>(m,"Token")
        .def(py::init<std::string,std::string>())
        .def_readwrite("type",&Token::type)
        .def_readwrite("value",&Token::value);
}