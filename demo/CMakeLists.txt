project(vb_demo)
set(CMAKE_CXX_STANDARD 17)

add_executable(vb_demo vb_demo.cpp)

target_link_libraries(vb_demo
	vb_libWasmModule
)
EnableClangTidyMSVS(vb_demo)

add_executable(vb_string_view_repro
	vb_string_view_repro.cpp
	vb_string_view_repro_runner.cpp
)

target_link_libraries(vb_string_view_repro
	vb_libWasmModule
)
EnableClangTidyMSVS(vb_string_view_repro)

