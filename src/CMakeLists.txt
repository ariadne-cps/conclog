add_library(CONCLOG_SRC OBJECT
   logging.cpp
)

if(COVERAGE)
    target_link_libraries(CONCLOG_SRC coverage_config)
endif()