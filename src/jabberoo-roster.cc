/* jabberoo-roster.cc
 * Jabber Roster handler
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
 */

#include <roster.hh>
#include <judo.hpp>
#include <session.hh>
#include <JID.hh>
#include <sigc++/object_slot.h>
#include <sigc++/signal.h>

namespace jabberoo {

Roster::Roster(Session& s)
     : _owner(s)
{}

const Roster::Item& Roster::operator[](const std::string& jid) const
{
     ItemMap::const_iterator it = _items.find(filterJID(jid));
     if (it == _items.end())
	  throw XCP_InvalidJID();
     else
	  return it->second;
}

Roster::Item& Roster::operator[](const std::string& jid)
{
     ItemMap::iterator it = _items.find(filterJID(jid));
     if (it == _items.end())
	  throw XCP_InvalidJID();
     else
	  return it->second;
}

void Roster::reset()
{
     _items.clear();
     evtRefresh(); // notify people that the overall Roster has been changed (emptied!)
}

// ---------------------------------------------------------
// Information
// ---------------------------------------------------------
bool Roster::containsJID(const std::string& jid) const
{
     return (_items.find(filterJID(jid)) != _items.end());
}

// ---------------------------------------------------------
// Update ops
// ---------------------------------------------------------
void Roster::update(const judo::Element& t)
{
     bool updateFlag = false;

     // Process each <item> tag and add/update
     // the roster
     judo::Element::const_iterator it = t.begin();
     for (; it != t.end(); ++it )
     {
	  if ((*it)->getType() != judo::Node::ntElement)
	       continue;
	  // Cast item into a tag..
	  judo::Element& item = *static_cast<judo::Element*>(*it);

	  // Extract JID & resource
	  std::string jid = filterJID(item.getAttrib("jid"));

	  // Lookup this jid in the item map
	  ItemMap::iterator rit = _items.find(jid);

	  // If this jid is already in the Item map, update it...
      if (rit != _items.end())
      {
          // If the subscription type = "remove" then, we need
          // to delete this roster item...
          updateFlag = true;
          if (item.cmpAttrib("subscription", "remove"))
          {
              evtRemovingItem(rit->second);
              removeItemFromAllGroups(rit->second);
              _items.erase(rit);
          }
          // Otherwise, update the roster item
          else
          {
              evtUpdating(rit->second);
              rit->second.update(*this, item);
              evtUpdateDone(rit->second);
          }

      }
	  // Otherwise, create a new item on the map
	  else if (!item.cmpAttrib("subscription", "remove"))
	  {
	       _items.insert(make_pair(jid, Item(*this, item)));
	       updateFlag = true;
	  }
     }
     // Notify whoever we need to that the overall roster has been updated
     if (updateFlag)
	  evtRefresh();

}

void Roster::update(const Presence& p, Presence::Type prev_type)
{
     // Locate the presence sender on our map
     std::string jid = filterJID(p.getFrom());
     ItemMap::iterator it = _items.find(jid);
     if (it != _items.end())
     {
          it->second.update(*this, jid, p, prev_type);             // Update the item
     }
}

void Roster::update(const Item& i)
{
     // Compose basic query packet
     judo::Element iq("iq");
     iq.putAttrib("type", "set");
     judo::Element* query = iq.addElement("query");
     query->putAttrib("xmlns", "jabber:iq:roster");
     judo::Element* item = query->addElement("item");

     // Insert item specifics
     item->putAttrib("jid", i.getJID());
     if (!i.getNickname().empty())
	  item->putAttrib("name", i.getNickname());
     // Add all groups in this item
     for (Item::iterator it = i.begin(); it != i.end(); it++)
     {
	  // Don't add the virtual groups
	  if (*it != "Unfiled" && *it != "Pending" && *it != "Agents")
	       item->addElement("group", *it);
     }

     // Transmit item
     _owner << iq.toString().c_str();
}

// ---------------------------------------------------------
// Control ops
// ---------------------------------------------------------
void Roster::deleteUser(const std::string& jid)
{

     // Send a subscription=remove request...
     judo::Element iq("iq");
     iq.putAttrib("type", "set");
     judo::Element* query = iq.addElement("query");
     query->putAttrib("xmlns", "jabber:iq:roster");
     judo::Element* item = query->addElement("item");
     item->putAttrib("jid", jid);
     item->putAttrib("subscription", "remove");

     // Send it
     _owner << iq.toString().c_str();
     _owner << Presence(jid, Presence::ptUnsubRequest);

     // If the user is an Agent then send a remove request
     if ((jid.find("@") == std::string::npos) && (jid.find("/") != std::string::npos))
          _owner.queryNamespace("jabber:iq:register", SigC::slot(*this, &Roster::deleteAgent), jid);
}

void Roster::deleteAgent(const judo::Element& iq)
{
     const judo::Element* query = iq.findElement("query");

     judo::Element niq("iq");
     niq.putAttrib("type", "set");
     niq.putAttrib("to", iq.getAttrib("from"));
     judo::Element* nquery = niq.addElement("query");
     nquery->putAttrib("xmlns", "jabber:iq:register");
     nquery->addElement("key", query->getChildCData("key"));
     nquery->addElement("remove");

     // send iit
     _owner << niq.toString().c_str();
}

void Roster::fetch() const
{
     judo::Element iq("iq");
     iq.putAttrib("type", "get");
     iq.putAttrib("id", _owner.getNextID());
     judo::Element* query = iq.addElement("query");
     query->putAttrib("xmlns", "jabber:iq:roster");

     _owner << iq.toString().c_str();
}

// ---------------------------------------------------------
// S10N translation
// ---------------------------------------------------------
std::string Roster::translateS10N(Subscription stype)
{
     switch (stype)
     {
     case rsNone:   return "none";
     case rsTo:     return "to";
     case rsFrom:   return "from";
     case rsBoth:   return "both";
     case rsRemove: return "remove";
     }
     return "none";
}

Roster::Subscription Roster::translateS10N(const std::string& stype)
{
     if (stype == "to")
	  return rsTo;
     else if (stype == "from")
	  return rsFrom;
     else if (stype == "both")
	  return rsBoth;
     else if (stype == "remove")
	  return rsRemove;
     else
	  return rsNone;
}

std::string Roster::filterJID(const std::string& jid)
{
     // If this jid doesn't have a user, then
     // escape the whole jid and return
     if (jid.find("@") == std::string::npos)
	  return judo::escape(jid);
     // Otherwise, escape and return just user@host
     else
	  return JID::getUserHost(jid);
     
}

void Roster::removeItemFromGroup(const std::string& group, const std::string& jid)
{
     typedef std::map<std::string, std::set<std::string> >::iterator GIT;
     GIT it = _groups.find(group);
     // Lookup this group..if found..
     if (it != _groups.end())
     {
	  // Erase the JID in the group-std::set
         std::set<std::string>::iterator member = it->second.find(jid);
	  if (member != it->second.end()) 
	  {
	       it->second.erase(member);
	  } 
	  // Erase the group-std::set if it's empty n
	  if (it->second.empty())
	       _groups.erase(it);
     }
}

void Roster::removeItemFromAllGroups(const Item& item)
{
     // Remove this item from all groups
     for (Item::iterator it = item.begin(); it != item.end(); it++)
     {
	  removeItemFromGroup(*it, item.getJID());
     }
}

void Roster::mergeItemGroups(const std::string& itemjid, const std::set<std::string>& oldgrp, const std::set<std::string>& newgrp)
{
     // Walk the group list and update Roster::_groups
     // accordingly..icky..slow..wish there was a better
     // way
    std::set<std::string>::const_iterator new_it = newgrp.begin();
     std::set<std::string>::const_iterator old_it = oldgrp.begin();
     while ((new_it != newgrp.end()) || (old_it != oldgrp.end()))
     {
	  if ((old_it == oldgrp.end()) && (new_it != newgrp.end()))
	  {
	       _groups[*new_it].insert(itemjid);
	       ++new_it;
	  }
	  else if ((old_it != oldgrp.end()) && (new_it == newgrp.end()))
	  {
	       removeItemFromGroup(*old_it, itemjid);
	       ++old_it;
	  }
	  else if ((*old_it) == (*new_it))
	  {
	       ++old_it; 
	       ++new_it;
	  }
	  else
	  {
	       removeItemFromGroup(*old_it, itemjid);
	       ++old_it;
	  }
     }

}

// ---------------------------------------------------------
// IMPLEMENTATION
// 
// Roster::Item
// ---------------------------------------------------------
Roster::Item::Item(const judo::Element& t)
     : _rescnt(0)
{
     update(t);
}

Roster::Item::Item(Roster& r, const judo::Element& t)
     : _rescnt(0)
{
     update(r, t);
}

Roster::Item::Item(const std::string& jid, const std::string& nickname)
     : _jid(jid), _nickname(nickname)
{}

Roster::Item::~Item()
{}

void Roster::Item::addToGroup(const std::string& group)
{
     _groups.insert(group);
}

void Roster::Item::delFromGroup(const std::string& group)
{
     _groups.erase(group);
}

void Roster::Item::clearGroups()
{
     _groups.clear();
}

bool Roster::Item::update(const judo::Element& t)
{
     _jid = t.getAttrib("jid");

     // If no nickname is available, use standard user@host
     _nickname = t.getAttrib("name");
     if (_nickname == "")
	  _nickname = _jid;
	  
     // Get subscription
     _type = Roster::translateS10N(t.getAttrib("subscription"));    
     
     // Determine if subscription is pending
     _pending = (t.getAttrib("ask") == "subscribe");

     _groups.clear();
     
     // Parse group std::set from subtags
     judo::Element::const_iterator it = t.begin();
     for (; it != t.end(); ++it)
     {
	  if ((*it)->getType() != judo::Node::ntElement)
	       continue;
	  std::string grp_name = static_cast<judo::Element*>(*it)->getCDATA();
	  if (!grp_name.empty())
	       _groups.insert(grp_name);
     }

     // If the subscription is pending, also display them in Pending virtual group
     if (_pending)
     {
	  _groups.insert("Pending");
     }
     // If they're not in a group, display them in Unfiled virtual group
     else if (_groups.empty())
     {
	  // If this jid has no user, but *does* have a resource,
	  // it must be an agent/transport registration
	  if ((_jid.find("@") == std::string::npos) && (_jid.find("/") != std::string::npos))
	       _groups.insert("Agents");
	  // Otherwise, it should be displayed in Unfiled
	  else
	       _groups.insert("Unfiled");
     }

     return true;
}

bool Roster::Item::update(Roster& r, const judo::Element& t)
{
     // Save old group std::set
     std::set<std::string> oldGroups = _groups;

     update(t);

     // Have the owner merge it's representation of item groups
     // appropriately
     r.mergeItemGroups(_jid, oldGroups, _groups);

     return true;
}

void Roster::Item::update(Roster& r, const std::string& jid, const Presence& p, Presence::Type prev_type)
{
     bool available = r._owner.presenceDB().available(jid);
     
     if ((available == true) && (_rescnt == 0))
     {
	  ++_rescnt;
     }
     else if ((available == false) && (_rescnt == 1))
     {
	  --_rescnt;
     }
     r.evtPresence(jid, (_rescnt != 0), prev_type);
}

bool Roster::Item::isAvailable() const
{
     return (_rescnt > 0);
}

std::string Roster::Item::getNickname() const
{
     return _nickname;
}

void Roster::Item::setNickname(const std::string& nickname)
{
     _nickname = nickname;
}

std::string Roster::Item::getJID() const
{
     return _jid;
}

void Roster::Item::setJID(const std::string& jid)
{
     _jid = jid;
}

Roster::Subscription Roster::Item::getSubsType() const
{
     return _type;
}

bool Roster::Item::isPending() const
{
     return _pending;
}

}
