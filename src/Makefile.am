# -----------------------------------*- mode: Makefile; -*--
# Makefile.am - Makefile.am for ANSLP NSLP
# ----------------------------------------------------------
# $Id$
# $HeadURL$
# ==========================================================
#                      
# (C)opyright, all rights reserved by
# - System and Computing Engineering, Universidad de los Andes
# ==========================================================
#
#

bin_PROGRAMS = anslpd test_client

LDADD = $(LIBPROT_LIBS) $(LIBFASTQUEUE_LIBS) $(LIBGIST_LIBS) $(LIBIPAP_LIBS)

API_INC		= $(top_srcdir)/include
INC_DIR 	= $(top_srcdir)/include/
ANSLPMSG_INCDIR	= $(INC_DIR)/msg
ANSLPMSG_LIBDIR	= $(top_srcdir)/src/msg/
ANSLPMSG_LLIB = anslp_msg

anslpd_CPPFLAGS = -I$(API_INC) -I$(ANSLPMSG_INCDIR) 
anslpd_CPPFLAGS += $(LIBGIST_CFLAGS) $(LIBPROT_CFLAGS) $(LIBFASTQUEUE_CFLAGS) $(LIBIPAP_CFLAGS)
anslpd_CPPFLAGS += @LIBXML_CFLAGS@ @CURL_CFLAGS@ @LIBXSLT_CFLAGS@ @LIBUUID_CFLAGS@


test_client_CPPFLAGS = -I$(API_INC) -I$(ANSLPMSG_INCDIR) -I$(LIBGIST_CFLAGS) -I$(LIBPROT_CFLAGS) -I$(LIBFASTQUEUE_CFLAGS) -I$(LIBIPAP_CFLAGS)
test_client_CPPFLAGS += @LIBXML_CFLAGS@ @CURL_CFLAGS@ @LIBXSLT_CFLAGS@ @LIBUUID_CFLAGS@


anslpd_SOURCES = benchmark_journal.cpp gistka_mapper.cpp session_id.cpp \
		    dispatcher.cpp nf_session.cpp ni_session.cpp \
	   	    nr_session.cpp session_manager.cpp session.cpp \
	   	    netauct_rule_installer.cpp \
	   	    anslp_config.cpp anslp_timers.cpp anslp_daemon.cpp \
	   	    auction_rule.cpp mspec_rule_key.cpp main.cpp \
	   	    nop_auction_rule_installer.cpp auction_rule_installer.cpp

test_client_SOURCES = test_client.cpp

if USE_WITH_SCTP
LD_SCTP_LIB= -lsctp
endif

anslpd_LDADD = -L$(ANSLPMSG_LIBDIR) -l$(ANSLPMSG_LLIB) $(LIBGIST_LIBS) 
anslpd_LDADD += $(LIBPROT_LIBS) $(LIBFASTQUEUE_LIBS) $(LIBIPAP_LIBS)
anslpd_LDADD += -lnetfilter_queue -lssl -lcrypto -lrt $(LD_SCTP_LIB) -lpthread -lxml2
anslpd_LDADD += @LIBXML_LIBS@ @CURL_LIBS@ @LIBXSLT_LIBS@ @LIBUUID_LIBS@

test_client_LDADD = -L. -l$(ANSLP_LLIB) -L$(ANSLP_MSG_LIBDIR) -l$(ANSLPMSG_LLIB) $(LIBGIST_LIBS) $(LIBPROT_LIBS) $(LIBFASTQUEUE_LIBS) $(LIBIPAP_LIBS)
test_client_LDADD += -lnetfilter_queue -lssl -lrt $(LD_SCTP_LIB) -lpthread -lxml2
test_client_LDADD += @LIBXML_LIBS@ @CURL_LIBS@ @LIBXSLT_LIBS@ @LIBUUID_LIBS@

include_HEADERS = $(INC_DIR)/session_id.h \
				  $(ANSLPMSG_INCDIR)/msg_sequence_number.h \
				  $(ANSLPMSG_INCDIR)/anslp_ie.h \
				  $(ANSLPMSG_INCDIR)/anslp_object.h \
	        	  $(ANSLPMSG_INCDIR)/ntlp_msg.h \
	        	  $(ANSLPMSG_INCDIR)/ie_store.h	\
	        	  $(ANSLPMSG_INCDIR)/message_hop_count.h \
	       	 	  $(ANSLPMSG_INCDIR)/anslp_msg.h \
	       	 	  $(ANSLPMSG_INCDIR)/anslp_create.h \
	       	 	  $(ANSLPMSG_INCDIR)/anslp_response.h	\
	        	  $(ANSLPMSG_INCDIR)/session_lifetime.h \
	        	  $(ANSLPMSG_INCDIR)/information_code.h \
	        	  $(ANSLPMSG_INCDIR)/anslp_refresh.h \
	        	  $(ANSLPMSG_INCDIR)/anslp_notify.h 


if ENABLE_DEBUG
AM_CXXFLAGS = -I$(top_srcdir)/include \
   			  -g  -fno-inline -DDEBUG -D_GLIBCXX_DEBUG
else
AM_CXXFLAGS = -I$(top_srcdir)/include \
			  -O2 
endif

if NSIS_NO_WARN_HASHMAP
AM_CXXFLAGS += -Wno-deprecated
else !NSIS_NO_WARN_HASHMAP
AM_CXXFLAGS += 
endif


# end of Makefile.am
