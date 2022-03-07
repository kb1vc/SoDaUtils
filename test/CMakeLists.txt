
########### next target ###############

set(TST_FMT_SRCS
	FormatTest.cxx
	../src/Format.cxx
	)



add_executable(FormatTest ${TST_FMT_SRCS})

add_executable(PropertyTest ../src/Property.cxx PropertyTest.cxx ../src/Split.cxx 	../src/PropertyIO_JSON.cxx)

