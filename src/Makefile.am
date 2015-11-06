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

pkgincludedir = $(includedir)/$(PACKAGE)

API_INC		= $(top_srcdir)/include
INC_DIR 	= $(top_srcdir)/include
ANSLPMSG_INCDIR	= $(INC_DIR)/msg
ANSLPMSG_LIBDIR	= $(top_srcdir)/src/msg/
ANSLPMSG_LLIB = anslp_msg


lib_LTLIBRARIES = libanslp.la

pkginclude_HEADERS = $(INC_DIR)/anslp_config.h \
					 $(INC_DIR)/mspec_rule_key.h \
					 $(INC_DIR)/anslp_daemon.h \
					 $(INC_DIR)/netauct_rule_installer.h \
					 $(INC_DIR)/anslp_timers.h \
					 $(INC_DIR)/nf_session.h \
					 $(INC_DIR)/auction_rule.h \
					 $(INC_DIR)/ni_session.h \
					 $(INC_DIR)/auction_rule_installer.h \
					 $(INC_DIR)/nop_auction_rule_installer.h \
					 $(INC_DIR)/benchmark_journal.h \
					 $(INC_DIR)/nr_session.h \
					 $(INC_DIR)/dispatcher.h \
					 $(INC_DIR)/session.h \
					 $(INC_DIR)/events.h \
					 $(INC_DIR)/session_id.h \
					 $(INC_DIR)/gistka_mapper.h \
					 $(INC_DIR)/session_manager.h



if ENABLE_DEBUG
AM_CXXFLAGS = -I$(top_srcdir)/include \
   			  -g  -fno-inline -DDEBUG -D_GLIBCXX_DEBUG -ggdb
else
AM_CXXFLAGS = -I$(top_srcdir)/include \
			  -O2
endif

libanslp_la_CPPFLAGS = -I$(API_INC) -I$(ANSLPMSG_INCDIR) 
libanslp_la_CPPFLAGS += $(LIBGIST_CFLAGS) $(LIBPROT_CFLAGS) $(LIBFASTQUEUE_CFLAGS) $(LIBIPAP_CFLAGS)
libanslp_la_CPPFLAGS += @LIBXML_CFLAGS@ @CURL_CFLAGS@ @LIBXSLT_CFLAGS@ @LIBUUID_CFLAGS@

libanslp_la_SOURCES = anslp_timers.cpp \
					  auction_rule.cpp \
					  auction_rule_installer.cpp \
					  benchmark_journal.cpp \
					  dispatcher.cpp \
					  gistka_mapper.cpp \
					  mspec_rule_key.cpp \
					  netauct_rule_installer.cpp \
					  nf_session.cpp \
					  ni_session.cpp \
					  nop_auction_rule_installer.cpp \
					  nr_session.cpp \
					  session.cpp \
					  session_id.cpp \
					  session_manager.cpp \
					  anslp_config.cpp \
					  anslp_daemon.cpp

libanslp_la_LIBADD = -L$(ANSLPMSG_LIBDIR) -l$(ANSLPMSG_LLIB) $(LIBGIST_LIBS) 
libanslp_la_LIBADD += $(LIBPROT_LIBS) $(LIBFASTQUEUE_LIBS) $(LIBGIST_LIBS) $(LIBIPAP_LIBS)
libanslp_la_LIBADD += -lnetfilter_queue -lssl -lcrypto -lrt $(LD_SCTP_LIB) -lpthread -lxml2
libanslp_la_LIBADD += @LIBXML_LIBS@ @CURL_LIBS@ @LIBXSLT_LIBS@ @LIBUUID_LIBS@

bin_PROGRAMS = anslpd

LDADD = $(LIBPROT_LIBS) $(LIBFASTQUEUE_LIBS) $(LIBGIST_LIBS) $(LIBIPAP_LIBS)


anslpd_CPPFLAGS = -I$(API_INC) -I$(ANSLPMSG_INCDIR) 
anslpd_CPPFLAGS += $(LIBGIST_CFLAGS) $(LIBPROT_CFLAGS) $(LIBFASTQUEUE_CFLAGS) $(LIBIPAP_CFLAGS)
anslpd_CPPFLAGS += @LIBXML_CFLAGS@ @CURL_CFLAGS@ @LIBXSLT_CFLAGS@ @LIBUUID_CFLAGS@


anslpd_SOURCES = benchmark_journal.cpp gistka_mapper.cpp session_id.cpp \
		    dispatcher.cpp nf_session.cpp ni_session.cpp \
	   	    nr_session.cpp session_manager.cpp session.cpp \
	   	    netauct_rule_installer.cpp \
	   	    auction_rule.cpp mspec_rule_key.cpp main.cpp \
	   	    nop_auction_rule_installer.cpp auction_rule_installer.cpp \
	   	    anslp_timers.cpp anslp_config.cpp anslp_daemon.cpp

if USE_WITH_SCTP
LD_SCTP_LIB= -lsctp
endif

anslpd_LDADD = -L$(ANSLPMSG_LIBDIR) -l$(ANSLPMSG_LLIB) $(LIBGIST_LIBS) 
anslpd_LDADD += $(LIBPROT_LIBS) $(LIBFASTQUEUE_LIBS) $(LIBIPAP_LIBS)
anslpd_LDADD += -lnetfilter_queue -lssl -lcrypto -lrt $(LD_SCTP_LIB) -lpthread -lxml2
anslpd_LDADD += @LIBXML_LIBS@ @CURL_LIBS@ @LIBXSLT_LIBS@ @LIBUUID_LIBS@


if NSIS_NO_WARN_HASHMAP
AM_CXXFLAGS += -Wno-deprecated
else !NSIS_NO_WARN_HASHMAP
AM_CXXFLAGS += 
endif


# end of Makefile.am
