/*
 * A ANSLP REFRESH Message.
 *
 * $Id: anslp_refresh.h 2014-11-05  $
 * $HeadURL: https://./include/msg/anslp_refresh.h $
 */
#ifndef ANSLP_REFRESH_H
#define ANSLP_REFRESH_H

#include "ie.h"

#include "anslp_msg.h"


namespace anslp {
  namespace msg {


/**
 * A ANSLP Refresh Message.
 *
 * This class implements a ANSLP REFRESH Message.
 */
class anslp_refresh : public anslp_msg {

  public:
	static const uint16 MSG_TYPE = 0x5;

	explicit anslp_refresh();
	explicit anslp_refresh(const anslp_refresh &other);
	virtual ~anslp_refresh();

	/*
	 * Inherited methods:
	 */
	anslp_refresh *new_instance() const;
	anslp_refresh *copy() const;
	void register_ie(IEManager *iem) const;
	virtual void serialize(NetMsg &msg, coding_t coding, uint32 &bytes_written) const throw (IEError);
	virtual bool check() const;

	/*
	 * New methods:
	 */
	void set_session_lifetime(uint32 seconds);
	uint32 get_session_lifetime() const;


	void set_msg_sequence_number(uint32 msn);
	uint32 get_msg_sequence_number() const;

  protected:
	uint32 serialize_object(ie_object_key &key, 
						    NetMsg &msg, 
						    coding_t coding) const;

};


  } // namespace msg
} // namespace anslp

#endif // ANSLP_REFRESH_H
