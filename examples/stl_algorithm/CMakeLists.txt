set(program
seq_set
par_set
print
)


foreach(p ${program})
  add_executable(${p} ${p}.cc)
  target_link_libraries(${p} ${SHAD_RUNTIME_LIB} runtime)
endforeach(p)
