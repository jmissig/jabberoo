#include <sigc++/slot.h>
#include <sigc++/signal.h>
#include <sigc++/bind.h>
#include <sigc++/connection.h>

#include "JabberOOLink.h"

using namespace SigC;

// Signal0<void>
void Signal0_v_execute(object func) { func(); }
Connection Signal0_v_connect(Signal0<void>* sig, object func) 
{ return sig->connect(bind(slot(Signal0_v_execute), func)); }
void Signal0_v_call(Signal0<void>* sig) { (*sig)(); }

// Signal3<void, const std::string&, bool, Present::Type>
void Signal3_v_cssr_b_PT_execute(const std::string& t1, bool t2, Presence::Type
        t3, object func)
{ func(t1, t2, t3); }
Connection Signal3_v_cssr_b_PT_connect(Signal3<void, const std::string&, bool,
        Presence::Type>* sig, object func)
{ return sig->connect(bind(slot(Signal3_v_cssr_b_PT_execute), func)); }
void Signal3_v_cssr_b_PT_call(Signal3<void, const std::string&, bool,
        Presence::Type>* sig, const std::string& t1, bool t2, 
        Presence::Type t3) 
{ (*sig)(t1, t2, t3); }

// Signal2<void, int, const std::string&>
void Signal2_v_int_cssr_execute(int t1, const std::string& t2, object func)
{ func(t1, t2); }
Connection Signal2_v_int_cssr_connect(
        Signal2<void, int, const std::string&>* sig, object func)
{ return sig->connect(bind(slot(Signal2_v_int_cssr_execute), func)); }
void Signal2_v_int_cssr_call(Signal2<void, int, const std::string&>* sig, int
        t1, const std::string& t2)
{ (*sig)(t1, t2); }

// Signal1<void, const char*>
void Signal1_v_ccp_execute(const char* t1, object func)
{ func(t1); }
Connection Signal1_v_ccp_connect(Signal1<void, const char*>* sig,
       object func)
{ return sig->connect(bind(slot(Signal1_v_ccp_execute), func)); }
void Signal1_v_ccp_call(Signal1<void, const char*>* sig,const char* t1)
{ (*sig)(t1); }

// Signal1<void, int>
void Signal1_v_i_execute(int t1, object func)
{ func(t1); }
Connection Signal1_v_i_connect(Signal1<void, int>* sig,
       object func)
{ return sig->connect(bind(slot(Signal1_v_i_execute), func)); }
void Signal1_v_i_call(Signal1<void, int>* sig, int t1)
{ (*sig)(t1); }

// Signal1<void, const Packet&>
void Signal1_v_cpr_execute(const Packet& t1, object func)
{ func(boost::cref(t1)); }
Connection Signal1_v_cpr_connect(Signal1<void, const Packet&>* sig, 
        object func)
{ return sig->connect(bind(slot(Signal1_v_cpr_execute), func)); }
void Signal1_v_cpr_call(Signal1<void, const Packet&>* sig, const Packet& t1)
{ (*sig)(t1); }

// Signal1<void, const Element&>
void Signal1_v_cer_execute(const Element& t1, object func)
{ func(boost::cref(t1)); }
Connection Signal1_v_cer_connect(Signal1<void, const Element&>* sig, 
        object func)
{ return sig->connect(bind(slot(Signal1_v_cer_execute), func)); }
void Signal1_v_cer_call(Signal1<void, const Element&>* sig, const Element& t1)
{ (*sig)(t1); }

// Signal1<void, const Message&>
void Signal1_v_cmr_execute(const Message& t1, object func)
{ func(boost::cref(t1)); }
Connection Signal1_v_cmr_connect(Signal1<void, const Message&>* sig, 
        object func)
{ return sig->connect(bind(slot(Signal1_v_cmr_execute), func)); }
void Signal1_v_cmr_call(Signal1<void, const Message&>* sig, const Message& t1)
{ (*sig)(t1); }

// Signal1<void, const Presence&>
void Signal1_v_cpresr_execute(const Presence& t1, object func)
{ func(boost::cref(t1)); }
Connection Signal1_v_cpresr_connect(Signal1<void, const Presence&>* sig, 
        object func)
{ return sig->connect(bind(slot(Signal1_v_cpresr_execute), func)); }
void Signal1_v_cpresr_call(Signal1<void, const Presence&>* sig, const Presence&
        t1)
{ (*sig)(t1); }

// Signal2<void, const Presence&, Presence::Type>
void Signal2_v_cpresr_pt_execute(const Presence& t1, Presence::Type t2, 
        object func)
{ func(boost::cref(t1), t2); }
Connection Signal2_v_cpresr_pt_connect(Signal2<void, const Presence&,
        Presence::Type>* sig, object func)
{ return sig->connect(bind(slot(Signal2_v_cpresr_pt_execute), func)); }
void Signal2_v_cpresr_pt_call(Signal2<void, const Presence&,
        Presence::Type>* sig, const Presence& t1, Presence::Type t2)
{ (*sig)(t1, t2); }

// Signal2<void, int, const char*>
void Signal2_v_i_ccp_execute(int t1, const char* t2, object func)
{ func(t1, t2); }
Connection Signal2_v_i_ccp_connect(Signal2<void, int, const char*>* sig, 
        object func)
{ return sig->connect(bind(slot(Signal2_v_i_ccp_execute), func)); }
void Signal2_v_i_ccp_call(Signal2<void, int, const char*>* sig, int t1, const
        char* t2)
{ (*sig)(t1, t2); }

// Signal3<void, std::string&, std::string&, std::string&>
void Signal3_v_ssr_ssr_ssr_execute(std::string& t1, std::string& t2,
        std::string& t3, object func)
{ func(boost::ref(t1), boost::ref(t2), boost::ref(t3)); }
Connection Signal3_v_ssr_ssr_ssr_connect(Signal3<void, std::string&,
        std::string&, std::string&>* sig, object func)
{ return sig->connect(bind(slot(Signal3_v_ssr_ssr_ssr_execute), func)); }
void Signal3_v_ssr_ssr_ssr_call(Signal3<void, std::string&,
        std::string&, std::string&>* sig, std::string& t1, std::string& t2,
        std::string& t3)
{ (*sig)(t1, t2, t3); }

// Signal1<void, std::string&>
void Signal1_v_ssr_execute(std::string& t1, object func)
{ func(boost::ref(t1)); }
Connection Signal1_v_ssr_connect(Signal1<void, std::string&>* s, object func)
{ return s->connect(bind(slot(Signal1_v_ssr_execute), func)); }
void Signal1_v_ssr_call(Signal1<void, std::string&>* sig, std::string& t1)
{ (*sig)(t1); }

// Signal2<void, std::string&, std::string&>
void Signal2_v_ssr_ssr_execute(std::string& t1, std::string& t2, object func)
{ func(boost::ref(t1), boost::ref(t2)); }
Connection Signal2_v_ssr_ssr_connect(
        Signal2<void, std::string&, std::string&>* sig, object func)
{ return sig->connect(bind(slot(Signal2_v_ssr_ssr_execute), func)); }
void Signal2_v_ssr_ssr_call(
        Signal2<void, std::string&, std::string&>* sig, std::string& t1,
        std::string& t2)
{ (*sig)(t1, t2); }

// Signal2<void, const char*, int>
void Signal2_v_ccp_i_execute(const char* t1, int t2, object func)
{ func(t1, t2); }
Connection Signal2_v_ccp_i_connect(Signal2<void, const char*, int>* sig, 
        object func)
{ return sig->connect(bind(slot(Signal2_v_ccp_i_execute), func)); }
void Signal2_v_ccp_i_call(Signal2<void, const char*, int>* sig, const char* t1,
        int t2)
{ (*sig)(t1, t2); }

void exportSigC()
{
    class_<SigC::Connection>("Connection", no_init)
        .def("connected", &SigC::Connection::connected)
        .def("disconnect", &SigC::Connection::disconnect)
    ;

    // Signal0<void>
    class_< Signal0<void> >("Signal0_void")
        .def("__call__", &Signal0_v_call)
        .def("connect", &Signal0_v_connect);

    // Signal3<void, const std::string&, bool, Present::Type>
    class_< Signal3<void, const std::string&, bool, Presence::Type> >(
            "Signal3_v_cssr_b_PT")
        .def("__call__", &Signal3_v_cssr_b_PT_call)
        .def("connect", &Signal3_v_cssr_b_PT_connect);

    // Signal2<void, int, const std::string&>
    class_< Signal2<void, int, const std::string&> >("Signal2_v_int_cssr")
        .def("__call__", &Signal2_v_int_cssr_call)
        .def("connect", &Signal2_v_int_cssr_connect);

    // Signal1<void, const char*>
    class_< Signal1<void, const char*> >("Signal1_v_ccp")
        .def("__call__", &Signal1_v_ccp_call)
        .def("connect", &Signal1_v_ccp_connect);

    // Signal1<void, int>
    class_< Signal1<void, int> >("Signal1_v_i")
        .def("__call__", &Signal1_v_i_call)
        .def("connect", &Signal1_v_i_connect);

    // Signal1<void, const Packet&>
    class_< Signal1<void, const Packet&> >("Signal1_v_cpr")
        .def("__call__", &Signal1_v_cpr_call)
        .def("connect", &Signal1_v_cpr_connect);

    // Signal1<void, const Element&>
    class_< Signal1<void, const Element&> >("Signal1_v_cer")
        .def("__call__", &Signal1_v_cer_call)
        .def("connect", &Signal1_v_cer_connect);

    // Signal1<void, const Message&>
    class_< Signal1<void, const Message&> >("Signal1_v_cmr")
        .def("__call__", &Signal1_v_cmr_call)
        .def("connect", &Signal1_v_cmr_connect);

    // Signal1<void, const Presence&>
    class_< Signal1<void, const Presence&> >("Signal1_v_cpresr")
        .def("__call__", &Signal1_v_cpresr_call)
        .def("connect", &Signal1_v_cpresr_connect);

    // Signal2<void, const Presence&, Presence::Type>
    class_< Signal2<void, const Presence&, Presence::Type> >(
            "Signal2_v_cpresr_pt")
        .def("__call__", &Signal2_v_cpresr_pt_call)
        .def("connect", &Signal2_v_cpresr_pt_connect);

    // Signal2<void, int, const char*>
    class_< Signal2<void, int, const char*> >("Signal2_v_i_ccp")
        .def("__call__", &Signal2_v_i_ccp_call)
        .def("connect", &Signal2_v_i_ccp_connect);
    
    // Signal3<void, std::string&, std::string&, std::string&>
    class_< Signal3<void, std::string&, std::string&, std::string&> >(
            "Signal3_v_ssr_ssr_ssr")
        .def("__call__", &Signal3_v_ssr_ssr_ssr_call)
        .def("connect", &Signal3_v_ssr_ssr_ssr_connect);

    // Signal1<void, std::string&>
    class_< Signal1<void, std::string&> >("Signal1_v_ssr")
        .def("__call__", &Signal1_v_ssr_call)
        .def("connect", &Signal1_v_ssr_connect);
    
    // Signal2<void, std::string&, std::string&>
    class_< Signal2<void, std::string&, std::string&> >("Signal2_v_ssr_ssr")
        .def("__call__", &Signal2_v_ssr_ssr_call)
        .def("connect", &Signal2_v_ssr_ssr_connect);

    // Signal2<void, const char*, int>
    class_< Signal2<void, const char*, int> >("Signal2_v_ccp_i")
        .def("__call__", &Signal2_v_ccp_i_call)
        .def("connect", &Signal2_v_ccp_i_connect);
}
