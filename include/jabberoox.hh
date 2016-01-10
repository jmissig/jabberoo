/* jabberoox.hh
 * Jabber client library extensions
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
 */

#ifndef INCL_JABBEROOX_HH
#define INCL_JABBEROOX_HH

#include <string>
#include <list>
#include <jabberoofwd.h>
#include <sigc++/object.h>
#include <sigc++/signal.h>

namespace jabberoo
{
     EXPORT class Filter
     {
     public:
	  class Action
	  {
	  public:
	       enum Value
	       {
		    Invalid = -1, SetType, ForwardTo, ReplyWith, StoreOffline, Continue
	       };
	       Action(Value v = SetType, const std::string& param = "")
		    : _value(v), _param(param) {}
	       // Accessors
	       bool requires_param()
		    { return Action::ParamReq[_value]; }
	       const std::string& param() const 
		    { return _param; }
	       Value value() const 
		    { return _value; }
	       std::string toXML() const;
	       // Mutators
	       Action& operator<<(Value v) 
		    { _value = v; return *this;}
	       Action& operator<<(const std::string& param) {
		    _param = param; return *this; }
	       // Translator
	       static Value translate(const std::string& s);
	  private:
	       static const bool ParamReq[5];
	       Value  _value;
	       std::string _param;
	  };

	  class Condition
	  {
	  public:
	       enum Value
	       {
		    Invalid = -1, Unavailable, From, MyResourceEquals, SubjectEquals, BodyEquals, ShowEquals, TypeEquals
	       };
	       Condition(Value v = Unavailable, const std::string& param = "")
		    : _value(v), _param(param) {}
	       // Accessors
	       bool  requires_param() 
		    { return Condition::ParamReq[_value]; }
	       const std::string& param() const 
		    { return _param; }
	       Value value() const 
		    { return _value; }
	       std::string toXML() const;
	       // Mutators
	       Condition& operator<<(Value v) 
		    { _value = v; return *this;}
	       Condition& operator<<(const std::string& param) 
		    { _param = param; return *this; }
	       // Translator
	       static Value translate(const std::string& s);
	  private:
	       static const bool ParamReq[7];
	       Value  _value;
	       std::string _param;
	  };

	  typedef std::list<Action> ActionList;
	  typedef std::list<Condition> ConditionList;

     public:
	  // Constructors
	  Filter(const std::string& name);
	  Filter(const judo::Element& rule);
	  Filter(const Filter& f);
 	  bool operator==(const Filter& f) const { return &f == this; }
 	  bool operator==(const Filter& f) { return &f == this; }
	  // XML converter
	  std::string toXML() const;
	  // Accessors
	  ActionList&    Actions()    { return _actions; }
	  ConditionList& Conditions() { return _conditions; }
	  const std::string&  Name() const { return _name; }
	  void           setName(const std::string& newname) { _name = newname; }
     private:
	  ActionList    _actions;
	  ConditionList _conditions;
	  std::string _name;
     };

     EXPORT class FilterList
	  : public std::list<Filter>
     {
     public:
	  FilterList(const judo::Element& query);
	  std::string toXML() const;
     };

     EXPORT class Agent
	  : public std::list<Agent>, public SigC::Object
     {
     public:
	  Agent(const Agent& a);
	  Agent(const std::string& jid, const judo::Element& baseElement, Session& s);
	  ~Agent();
     public:
	  // Events
      SigC::Signal1<void, bool> evtFetchComplete;
	  // Accessors
	  const std::string& JID()         const { return _jid; };
	  const std::string& name()        const { return _name; };
	  const std::string& description() const { return _desc; };
	  const std::string& service()     const { return _service; };
	  const std::string& transport()   const { return _transport; };
	  // Info ops
	  bool isRegisterable() const { return _registerable; };
	  bool isSearchable()   const { return _searchable; };
	  bool isGCCapable()    const { return _gc_capable; };
	  bool hasAgents()      const { return _subagents; };
	  // Action ops
	  void requestRegister(ElementCallbackFunc f);
	  void requestSearch(ElementCallbackFunc f);
	  void fetch();
     protected:
	  void on_fetch(const judo::Element& iq);
     private:
	  std::string   _jid;
	  std::string   _name;
	  std::string   _desc;
	  std::string   _service;
	  std::string   _transport;
	  bool     _registerable;
	  bool     _searchable;
	  bool     _subagents;
	  bool     _gc_capable;
	  bool     _fetchrequested;
	  Session& _session;
     };

     EXPORT class AgentList
	  : public std::list<Agent>
     {
     public:
	  AgentList(const judo::Element& query, Session& s);
	  void load(const judo::Element& query, Session& s);
     };


};

#endif
