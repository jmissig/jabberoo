/* jabberoo-component.hh
 * Jabber client library
 *
 * Original Code Copyright (C) 1999-2001 Dave Smith (dave@jabber.org)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Contributor(s): Julian Missig
 *
 * This Original Code has been modified by IBM Corporation. Modifications 
 * made by IBM described herein are Copyright (c) International Business 
 * Machines Corporation, 2002.
 *
 * Date             Modified by     Description of modification
 * 01/20/2002       IBM Corp.       Updated to libjudo 1.1.1
 * 2002-03-05       IBM Corp.       Updated to libjudo 1.1.5
 * 2002-07-09       IBM Corp.       Added Roster::getSession()
 */

#ifndef INCL_JABBEROO_COMPONENT_H
#define INCL_JABBEROO_COMPONENT_H

#include <jabberoofwd.h>

#include <judo.hpp>
#include <sigc++/object.h>
#include <sigc++/signal.h>

#include <string>
#include <list>
#include <map>
#include <XPath.h>
#include <packet.hh>

namespace jabberoo {
    
EXPORT class ComponentSession:
    public judo::ElementStreamEventListener, public judo::ElementStream, public SigC::Object
{
public:
    enum ConnectionState
    {
        csNotConnected,
        csAwaitingAuth,
        csConnected
    };

    ComponentSession();
    virtual ~ComponentSession();

    const std::string& getComponentID() const
    { return _componentID; }

    ConnectionState getState() const
    { return _connState; }

    void connect(const std::string& server, const std::string& componentID, 
            const std::string& password);

    void disconnect();

    judo::XPath::Query* registerXPath(const std::string& query, ElementCallbackFunc f);

    void unregisterXPath(judo::XPath::Query* id);

    ComponentSession& operator>>(const char* buffer) { push(buffer, strlen(buffer)); return *this; } 
    ComponentSession& operator<<(const Packet& p) { evtTransmitXML(p.toString().c_str()); return *this;}
    ComponentSession& operator<<(const char* buffer) { evtTransmitXML(buffer); return *this;}
    virtual void push(const char* data, int datasz);


    SigC::Signal1<void, const char*>        evtTransmitXML;
    SigC::Signal0<void>         evtConnected;
protected:
    virtual void onDocumentStart(judo::Element* t);
    virtual void onElement(judo::Element* t);
    virtual void onCDATA(judo::CDATA* c);
    virtual void onDocumentEnd();

private:
    std::string _server;
    std::string _componentID;
    std::string _password;
    std::string _SessionID;
    ConnectionState _connState;
    std::list<judo::XPath::Query*> _XPaths;
    std::map<judo::XPath::Query*, ElementCallbackFunc> _XPCallbacks;
};

} // namespace jabberoo
#endif //INCL_JABBEROO_COMPONENT_H
