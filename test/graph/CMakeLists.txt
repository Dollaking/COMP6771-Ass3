cxx_test(
   TARGET graph_test1
   FILENAME "graph_test1.cpp"
   LINK absl::flat_hash_set absl::flat_hash_map gsl::gsl-lite-v1 fmt::fmt-header-only range-v3
)

cxx_test(
   TARGET my_test
   FILENAME "my_test.cpp"
   LINK absl::flat_hash_set absl::flat_hash_map gsl::gsl-lite-v1 fmt::fmt-header-only range-v3
)
