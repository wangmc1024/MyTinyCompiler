#include<pybind11/pybind11.h>
#include<pybind11/stl.h>
#include"AST.hpp"

namespace py = pybind11;
using namespace pybind11::literals;

PYBIND11_MODULE(AST,m){
    py::class_<ASTSemanticAnalyzer>(m,"ASTSemanticAnalyzer")
        .def(py::init<Lexer&>()) 
        .def("analyze",&ASTSemanticAnalyzer::analyze,"debugMode"_a=true)
        .def("getAST",&ASTSemanticAnalyzer::getAST)  
        .def("getDebugInfo",&ASTSemanticAnalyzer::getDebugInfo)
        .def("getOutputInfo",&ASTSemanticAnalyzer::getOutputInfo);
    
    py::class_<node>(m,"node")
        .def(py::init<const std::string&>())
        .def("toString",&node::toString)
        .def_readonly("root",&node::root)
        .def_property_readonly("children",&node::getChildren);

}