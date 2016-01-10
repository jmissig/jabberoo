#ifndef XPATH_H
#define XPATH_H

#include "judo.hpp"

#include <vector>
#include <iostream>

namespace judo
{
    namespace XPath
    {
        /**
         * Represents a result from a XPath query.
         */
        class Value
        {
        public:
            typedef std::list<judo::Element*> ElemList;
            typedef std::list<std::string> ValueList;
            typedef std::map<std::string,std::string> AttribMap;

            Value(judo::Element* elem)
            { 
                _elems.push_back(elem);
                _matched = false;
                _in_context = false;
            }
            
            Value(ElemList& elems) : _elems(elems)
            { 
                _matched = false;
                _in_context = false;
            }

            Value(const std::string value)
            { 
                _values.push_back(value);
                _matched = false;
                _in_context = false;
            }
            
            Value(ValueList& values) : _values(values)
            {
                _matched = false;
                _in_context = false;
            }

            Value()
            {
                _matched = false;
                _in_context = false;
            }

            ~Value()
            {
                #if 0
                for(ElemList::iterator it = _elems.begin(); 
                    it != _elems.end(); ++ it)
                {
                    delete *it;
                }
                #endif
            }
            
            judo::Element* getElem() const { return _elems.front(); }
            ElemList&  getList() { return _elems; }
            void setElems(ElemList& elems) { _elems = elems; }
            void setElems(judo::Element* elem)
            {
                _elems.clear();
                _elems.push_back(elem);
            }

            const std::string getValue() const { return _values.front(); }
            ValueList& getValues() { return _values; }
            void setValues(ValueList& values) { _values = values; }

            const std::string getAttrib(const std::string& name) const
            {
                AttribMap::const_iterator it = _attribs.find(name);
                if (it != _attribs.end())
                    return it->second;
                else
                    return "";
            }
            AttribMap& getAttribs() { return _attribs; }
            void setAttribs(AttribMap& attribs) { _attribs = attribs; }

            void setMatch(const bool matched)
            {
                _matched = matched;
            }

            bool check()
            {
                return _matched;
            }

            bool in_context()
            {
                return _in_context;
            }
            
            bool in_context(bool in_context)
            {
                _in_context = in_context;
                return _in_context;
            }
            
        private:
            ElemList _elems;
            ValueList _values;
            AttribMap _attribs;
            bool _matched;
            bool _in_context;
        };
        
        class Op
        {
        public:
            typedef std::vector<Op*> OpList;
            enum Type
            {
                OP_NODE,
                OP_ALL,
                OP_ATTRIBUTE,
                OP_CONTEXT_CONDITION,
                OP_LITERAL, 
                OP_POSITION,
                OP_EQUAL,
                OP_NOTEQUAL,
                OP_AND,
                OP_OR,
                OP_FUNCTION
            };

            Op(Type op, const std::string& value) :
                _op(op), _value(value)
            { }

            virtual ~Op()
            { }

            virtual std::string getValue()
            {
                return _value;
            }

            virtual std::string calcStr(judo::Element* elem)
            {
                return _value;
            }
            
            virtual bool isValid(XPath::Value* ctxt)
            {
                return true;
            }

            void display()
            {
                std::string type_str;
                switch(_op)
                {
                case OP_NODE:
                    type_str = "NODE";
                    break;
                case OP_ATTRIBUTE:
                    type_str = "ATTRIBUTE";
                    break;
                case OP_CONTEXT_CONDITION:
                    type_str = "CONTEXT_CONDITION";
                    break;
                case OP_LITERAL:
                    type_str = "LITERAL";
                    break;
                case OP_POSITION:
                    type_str = "POSITION";
                    break;
                case OP_EQUAL:
                    type_str = "EQUAL";
                    break;
                case OP_NOTEQUAL:
                    type_str = "NOTEQUAL";
                    break;
                case OP_FUNCTION:
                    type_str = "FUNCTION";
                    break;
                 default:
                    type_str = "UNKNOWN";
                    break;
                };

                std::cout << "OP: type(" << type_str << ") value(" << _value << ")" 
                    << std::endl;
            }

            bool isType(Type type)
            {
                return (_op == type);
            }

            virtual void setArgs(OpList& args)
            {
            }

            virtual bool isClosed()
            {
                return true;
            }
 
            virtual void close()
            {
            }

        protected:
            Type _op;
            std::string _value;
        };

        // Function map
        struct Function
        {
            virtual bool run(Value* ctxt, Op::OpList& args) = 0;
            virtual std::string value(judo::Element* elem, Op::OpList& args) 
            { return std::string(""); };
        };
        typedef std::map<std::string, XPath::Function*> FunctionMap;
        extern FunctionMap functions;
        void init_functions();
        void add_function(const std::string& name, XPath::Function* func);


        /**
        * Represents a single XPath query string.
        */
        class Query
        {
        public:
            typedef std::vector<Op*> OpList;
            Query(const std::string query) : 
                _query(query)
            { 
                _tokens.push_back('/');
                _tokens.push_back('[');
                _tokens.push_back(']');
                _tokens.push_back('@');
                _tokens.push_back('\"');
                _tokens.push_back('\'');
                _tokens.push_back('=');
                _tokens.push_back('!');
                _tokens.push_back('(');
                _tokens.push_back(')');
                _tokens.push_back(':');
                _tokens.push_back(' ');
                _tokens.push_back(',');

                if (XPath::functions.empty())
                    init_functions();
                if (!parseQuery())
                    throw Invalid();
            }

            ~Query()
            {
                while (!_ops.empty())
                {
                    Op *op = _ops.back();
                    _ops.pop_back();
                    delete op;
                }
            }

            /**
            * Check to see if the query would match on the given root.
            * 
            * @param root The element to check against.
            * @return true if the element would match the query.
            */
            bool check(const judo::Element& root);
            /**
            * Execute the query on the given element and return the result set.
            *
            * @param root
            * @return A Value object that can be used to retrieve the result.
            */
            Value* execute(judo::Element* root);

            const OpList& getOps() const
            { return _ops; };

            class Invalid{};
        private:
            std::list<char> _tokens;
            std::string _query;
            OpList _ops;

            bool parseQuery();
            char getNextToken(std::string::size_type& cur);
            std::string getNextIdentifier(std::string::size_type& pos);
            Op* getOp(std::string::size_type& pos, char in_context = 0);
        };
    };
};

#endif // ifndef XPATH_H
