
########### next target ###############

set(TST_FMT_SRCS
	FormatTest.cxx
	../src/Format.cxx
	)



add_executable(FormatTest ${TST_FMT_SRCS})
      
add_executable(MailBoxTest MailBoxTest.cxx ../src/Format.cxx ../src/Options.cxx ../src/UtilsBase.cxx ../src/Split.cxx)

