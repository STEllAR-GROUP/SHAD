set(program
array
unordered_set
unordered_map
execute_at_task
execute_on_all_task
rdma
for_each
)


foreach(p ${program})
  add_executable(${p} ${p}.cc)
  target_link_libraries(${p} ${SHAD_RUNTIME_LIB} runtime)
endforeach(p)