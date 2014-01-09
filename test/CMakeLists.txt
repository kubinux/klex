include_directories(${klex_SOURCE_DIR}/src
                    ${GTEST_ROOT}/include
                    ${GTEST_ROOT}
                    )

add_executable(klex-unit-tests
               InputStream.t.cpp
               )

target_link_libraries(klex-unit-tests
                      klex
                      gtest
                      gtest_main
                      )
