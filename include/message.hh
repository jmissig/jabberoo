// message.hh
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

#ifndef MESSAGE_HH
#define MESSAGE_HH

#include <string>
#include <time.h>
#include "packet.hh"

namespace jabberoo
{
     /**
      * A Jabber Message.
      * This class implements most of the methods needed to deal with a Jabber Message.
      * @see jabberoo::Packet
      */
     EXPORT class Message
	  : public Packet
	  {
	  public:
	       // Make sure numTypes is updated in jabberoo-message.cc if you add
	       // a new Message Type
	       /**
		* Number of message types.
		*/
	       static const unsigned int numTypes;

	       /**
		* Message type.
		* This is a hint to clients as to how the message should be displayed.
		*/
	       enum Type {
		    mtNormal,    /**< A normal message (email, icq). */
		    mtError,     /**< An error message. */
		    mtChat,      /**< A chat message (AIM, IRC /msg's). */
		    mtGroupchat, /**< A groupchat message (IRC). */
		    mtHeadline   /**< A headline message (stock ticker, news ticker). */
	       };

	       /**
		* Construct a message based upon the given judo::Element.
		* @see judo::Element
		* @param t A judo::Element which should have a message base element
		*/
	       Message(const judo::Element& t);

	       /**
		* Construct a message to a JabberID with a body.
		* Note that setting a body here is oh-so-slightly faster than calling
		* setBody(), since setBody() checks for a pre-existing body element.
		* @see Message::Type
		* @param jid The string of a JabberID to which this message is addressed.
		* @param body The string body of a message. Can be NULL.
		* @param mtype The Message::Type of the message. Defaults to mtNormal.
		*/
	       Message(const std::string& jid, const std::string& body, Type mtype = mtNormal);

	       // Modifiers
	       /**
		* Sets the body of the message.
		* @param body The string body of a message.
		*/
	       void  setBody(const std::string& body);

	       /**
		* Sets the subject of the message.
		* @param subject The string subject of the message.
		*/
	       void  setSubject(const std::string& subject);

	       /**
		* Sets the message thread.
		* The message thread is used by clients to relate messages to one another.
		* @param thread A string representing the thread, there is no rule as to how it should be generated.
		*/
	       void  setThread(const std::string& thread);
	       
	       /**
		* Sets the message type
		* @param mtype The Message::Type of the message. Defaults to mtNormal.
		*/
	       void  setType(Message::Type mtype);

	       /**
		* Request the delivered message event.
		* If the receiving client supports message events, a message with a message:x:event containing
		* a delivered element will be returned once this message has been delivered.
		*/
	       void requestDelivered();

	       /**
		* Request the displayed message event.
		* If the receiving client supports message events, a message with a message:x:event containing
		* a displayed element will be returned once this message has been displayed.
		*/
	       void requestDisplayed();

	       /**
		* Request the composing message event.
		* If the receiving client supports message events, a message with a message:x:event containing
		* a composing element will be returned when the receiving user begins composing a reply.
		*/
	       void requestComposing();

	       // Accessors
	       /**
		* Get the message body.
		* @return A string containing the message body.
		*/
	       const std::string getBody()     const;
	       /**
		* Get the message subject.
		* @return A string containing the message subject.
		*/
	       const std::string getSubject()  const;
	       /**
		* Get the message thread.
		* @return A string containing the message thread.
		*/
	       const std::string getThread()   const;
	       /**
		* Get the Message::Type.
		* @see Message::Type
		*/
	       Type         getType()     const;
	       /**
		* Get the date and time the message was sent with a given format.
		* @param format The format of the date and time.
		*/
	       const std::string getDateTime(const std::string& format = "") const;

	       // Factory methods
	       /**
		* Create a message in response to this message.
		* @param body The body of the reply message.
		* @return The new message.
		*/
	       Message replyTo(const std::string& body) const;

	       /**
		* Create a message event stating that this Message has been delivered.
		* It is up to the client to determine whether this message should actually
		* be generated and sent.
		* @return The delivered message event which should be sent.
		*/
	       Message Message::delivered() const;

	       /**
		* Create a message event stating that this Message has been displayed.
		* It is up to the client to determine whether this message should actually
		* be generated and sent.
		* @return The displayed message event which should be sent.
		*/	       
	       Message Message::displayed() const;

	       /**
		* Create a message event stating that this Message is being replied to.
		* It is up to the client to determine whether this message should actually
		* be generated and sent.
		* @return The composing message event which should be sent.
		*/
	       Message Message::composing() const;


           /**
           * Get the timestamp of the message.
           */
           time_t get_timestamp() const
           { return _timestamp; }
	       // Static class methods
	       /**
		* Sets the date and time format.AM_CONDITIONAL(DEBUG, test x$debug = xyes)
test=no
AM_CONDITIONAL(TEST, test x$test = xyes)
		* @param format A string date and time format.
		*/
	       static void setDateTimeFormat(const std::string& format);
	       /**
		* Get the date and time format.
		*/
	       static const std::string& getDateTimeFormat();
	  protected:
	       // Specialized reply-to constructor
	       Message(const Message& m, const std::string& body);

	       static std::string translateType(Type mtype);
	       static Type   translateType(const std::string& mtype);
	  private:
	       Type  _type;
	       time_t _timestamp;
	       static std::string _dtFormat;
	  };

} // namespace jabberoo

#endif // MESSAGE_HH
