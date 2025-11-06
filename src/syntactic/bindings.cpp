#include "syntactic.hpp"
#include<pybind11/pybind11.h>
#include<pybind11/stl.h>
namespace py = pybind11;
using namespace pybind11::literals;


PYBIND11_MODULE(syntactic,m){
    py::class_<Parser>(m, "Parser")
        .def(py::init<Lexer&>())
        .def("parseProgram", &Parser::parseProgram)
        .def("getParseResult", &Parser::getParseResult);
        //.def_property_readonly("lexer", &Parser::getLexer);
    
}
