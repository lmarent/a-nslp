# -----------------------------------*- mode: Makefile; -*--
# Makefile.am - Makefile.am for Auctioning Processing NSLP message lib
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
ACLOCAL_AMFLAGS = -I m4

pkgincludedir = $(includedir)/$(PACKAGE)/msg

LDADD = $(PROTLIB_LIB) @XMLLIB@

lib_LTLIBRARIES 	= libanslp_msg.la

API_INC		= $(top_srcdir)/include
INC_DIR 	= $(top_srcdir)/include/msg

libanslp_msg_la_CPPFLAGS = -I. -I$(API_INC) -I$(INC_DIR) -I$(ANSLP_INC) $(LIBGIST_CFLAGS)  \
					      $(LIBPROT_CFLAGS) $(LIBFASTQUEUE_CFLAGS) $(LIBIPAP_CFLAGS)


libanslp_msg_la_SOURCES = ie_object_key.cpp \
						  ie_store.cpp \
					      anslp_ie.cpp \
						  anslp_object.cpp \
					      msg_sequence_number.cpp \
					      selection_auctioning_entities.cpp \
						  information_code.cpp \
						  message_hop_count.cpp \
						  session_lifetime.cpp \
						  ntlp_msg.cpp \
					      anslp_msg.cpp	\
					      xml_object_key.cpp \
					      anslp_constants.cpp \
					      anslp_ipap_message.cpp \
					      anslp_ipap_message_splitter.cpp \
					      anslp_ipap_xml_message.cpp \
						  anslp_create.cpp \
						  anslp_notify.cpp \
						  anslp_refresh.cpp \
					      anslp_response.cpp \
					      anslp_bidding.cpp \
					      anslp_mspec_object.cpp 


pkginclude_HEADERS   = 	$(INC_DIR)/anslp_bidding.h \
						$(INC_DIR)/anslp_constants.h \
						$(INC_DIR)/anslp_create.h \
						$(INC_DIR)/anslp_ie.h \
						$(INC_DIR)/anslp_ipap_exception.h \
						$(INC_DIR)/anslp_ipap_message.h \
						$(INC_DIR)/anslp_ipap_message_splitter.h \
						$(INC_DIR)/anslp_ipap_xml_message.h \
						$(INC_DIR)/anslp_msg.h \
						$(INC_DIR)/anslp_mspec_object.h \
						$(INC_DIR)/anslp_notify.h \
						$(INC_DIR)/anslp_object.h \
						$(INC_DIR)/anslp_refresh.h \
						$(INC_DIR)/anslp_response.h \
						$(INC_DIR)/ie_object_key.h \
						$(INC_DIR)/ie_store.h \
						$(INC_DIR)/information_code.h \
						$(INC_DIR)/message_hop_count.h \
						$(INC_DIR)/msg_sequence_number.h \
						$(INC_DIR)/ntlp_msg.h \
						$(INC_DIR)/selection_auctioning_entities.h \
						$(INC_DIR)/session_lifetime.h \
						$(INC_DIR)/xml_object_key.h


if ENABLE_DEBUG
  AM_CXXFLAGS = -I$(top_srcdir)/include \
				-g  -fno-inline -DDEBUG @LIBXML2_CFLAGS@ -ggdb
else
  AM_CXXFLAGS = -I$(top_srcdir)/include \
				-O2 @LIBXML2_CFLAGS@ 
endif


if NSIS_NO_WARN_HASHMAP
	AM_CXXFLAGS += -Wno-deprecated
else !NSIS_NO_WARN_HASHMAP
	AM_CXXFLAGS += 
endif
