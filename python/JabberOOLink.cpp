#include "JabberOOLink.h"
#include "jabberoo-component.hh"

using namespace std;

void sendToCSession(ComponentSession* s, const Packet& p)
{
    *s << p;
}

void sendToCSession(ComponentSession* s, const char* buf)
{
    *s << buf;
}

void onCSExecuteXPath(const Element& elem, object func)
{ func(elem); }

XPath::Query* registerCSXPath(ComponentSession* s, const std::string& query, 
        object func)
{ return s->registerXPath(query, bind(slot(onCSExecuteXPath), func)); }

void unregisterCSXPath(ComponentSession* s, XPath::Query* id) { s->unregisterXPath(id); }

void exportComponentSession()
{
    scope in_CS(
        class_<ComponentSession, bases<ElementStream> >("ComponentSession")
            .def("getComponentID", &ComponentSession::getComponentID, ccr())
            .def("getState", &ComponentSession::getState)
            .def("connect", &ComponentSession::connect)
            .def("disconnect", &ComponentSession::disconnect)
            .def("send", (void (*)(ComponentSession*, const Packet&))sendToCSession)
            .def("send", (void (*)(ComponentSession*, const char*))sendToCSession)
            .def("push", &ComponentSession::push)
            .def("registerXPath", registerCSXPath, rir())
            .def("unregisterXPath", unregisterCSXPath)
            .def_readonly("evtTransmitXML", &ComponentSession::evtTransmitXML)
            .def_readonly("evtConnected", &ComponentSession::evtConnected)
    );

    enum_<ComponentSession::ConnectionState>("ConnectionState")
        .value("csNotConnected", ComponentSession::csNotConnected)
        .value("csAwaitingAuth", ComponentSession::csAwaitingAuth)
        .value("csConnected", ComponentSession::csConnected)
    ;
}

void exportMessage()
{
    scope in_Message(
        class_<Message, bases<Packet> >("Message", init<const Element&>())
            .def(init<const std::string&, const std::string&, 
                optional<Message::Type> >())
            .add_property("body", &Message::getBody, &Message::setBody)
            .add_property("subject", &Message::getSubject, &Message::setSubject)
            .add_property("thread", &Message::getThread, &Message::setThread)
            .add_property("type", &Message::getType, &Message::setType)

            .def("requestDelivered", &Message::requestDelivered)
            .def("requestDisplayed", &Message::requestDisplayed)
            .def("requestComposing", &Message::requestComposing)
            .def("getDateTime", &Message::getDateTime)
            .def("replyTo", &Message::replyTo)
            .def("delivered", &Message::delivered)
            .def("displayed", &Message::displayed)
            .def("composing", &Message::composing)
    );

    enum_<Message::Type>("Type")
        .value("mtNormal", Message::mtNormal)
        .value("mtError", Message::mtError)
        .value("mtChat", Message::mtChat)
        .value("mtGroupchat", Message::mtGroupchat)
        .value("mtHeadline", Message::mtHeadline)
    ;
}

void exportPresence()
{
    scope in_Presence(
        class_<Presence, bases<Packet> >("Presence", init<const Element&>())
            .def(init<const std::string&, Presence::Type, 
                optional<Presence::Show, const std::string&, 
                const std::string&> >())
            .add_property("type", &Presence::getType, &Presence::setType)
            .add_property("status", &Presence::getStatus, &Presence::setStatus)
            .add_property("show", &Presence::getShow, &Presence::setShow)
            .add_property("priority", &Presence::getPriority,
                &Presence::setPriority)
            .def("getShow_str", &Presence::getShow_str)
    );

    enum_<Presence::Type>("Type")
        .value("ptSubRequest", Presence::ptSubRequest)
        .value("ptUnsubRequest", Presence::ptUnsubRequest)
        .value("ptSubscribed", Presence::ptSubscribed)
        .value("ptUnsubscribed", Presence::ptUnsubscribed)
        .value("ptAvailable", Presence::ptAvailable)
        .value("ptUnavailable", Presence::ptUnavailable)
        .value("ptError", Presence::ptError)
        .value("ptInvisible", Presence::ptInvisible)
    ;

    enum_<Presence::Show>("Show")
        .value("stInvalid", Presence::stInvalid)
        .value("stOffline", Presence::stOffline)
        .value("stOnline", Presence::stOnline)
        .value("stChat", Presence::stChat)
        .value("stAway", Presence::stAway)
        .value("stXA", Presence::stXA)
        .value("stDND", Presence::stDND)
    ;
}

void exportRoster()
{
    scope in_Roster(
        class_<Roster>("Roster", init<Session&>())
            // Item iterator
            .def("__iter__", boost::python::iterator<Roster, rir >())

            .def("__getitem__", &Roster::operator[], rir())

            .def("getSession", &Roster::getSession, rir())
            .def("containsJID", &Roster::containsJID)
            .def("update", (void (Roster::*)(const Element&))&Roster::update)
            .def("update", (void (Roster::*)(const Presence&, Presence::Type))
                &Roster::update)
            .def("update", (void (Roster::*)(const Roster::Item&)) &Roster::update)
            .def("deleteUser", &Roster::deleteUser)
            .def("fetch", &Roster::fetch)

            // Signals
            .def_readonly("evtRefresh", &Roster::evtRefresh)
            .def_readonly("evtPresence", &Roster::evtPresence)
    );

    class_<Roster::Item>("Item", init<const Element&>())
        // Some more constructors
        .def(init<Roster&, const Element&>())
        .def(init<const std::string&, const std::string&>())

        // Group iterator
        .def("__iter__", boost::python::iterator<Roster::Item>())
        
        .def("addToGroup", &Roster::Item::addToGroup)
        .def("delFromGroup", &Roster::Item::delFromGroup)
        .def("clearGroups", &Roster::Item::clearGroups)
        .def("setNickname", &Roster::Item::setNickname)
        .def("setJID", &Roster::Item::setJID)
        .def("isAvailable", &Roster::Item::isAvailable)
        .def("getNickname", &Roster::Item::getNickname)
        .def("getJID", &Roster::Item::getJID)
        .def("getSubsType", &Roster::Item::getSubsType)
        .def("isPending", &Roster::Item::isPending)
    ;

    enum_<Roster::Subscription>("Subscription")
        .value("rsNone", Roster::rsNone)
        .value("rsTo", Roster::rsTo)
        .value("rsFrom", Roster::rsFrom)
        .value("rsBoth", Roster::rsBoth)
        .value("rsRemove", Roster::rsRemove)
    ;
}

void translateXCP_NotCached(const BrowseDB::XCP_NotCached& e)
{
    PyErr_SetString(PyExc_IndexError, "JID Not cached");
}

BrowseDB::Item& BDB_getitem(BrowseDB* db, const std::string& jid)
{
    try
    {
        return (*db)[jid];
    }
    catch (...)
    {
        throw BrowseDB::XCP_NotCached();
    }
}

void Signal1_v_bdbi_execute(const BrowseDB::Item& t1, object func) { func(t1); }
Connection Signal1_v_bdbi_connect(Signal1<void, const BrowseDB::Item&>* sig,
    object func)
{ return sig->connect(bind(slot(Signal1_v_bdbi_execute), func)); }
void Signal1_v_bdbi_call(Signal1<void, const BrowseDB::Item&>* sig, 
    const BrowseDB::Item& item)
{ (*sig)(item); }

void BDB_executeCB(const BrowseDB::Item& item, object func) { func(item); }
void BDB_cache_cb(BrowseDB* bdb, const std::string& jid, object func)
{ return bdb->cache(jid, bind(slot(BDB_executeCB), func)); }

void exportBrowseDB()
{
    register_exception_translator<BrowseDB::XCP_NotCached>(
        &translateXCP_NotCached);

    class_< Signal1<void, const BrowseDB::Item&> >("Signal1_v_bdbi")
        .def("__call__", &Signal1_v_bdbi_call)
        .def("connect", &Signal1_v_bdbi_connect)
    ;
    
    scope in_bdb(
        class_<BrowseDB>("BrowseDB", init<Session&>())
            .def("__getitem__", BDB_getitem, rir())
            .def("cache", BDB_cache_cb)
            .def("clear", &BrowseDB::clear)
    );

    class_<BrowseDB::Item>("Item", init<const Element&>())
        .def("__iter__", boost::python::iterator<BrowseDB::Item, rir>())

        .def("getType", &BrowseDB::Item::getType, ccr())
        .def("getVersion", &BrowseDB::Item::getVersion, ccr())
        .def("getName", &BrowseDB::Item::getName, ccr())
        .def("getCategory", &BrowseDB::Item::getCategory, ccr())
        .def("getJID", &BrowseDB::Item::getJID, ccr())
        .def("getNamespaceList", &BrowseDB::Item::getNamespaceList, rir())
    ;

    class_< std::list<std::string> >("BrowseDB_Item_ns_list")
        .def("__iter__", boost::python::iterator< std::list< std::string> >())
    ;

}
            
typedef std::list<Presence> pres_list;

Presence PresDB_find(PresenceDB* db, const std::string& jid)
{
    try
    {
        PresenceDB::const_iterator it = db->find(jid);
        return *it;
    }
    catch (...)
    {
        throw PresenceDB::XCP_InvalidJID();
    }
}

pres_list PresDB_equal_range(PresenceDB* db, const std::string& jid)
{
    pres_list ret;
    PresenceDB::range rng = db->equal_range(jid);

    for (PresenceDB::const_iterator it = rng.first; it != rng.second; ++it)
    {
        ret.push_back(*it);
    }

    return ret;
}

void translateXCP_InvalidJID(const PresenceDB::XCP_InvalidJID& e)
{
    PyErr_SetString(PyExc_IndexError, "Invalid JID");
}

void exportPresenceDB()
{
    register_exception_translator<PresenceDB::XCP_InvalidJID>(
            &translateXCP_InvalidJID);

    scope in_PresDB(
        class_<PresenceDB>("PresenceDB", init<Session&>())
            .def("insert", &PresenceDB::insert)
            .def("remove", &PresenceDB::remove)
            .def("clear", &PresenceDB::clear)
            .def("find", PresDB_find)
            .def("equal_range", PresDB_equal_range)
            .def("findExact", &PresenceDB::findExact)
            .def("contains", &PresenceDB::contains)
            .def("available", &PresenceDB::available)
    );

    class_<pres_list>("PresDB_pres_list")
        .def("__iter__", boost::python::iterator<pres_list>())
    ;
}

BOOST_PYTHON_MODULE(JabberOO)
{
    exportSigC();

    class_<Packet>("Packet", init<const std::string&>())
        // Extra constructor
        .def(init<Element&>())
        // Our main properties
        .add_property("sender", &Packet::getFrom, &Packet::setFrom)
        .add_property("to", &Packet::getTo, &Packet::setTo)
        .add_property("id", &Packet::getID, &Packet::setID)

        // Everything else
        .def("getError", &Packet::getError)
        .def("getErrorCode", &Packet::getErrorCode)
        .def("toString", &Packet::toString)
        .def("getBaseElement", (Element& (Packet::*)())&Packet::getBaseElement, rir())
        .def("addX", (Element* (Packet::*)())&Packet::addX, rir())
        .def("addX", (Element* (Packet::*)(const std::string&))&Packet::addX, 
                rir())
        .def("findX", &Packet::findX, rir())
        .def("eraseX", &Packet::eraseX)
    ;

    exportMessage();
    exportPresence();
    exportBrowseDB();
    exportPresenceDB();
    exportRoster();
    exportSession();
    exportComponentSession();

    class_<JID>("JID")
        .def("getResource", &JID::getResource)
        .staticmethod("getResource")
        .def("getUserHost", &JID::getUserHost)
        .staticmethod("getUserHost")
        .def("getHost", &JID::getHost)
        .staticmethod("getHost")
        .def("getUser", &JID::getUser)
        .staticmethod("getUser")
        .def("isValidUser", &JID::isValidUser)
        .staticmethod("isValidUser")
        .def("isValidHost", &JID::isValidHost)
        .staticmethod("isValidHost")
        .def("compare", &JID::compare)
        .staticmethod("compare")
    ;

}
