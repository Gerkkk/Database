if(EXISTS "/mnt/c/Users/danpo/OneDrive/Рабочий стол/working db/Database/Test[1]_tests.cmake")
  include("/mnt/c/Users/danpo/OneDrive/Рабочий стол/working db/Database/Test[1]_tests.cmake")
else()
  add_test(Test_NOT_BUILT Test_NOT_BUILT)
endif()
