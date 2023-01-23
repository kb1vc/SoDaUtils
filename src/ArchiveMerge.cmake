# taken from
# https://stackoverflow.com/questions/37924383/combining-several-static-libraries-into-one-using-cmake
# response by "zbut" on Nov 8, 2018

function(combine_archives output_archive list_of_input_archives list_of_dep_archives)
  message("ArchiveMerge: list of input archives [${list_of_input_archives}]")
    set(mri_file ${CMAKE_CURRENT_BINARY_DIR}/${output_archive}.mri)
    set(FULL_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/lib${output_archive}.a)
    file(WRITE ${mri_file} "create ${FULL_OUTPUT_PATH}\n")
    FOREACH(in_archive ${list_of_input_archives})
        file(APPEND ${mri_file} "addlib ${in_archive}\n")
    ENDFOREACH()
    file(APPEND ${mri_file} "save\n")
    file(APPEND ${mri_file} "end\n")

    set(output_archive_dummy_file ${CMAKE_CURRENT_BINARY_DIR}/${output_archive}.dummy.cpp)
    add_custom_command(OUTPUT ${output_archive_dummy_file}
                       COMMAND touch ${output_archive_dummy_file}
                       DEPENDS ${dep_archives})

    message("\n\n******\n**** About to create library [${output_archive}]\n\n")     
    add_library(${output_archive} STATIC ${output_archive_dummy_file})
		     
    add_custom_command(TARGET ${output_archive}
                       POST_BUILD
                       COMMAND ar -M < ${mri_file})
endfunction(combine_archives)
