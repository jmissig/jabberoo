#include <string>
#include <utility>
#include <iterator>
#include <algorithm>

#include <boost/python.hpp>
#include <boost/python/iterator.hpp>
#include <judo.hpp>
#include <XPath.h>

using namespace judo;
using namespace boost::python;

// Shorten the names
typedef return_value_policy<boost::python::copy_const_reference> ccr;
typedef return_internal_reference<> rir;

struct ESELWrap : public ElementStreamEventListener
{
    ESELWrap(PyObject* self) : _self(self)
    { }

    void onDocumentStart(Element* e) 
    { return call_method<void>(_self, "onDocumentStart", e); }
    void onElement(Element* e) 
    { return call_method<void>(_self, "onElement", e); }
    void onCDATA(CDATA* c) 
    { return call_method<void>(_self, "onCDATA", c); }
    static void default_onCDATA(ElementStreamEventListener* esel, CDATA* c)
    { return esel->ElementStreamEventListener::onCDATA(c); }
    void onDocumentEnd() 
    { return call_method<void>(_self, "onDocumentEnd"); }

    PyObject* _self;
};

Element* cast_to_element(Node* n)
{
    if (n->getType() == judo::Node::ntElement)
        return static_cast<Element*>(n);
}

void test_ESptr(ElementStreamEventListener& el)
{
    std::cout << "Calling onDocumentEnd()" << std::endl;
    el.onDocumentEnd();
}

void exportNode()
{
    scope in_Node(
        class_<Node, boost::noncopyable>("Node", no_init)
                
            .def("getName", &Node::getName, ccr())
            .def("getType", &Node::getType)
    );
    enum_<Node::Type>("Type")
        .value("ntElement", Node::ntElement)
        .value("ntCDATA", Node::ntCDATA)
    ;
}

void exportXPath()
{
//    handle<> xpath_module( PyModule_New("XPath") );
//    scope().attr("XPath") = xpath_module;
//    scope in_XPath(object(xpath_module));

    class_<XPath::Value>("XPathValue")
        .def(init<Element*>())
        .def(init<const std::string>())
        .def("getValue", &XPath::Value::getValue)
        .def("getElem", &XPath::Value::getElem, rir())
        .def("getAttrib", &XPath::Value::getAttrib)
        .def("check", &XPath::Value::check)
    ;

    class_<XPath::Query>("XPathQuery", init<const std::string>())
        .def("check", &XPath::Query::check)
        .def("execute", &XPath::Query::execute, 
                return_value_policy<manage_new_object>())
    ;
}
            
BOOST_PYTHON_MODULE(Judo)
{
    exportNode();

    class_<CDATA, bases<Node> >("CDATA", init<const char*, unsigned int,
            optional<bool> >())
        .def("setText", &CDATA::setText)
        .def("appendText", &CDATA::appendText)
        .def("getText", &CDATA::getText, ccr())
        .def("toString", &CDATA::toString)
    ;

    class_<Element, bases<Node> >("Element", init<const std::string&,
            optional<const char**> >())
        .def("__iter__", iterator<Element, rir>())
        
        .def("addElement", (Element* (Element::*)(const std::string&, const
                        std::string&, bool))&Element::addElement, rir())
        .def("addCDATA", &Element::addCDATA, rir())
        .def("putAttrib", &Element::putAttrib)
        .def("getAttrib", &Element::getAttrib)
        .def("delAttrib", &Element::delAttrib)
        .def("cmpAttrib", &Element::cmpAttrib)
        .def("toString", &Element::toString)
        .def("appendChild", &Element::appendChild,
                with_custodian_and_ward<1,2>())
        .def("empty", &Element::empty)
        .def("size", &Element::size)
        .def("erase", &Element::erase)
        .def("findElement", (Element* (Element::*)(const std::string&))&Element::findElement, rir())
        .def("eraseElement", &Element::eraseElement)
        .def("getChildCData", &Element::getChildCData)
        .def("getChildCDataAsInt", &Element::getChildCDataAsInt)
    ;

    class_<ElementStreamEventListener, boost::noncopyable, boost::shared_ptr<ESELWrap> >(
            "ElementStreamEventListener", no_init)
        .def("onCDATA", &ESELWrap::default_onCDATA)
    ;

    class_<ElementStream>("ElementStream", init<ElementStreamEventListener*>())
        .def("push", &ElementStream::push)
        .def("reset", &ElementStream::reset)
    ;

    exportXPath();

    def("cast_to_element", cast_to_element, rir());
    def("test_ESptr", test_ESptr);
        
                
}
