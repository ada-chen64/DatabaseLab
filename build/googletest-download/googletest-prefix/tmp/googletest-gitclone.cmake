if("e8b478a7" STREQUAL "")
  message(FATAL_ERROR "Tag for git checkout should not be empty.")
endif()

set(run 0)

if("/home/ada/Documents/Tsinghua_Year_4/shujuku/dbtrain-lab-2017080067/build/googletest-download/googletest-prefix/src/googletest-stamp/googletest-gitinfo.txt" IS_NEWER_THAN "/home/ada/Documents/Tsinghua_Year_4/shujuku/dbtrain-lab-2017080067/build/googletest-download/googletest-prefix/src/googletest-stamp/googletest-gitclone-lastrun.txt")
  set(run 1)
endif()

if(NOT run)
  message(STATUS "Avoiding repeated git clone, stamp file is up to date: '/home/ada/Documents/Tsinghua_Year_4/shujuku/dbtrain-lab-2017080067/build/googletest-download/googletest-prefix/src/googletest-stamp/googletest-gitclone-lastrun.txt'")
  return()
endif()

execute_process(
  COMMAND ${CMAKE_COMMAND} -E remove_directory "/home/ada/Documents/Tsinghua_Year_4/shujuku/dbtrain-lab-2017080067/build/googletest-src"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to remove directory: '/home/ada/Documents/Tsinghua_Year_4/shujuku/dbtrain-lab-2017080067/build/googletest-src'")
endif()

set(git_options)

# disable cert checking if explicitly told not to do it
set(tls_verify "")
if(NOT "x" STREQUAL "x" AND NOT tls_verify)
  list(APPEND git_options
    -c http.sslVerify=false)
endif()

set(git_clone_options)

set(git_shallow "")
if(git_shallow)
  list(APPEND git_clone_options --depth 1 --no-single-branch)
endif()

set(git_progress "")
if(git_progress)
  list(APPEND git_clone_options --progress)
endif()

set(git_config "")
foreach(config IN LISTS git_config)
  list(APPEND git_clone_options --config ${config})
endforeach()

# try the clone 3 times incase there is an odd git clone issue
set(error_code 1)
set(number_of_tries 0)
while(error_code AND number_of_tries LESS 3)
  execute_process(
    COMMAND "/usr/bin/git" ${git_options} clone ${git_clone_options} --origin "origin" "git@git.tsinghua.edu.cn:dbtrain-2021/googletest.git" "googletest-src"
    WORKING_DIRECTORY "/home/ada/Documents/Tsinghua_Year_4/shujuku/dbtrain-lab-2017080067/build"
    RESULT_VARIABLE error_code
    )
  math(EXPR number_of_tries "${number_of_tries} + 1")
endwhile()
if(number_of_tries GREATER 1)
  message(STATUS "Had to git clone more than once:
          ${number_of_tries} times.")
endif()
if(error_code)
  message(FATAL_ERROR "Failed to clone repository: 'git@git.tsinghua.edu.cn:dbtrain-2021/googletest.git'")
endif()

execute_process(
  COMMAND "/usr/bin/git" ${git_options} checkout e8b478a7 --
  WORKING_DIRECTORY "/home/ada/Documents/Tsinghua_Year_4/shujuku/dbtrain-lab-2017080067/build/googletest-src"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to checkout tag: 'e8b478a7'")
endif()

execute_process(
  COMMAND "/usr/bin/git" ${git_options} submodule init 
  WORKING_DIRECTORY "/home/ada/Documents/Tsinghua_Year_4/shujuku/dbtrain-lab-2017080067/build/googletest-src"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to init submodules in: '/home/ada/Documents/Tsinghua_Year_4/shujuku/dbtrain-lab-2017080067/build/googletest-src'")
endif()

execute_process(
  COMMAND "/usr/bin/git" ${git_options} submodule update --recursive --init 
  WORKING_DIRECTORY "/home/ada/Documents/Tsinghua_Year_4/shujuku/dbtrain-lab-2017080067/build/googletest-src"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to update submodules in: '/home/ada/Documents/Tsinghua_Year_4/shujuku/dbtrain-lab-2017080067/build/googletest-src'")
endif()

# Complete success, update the script-last-run stamp file:
#
execute_process(
  COMMAND ${CMAKE_COMMAND} -E copy
    "/home/ada/Documents/Tsinghua_Year_4/shujuku/dbtrain-lab-2017080067/build/googletest-download/googletest-prefix/src/googletest-stamp/googletest-gitinfo.txt"
    "/home/ada/Documents/Tsinghua_Year_4/shujuku/dbtrain-lab-2017080067/build/googletest-download/googletest-prefix/src/googletest-stamp/googletest-gitclone-lastrun.txt"
  WORKING_DIRECTORY "/home/ada/Documents/Tsinghua_Year_4/shujuku/dbtrain-lab-2017080067/build/googletest-src"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to copy script-last-run stamp file: '/home/ada/Documents/Tsinghua_Year_4/shujuku/dbtrain-lab-2017080067/build/googletest-download/googletest-prefix/src/googletest-stamp/googletest-gitclone-lastrun.txt'")
endif()

