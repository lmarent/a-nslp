/*
 * A A-NSLP RESPONSE Message.
 *
 * $Id: anslp_response.h 2015-09-01  $
 * $HeadURL: https://./include/msg/anslp_response.h $
 */
#ifndef ANSLP_RESPONSE_H
#define ANSLP_RESPONSE_H

#include "ie.h"
#include "anslp_msg.h"
#include "anslp_ipap_message.h"


namespace anslp {
  namespace msg {


/**
 * \class anslp_response
 *
 * \brief This class implements a ANSLP RESPONSE Message.
 * 
 * \author Andres Marentes
 *
 * \version 0.1 
 *
 * \date 2015/09/01 08:55:00
 *
 * Contact: la.marentes455@uniandes.edu.co
 *  
 */
class anslp_response : public anslp_msg {

  public:
	
	static const uint16 MSG_TYPE = 0x2;			///< Number assigned to the response message

	/**
	 * Constructor for the class
	 */
	explicit anslp_response();
	
	/**
	 * Constructor copying the definition of other message
	 */
	explicit anslp_response(const anslp_response &other);
	
	/**
	 * Class destructor 
	 */
	virtual ~anslp_response();

	/*
	 * Inherited methods:
	 */
	/**
	 * Return a pointer for a new instance of the class
	 */
	anslp_response *new_instance() const;
	
	/**
	 * Return a pointer to the copy of a response message.
	 */
	anslp_response *copy() const;
	
	void register_ie(IEManager *iem) const;
	

	virtual void serialize(NetMsg &msg, coding_t coding,
		uint32 &bytes_written) const throw (IEError);
	
	virtual bool check() const;		
	
	/** 
	 * Set the session life time of the message
	 */
	void set_session_lifetime(uint32 seconds);
	
	/** 
	 * Get the session life time of the message
	 */
	uint32 get_session_lifetime() const;

	/** 
	 * Set the message sequence number for the message
	 */
	void set_msg_sequence_number(uint32 msn);
	
	/** 
	 * Get the message sequence number for the message
	 */
	uint32 get_msg_sequence_number(void) const;

	/** 
	 * Set the message information code for the message
	 */
	void set_information_code(uint8 severity, uint8 response_code,
							  uint16 object_type = 0);
							  
	/** 
	 * Get the severity class of the message
	 */	
	uint8 get_severity_class() const;
	
	/**
	 * Get the response code
	 */
	 uint8 get_response_code() const;
	 
	/**
	 * Get the response object type
	 */
	uint16 get_response_object_type() const; 
	
	/** 
	 * Get the object type of the message
	 */	
	uint16 get_object_type() const;
	

	/** 
	 * Get whether the message was sucessfull or not
	 */	
	bool is_success() const;

	/** 
	 * Determine whether this message is response to the create message 
	 * given as parameter.
	 */	
	bool is_response_to(const anslp_create *msg) const;
	
	/** 
	 * Determine whether this message is response to the refresh message 
	 * given as parameter.
	 */	
	bool is_response_to(const anslp_refresh *msg) const;

	/** 
	 * Insert an mspec oject message to the response message
	 */ 
	void set_mspec_object(anslp_mspec_object *message);
	
	void get_mspec_objects(vector<anslp_mspec_object *> &list_return);

  protected:
  
	uint32 serialize_object(ie_object_key &key, NetMsg &msg, coding_t coding) const;


};


  } // namespace msg
} // namespace anslp

#endif // ANSLP_RESPONSE_H
