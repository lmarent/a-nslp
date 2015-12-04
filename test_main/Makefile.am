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

bin_PROGRAMS = main

LDADD = $(LIBPROT_LIBS) $(LIBFASTQUEUE_LIBS) $(LIBGIST_LIBS) $(LIBIPAP_LIBS)

API_INC		= $(top_srcdir)/include
INC_DIR 	= $(top_srcdir)/include/
ANSLPMSG_INCDIR	= $(INC_DIR)/msg
ANSLPMSG_LIBDIR	= $(top_srcdir)/src/msg/
ANSLPMSG_LLIB = anslp_msg

main_CPPFLAGS = -I$(API_INC) -I$(ANSLPMSG_INCDIR) 
main_CPPFLAGS += $(LIBGIST_CFLAGS) $(LIBPROT_CFLAGS) $(LIBFASTQUEUE_CFLAGS) $(LIBIPAP_CFLAGS)
main_CPPFLAGS += @LIBXML_CFLAGS@ @CURL_CFLAGS@ @LIBXSLT_CFLAGS@ @LIBUUID_CFLAGS@

main_SOURCES = @top_srcdir@/src/msg/ie_object_key.cpp \
			   @top_srcdir@/src/msg/ie_store.cpp \
			   @top_srcdir@/src/msg/anslp_ie.cpp \
			   @top_srcdir@/src/msg/anslp_object.cpp \
		       @top_srcdir@/src/msg/msg_sequence_number.cpp \
			   @top_srcdir@/src/msg/selection_auctioning_entities.cpp \
			   @top_srcdir@/src/msg/information_code.cpp \
			   @top_srcdir@/src/msg/message_hop_count.cpp \
			   @top_srcdir@/src/msg/session_lifetime.cpp \
			   @top_srcdir@/src/msg/ntlp_msg.cpp \
			   @top_srcdir@/src/msg/anslp_msg.cpp	\
			   @top_srcdir@/src/msg/xml_object_key.cpp \
			   @top_srcdir@/src/msg/anslp_ipap_message.cpp \
			   @top_srcdir@/src/msg/XMLParser.cpp \
			   @top_srcdir@/src/msg/anslp_ipap_xml_message.cpp \
			   @top_srcdir@/src/msg/anslp_create.cpp \
			   @top_srcdir@/src/msg/anslp_notify.cpp \
			   @top_srcdir@/src/msg/anslp_refresh.cpp \
			   @top_srcdir@/src/msg/anslp_response.cpp \
			   @top_srcdir@/src/msg/anslp_mspec_object.cpp \
			   @top_srcdir@/src/anslp_config.cpp \
			   @top_srcdir@/test_main/anslp_ipap_xml_message_test.cpp \
			   @top_srcdir@/test_main/main.cpp


if USE_WITH_SCTP
LD_SCTP_LIB= -lsctp
endif

main_LDADD = -L$(ANSLPMSG_LIBDIR) -l$(ANSLPMSG_LLIB) $(LIBGIST_LIBS) 
main_LDADD += $(LIBPROT_LIBS) $(LIBFASTQUEUE_LIBS) $(LIBIPAP_LIBS)
main_LDADD += -lnetfilter_queue -lssl -lcrypto -lrt $(LD_SCTP_LIB) -lxml2
main_LDADD += @LIBXML_LIBS@ @CURL_LIBS@ @LIBXSLT_LIBS@ @LIBUUID_LIBS@


if ENABLE_DEBUG
AM_CXXFLAGS = -I$(top_srcdir)/include \
   			  -g  -fno-inline -DDEBUG -ggdb
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
