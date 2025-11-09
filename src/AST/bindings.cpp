#include<pybind11/pybind11.h>
#include<pybind11/stl.h>
#include"AST.hpp"

namespace py = pybind11;
using namespace pybind11::literals;

PYBIND11_MODULE(AST,m){
    py::class_<ASTSemanticAnalyzer>(m,"ASTSemanticAnalyzer")
        .def(py::init<Lexer&>())  // 确保 ASTSemanticAnalyzer 存活时 Lexer 也不会被销毁
        .def("analyze",&ASTSemanticAnalyzer::analyze,"debugMode"_a=true)
        .def("getAST",&ASTSemanticAnalyzer::getAST)  // 修改：返回node对象而不是shared_ptr
        .def("dumpIdentifierTable",&ASTSemanticAnalyzer::dumpIdentifierTable)
        .def("getDebugInfo",&ASTSemanticAnalyzer::getDebugInfo)
        .def("getOutputInfo",&ASTSemanticAnalyzer::getOutputInfo);
    
    py::class_<node>(m,"node")
        .def(py::init<const std::string&>())
        .def("toString",&node::toString)
        .def("saveToTXT",&node::saveToTXT);
}