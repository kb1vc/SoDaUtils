
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

enable_testing()

add_test(NAME FastBarrierTest 
  COMMAND $<TARGET_FILE:BarrierTest> --maddur 35 --threads 20 --trials 100000)
set_tests_properties(FastBarrierTest PROPERTIES
  FAIL_REGULAR_EXPRESSION "Ooops, dang!"
  )

add_test(FastBarrierTest_should_timeout BarrierTest --maddur 1001000 --threads 10 --trials 100)
set_tests_properties(FastBarrierTest_should_timeout PROPERTIES
  PASS_REGULAR_EXPRESSION "Ooops, dang!"
  )


add_test(NAME MailBoxTest1
  COMMAND $<TARGET_FILE:MailBoxTest> -m 50000 -t 20 -r 10)
set_tests_properties(MailBoxTest1 PROPERTIES
  FAIL_REGULAR_EXPRESSION "subscriber")


add_test(NAME FastFormatTest 
  COMMAND $<TARGET_FILE:FormatTest>)

