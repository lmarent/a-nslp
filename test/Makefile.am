check_PROGRAMS = test_runner

API_INC			= $(top_srcdir)/include
INC_DIR 		= $(top_srcdir)/include/
ANSLPMSG_INCDIR	= $(INC_DIR)/msg
ANSLPMSG_LIBDIR	= $(abs_top_builddir)/src/msg
ANSLPMSG_LLIB 	= anslp_msg


test_runner_SOURCES =  @top_srcdir@/src/benchmark_journal.cpp \
					   @top_srcdir@/src/gistka_mapper.cpp \
					   @top_srcdir@/src/aqueue.cpp \
					   @top_srcdir@/src/session_id.cpp \
					   @top_srcdir@/src/dispatcher.cpp \
					   @top_srcdir@/src/nf_session.cpp \
					   @top_srcdir@/src/ni_session.cpp \
					   @top_srcdir@/src/nr_session.cpp \
					   @top_srcdir@/src/session_manager.cpp \
					   @top_srcdir@/src/session.cpp \
					   @top_srcdir@/src/netauct_rule_installer.cpp \
				       @top_srcdir@/src/anslp_config.cpp \
				       @top_srcdir@/src/anslp_timers.cpp \
					   @top_srcdir@/src/auction_rule.cpp \
					   @top_srcdir@/src/mspec_rule_key.cpp \
					   @top_srcdir@/src/nop_auction_rule_installer.cpp \
					   @top_srcdir@/src/auction_rule_installer.cpp \
					   @top_srcdir@/test/utils.cpp \
					   @top_srcdir@/test/basic.cpp \
					   @top_srcdir@/test/anslp_ipap_message_test.cpp \
					   @top_srcdir@/test/anslp_ipap_xml_message_test.cpp \
					   @top_srcdir@/test/generic_object_test.cpp \
					   @top_srcdir@/test/information_code_test.cpp \
					   @top_srcdir@/test/message_hop_count_test.cpp \
					   @top_srcdir@/test/msg_sequence_number_test.cpp \
					   @top_srcdir@/test/session_lifetime_test.cpp \
					   @top_srcdir@/test/anslp_msg_test.cpp \
					   @top_srcdir@/test/anslp_create_test.cpp \
					   @top_srcdir@/test/anslp_notify_test.cpp \
					   @top_srcdir@/test/anslp_refresh_test.cpp \
					   @top_srcdir@/test/anslp_response_test.cpp \
					   @top_srcdir@/test/session_id_test.cpp \
					   @top_srcdir@/test/ni_session_test.cpp \
					   @top_srcdir@/test/nf_session_test.cpp \
					   @top_srcdir@/test/nr_session_test.cpp \
					   @top_srcdir@/test/netauct_rule_installer_test.cpp \
					   @top_srcdir@/test/test_runner.cpp

if USE_WITH_SCTP
LD_SCTP_LIB= -lsctp
endif


test_runner_CPPFLAGS  = -I$(API_INC) -I$(ANSLPMSG_INCDIR) $(LIBIPAP_CFLAGS)
test_runner_CPPFLAGS += $(LIBGIST_CFLAGS) $(LIBPROT_CFLAGS) $(LIBFASTQUEUE_CFLAGS)
test_runner_CPPFLAGS += @LIBXML_CFLAGS@ @CURL_CFLAGS@ @LIBXSLT_CFLAGS@ @LIBUUID_CFLAGS@

test_runner_LDADD  = -L$(ANSLPMSG_LIBDIR) -lanslp_msg $(LIBGIST_LIBS) 
test_runner_LDADD += $(LIBPROT_LIBS) $(LIBFASTQUEUE_LIBS) $(LIBIPAP_LIBS)
test_runner_LDADD += -lnetfilter_queue -lssl -lcrypto -lrt $(LD_SCTP_LIB) -lpthread -lxml2
test_runner_LDADD += @LIBXML_LIBS@ @CURL_LIBS@ @LIBXSLT_LIBS@ @LIBUUID_LIBS@

TESTS = $(check_PROGRAMS)

if ENABLE_DEBUG
  AM_CXXFLAGS = -I$(top_srcdir)/include \
				-g  -fno-inline -DDEBUG 
else
  AM_CXXFLAGS = -I$(top_srcdir)/include \
				-O2 
endif

AM_CXXFLAGS += -Wall -ggdb -pedantic -Wno-long-long

if NSIS_NO_WARN_HASHMAP
AM_CXXFLAGS += -Wno-deprecated
endif

#  TEST_CXX_FLAGS=`cppunit-config --cflags`
