#include "JabberOOLink.h"

void sendToSession(Session* s, const Packet& p)
{
    *s << p;
}

void sendToSession(Session* s, const char* buf)
{
    *s << buf;
}

void onExecuteXPath(const Element& elem, object func)
{ func(elem); }

XPath::Query* registerXPath(Session* s, const std::string& query, object func)
{ return s->registerXPath(query, bind(slot(onExecuteXPath), func)); }

void unregisterXPath(Session* s, XPath::Query* id) { s->unregisterXPath(id); }

void exportSession()
{
    scope in_Session(
        class_<Session, bases<ElementStream> >("Session")
            .def("send", (void (*)(Session*, const Packet&))sendToSession)
            .def("send", (void (*)(Session*, const char*))sendToSession)
            .def("connect", &Session::connect)
            .def("disconnect", &Session::disconnect)
            .def("getConnState", &Session::getConnState)
            .def("push", &Session::push)
            .def("registerIQ", &Session::registerIQ)
            .def("registerXPath", registerXPath, rir())
            .def("unregisterXPath", unregisterXPath)
            .def("queryNamespace", &Session::queryNamespace)
            .def("roster", (Roster& (Session::*)())&Session::roster, rir())
            .def("browseDB", (BrowseDB& (Session::*)())
                &Session::browseDB, rir())
            .def("presenceDB", (PresenceDB& (Session::*)()) 
                &Session::presenceDB, rir())
            .def("getAuthType", &Session::getAuthType)
            .def("getUserName", &Session::getUserName, ccr())
            .def("getNextID", &Session::getNextID)
            .def("getDigest", &Session::getDigest)
            // Signals
            .def_readonly("evtXMLParserError", &Session::evtXMLParserError)
            .def_readonly("evtTransmitXML", &Session::evtTransmitXML)
            .def_readonly("evtRecvXML", &Session::evtRecvXML)
            .def_readonly("evtTransmitPacket", &Session::evtTransmitPacket)
            .def_readonly("evtConnected", &Session::evtConnected)
            .def_readonly("evtDisconnected", &Session::evtDisconnected)
            .def_readonly("evtMessage", &Session::evtMessage)
            .def_readonly("evtIQ", &Session::evtIQ)
            .def_readonly("evtMyPresence", &Session::evtMyPresence)
            .def_readonly("evtPresence", &Session::evtPresence)
            .def_readonly("evtPresenceRequest", &Session::evtPresenceRequest)
            .def_readonly("evtUnknownPacket", &Session::evtUnknownPacket)
            .def_readonly("evtAuthError", &Session::evtAuthError)
            .def_readonly("evtOnRoster", &Session::evtOnRoster)
            .def_readonly("evtOnVersion" ,&Session::evtOnVersion)
            .def_readonly("evtOnLast", &Session::evtOnLast)
            .def_readonly("evtOnTime", &Session::evtOnTime)
    );

    enum_<Session::AuthType>("AuthType")
        .value("atPlaintextAuth", Session::atPlaintextAuth)
        .value("atDigestAuth", Session::atDigestAuth)
        .value("atAutoAuth", Session::atAutoAuth)
        .value("at0kAuth", Session::at0kAuth)
    ;

    enum_<Session::ConnectionState>("ConnectionState")
        .value("csNotConnected", Session::csNotConnected)
        .value("csCreateUser", Session::csCreateUser)
        .value("csAuthReq", Session::csAuthReq)
        .value("csAwaitingAuth", Session::csAwaitingAuth)
        .value("csConnected", Session::csConnected)
    ;
}
