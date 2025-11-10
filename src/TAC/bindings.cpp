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
        
    py::class_<triple>(m,"triple")
        .def(py::init<>())
        .def(py::init<const std::string&>())
        .def(py::init<const std::string&,const std::string&,const std::string&>())
        .def_readwrite("name",&triple::name)
        .def_readwrite("type",&triple::type)
        .def_readwrite("value",&triple::value)
        .def("getValueAsINT32",&triple::getValueAsINT32)
        .def("getValueAsBOOL",&triple::getValueAsBOOL)
        .def("toString",&triple::toString);
        
    py::class_<identifierTable>(m,"identifierTable")
        .def(py::init<>())
        .def("getIdentifierByName",&identifierTable::getIdentifierByName,py::return_value_policy::reference)
        .def("identifierExists",&identifierTable::identifierExists)
        .def("getTypeByName",&identifierTable::getTypeByName)
        .def("Add",&identifierTable::Add)
        .def("UpdateTypeByName",&identifierTable::UpdateTypeByName)
        .def("UpdateValueByName",&identifierTable::UpdateValueByName)
        .def("dump",&identifierTable::dump)
        .def("getTable",&identifierTable::getTable);
        
    py::class_<tempVariableTable>(m,"tempVariableTable")
        .def(py::init<>())
        .def("createNewVariable",&tempVariableTable::createNewVariable)
        .def("getTable",&tempVariableTable::getTable);
        
    py::class_<TAC>(m,"TAC")
        .def(py::init<const std::string&,const std::string&,const std::string&,const std::string&>())
        .def_readwrite("op",&TAC::op)
        .def_readwrite("oprand1",&TAC::oprand1)
        .def_readwrite("oprand2",&TAC::oprand2)
        .def_readwrite("result",&TAC::result)
        .def("toString",&TAC::toString);
}