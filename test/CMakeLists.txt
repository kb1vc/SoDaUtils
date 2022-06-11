
########### next target ###############

set(TST_FMT_SRCS
	FormatTest.cxx
	../src/Format.cxx
	)



add_executable(FormatTest ${TST_FMT_SRCS})
      
add_executable(MailBoxTest MailBoxTest.cxx)
target_link_libraries(MailBoxTest sodautils)

add_executable(BarrierTest BarrierTest.cxx)
target_link_libraries(BarrierTest sodautils)

