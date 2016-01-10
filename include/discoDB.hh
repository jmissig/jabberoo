// discoDB.hh
// Jabber client library
//
// Original Code Copyright (C) 1999-2001 Dave Smith (dave@jabber.org)
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// Contributor(s): Julian Missig
//
// This Original Code has been modified by IBM Corporation. Modifications 
// made by IBM described herein are Copyright (c) International Business 
// Machines Corporation, 2002.
//
// Date             Modified by     Description of modification
// 01/20/2002       IBM Corp.       Updated to libjudo 1.1.1
// 2002-03-05       IBM Corp.       Updated to libjudo 1.1.5
// 2002-07-09       IBM Corp.       Added Roster::getSession()
//
// =====================================================================================

#ifndef DISCODB_HH
#define DISCODB_HH

#include <time.h>
#include <cstdio>
#include <cstring>
#include <map>
#include <list>
#include <utility>
#include <set>
#include <algorithm>
#include <functional>
#include <iostream>

#include "jutil.hh"
#include "XPath.h"
#include "XCP.hh"
#include "packet.hh"
#include "message.hh"
#include "presence.hh"

#include <sigc++/object.h>
#include <sigc++/signal.h>
#include <sigc++/slot.h>

#include <string>

#include <jabberoofwd.h>

namespace jabberoo {

     /**
      * DiscoDB.
      * Don't blame me, temas did it.
      */
    EXPORT class DiscoDB : public SigC::Object
    {
    public:
        class Item
        {
        public:
            /// A single identity on an Item
            class Identity
            {
            public:
                Identity(const judo::Element& e);
                Identity(const std::string& category, const std::string& type);
                ~Identity();

                const std::string& getType() const;
                const std::string& getCategory() const;

            private:
                std::string _category;
                std::string _type;
            };

            typedef std::list<std::string> FeatureList;
            typedef std::list<Identity*> IdentityList;
            typedef std::list<DiscoDB::Item*>::iterator iterator;
            typedef std::list<DiscoDB::Item*>::const_iterator const_iterator;


            Item(const std::string& jid);
            ~Item();

            const std::string& getJID() const;
            void setNode(const std::string& val);
            const std::string& getNode() const;
            void setName(const std::string& val);
            const std::string& getName() const;
            const IdentityList& getIdentityList() const;
            const FeatureList& getFeatureList() const;
            bool hasChildren() const;
            iterator begin();
            const_iterator begin() const;
            iterator end();
            const_iterator end() const;
            void appendChild(DiscoDB::Item* item);
            void addFeature(const std::string& feature);
            void addIdentity(const judo::Element& e);
            void addIdentity(const std::string& category, 
                             const std::string& type);

        private:
            std::string _jid;
            std::string _node;
            std::string _name;
            std::list<DiscoDB::Item*> _children;
            FeatureList _features;
            IdentityList _identities;
        };

        typedef std::map<std::string, DiscoDB::Item*>::iterator iterator;
        typedef std::map<std::string, DiscoDB::Item*>::const_iterator const_iterator;
        typedef SigC::Slot1<void, const DiscoDB::Item*> DiscoCallbackFunc;

        class XCP_NotCached : public XCP{};


        DiscoDB(jabberoo::Session& sess);
        ~DiscoDB();

        /** 
        * Search the cache for a JID or throw XCP_NotCached if not found
        *
        * This will only return information for JIDs that have been directly
        * queried.  Some of these JIDs may have children attached to them, but
        * they will not be in the DB until they are authortatively answered for
        * by a direct query.
        */
        DiscoDB::Item& operator[](const std::string& jid);

        /**
        * Force the cache to be updated for the specified item.
        *
        * The item is queried and when complete the callback is fired.
        * @param jid The full jid of the entity to query
        * @param f The callback to fire on completion
        * @param get_items true for disco#items false for disco#info, default fasle
        */
        void cache(const std::string& jid, DiscoCallbackFunc f, 
                   bool get_items = false);
        /**
        * Force the cache to be updated for the specified item.
        *
        * The item is queried and when complete the callback is fired.
        * @param jid The full jid of the entity to query
        * @param node The node on the jid to query
        * @param f The callback to fire on completion
        * @param get_items true for disco#items false for disco#info, default fasle
        */
        void cache(const std::string& jid, const std::string& node,
                   DiscoCallbackFunc f, bool get_items = false);

        /// Cleanup the cache
        void clear();

        iterator begin()
        { return _items.begin(); }
        
        const_iterator begin() const
        { return _items.begin(); }

        iterator end()
        { return _items.end(); }

        const_iterator end() const
        { return _items.end(); }

        void erase(iterator it)
        { delete it->second; _items.erase(it); }

        /**
         * Signal is fired whenever a new item is added to the cache.
         * This is most commonly caught for filtering into special lists.
         */
        SigC::Signal1<void, const DiscoDB::Item&> signal_cache_updated;

    private:
        typedef std::multimap<std::string, DiscoDB::Item*> ItemMap;
        typedef std::multimap<std::string, DiscoCallbackFunc> CallbackMap;

        jabberoo::Session& _session;
        CallbackMap _callbacks;
        ItemMap _items;

        void browseCB(const judo::Element& e);
        void discoInfoCB(const judo::Element& e);
        void discoItemsCB(const judo::Element& e);
        void runCallbacks(const std::string& jid, const DiscoDB::Item* item);
    };
} // namespace jabberoo

#endif  // #ifndef DISCODB_HH
