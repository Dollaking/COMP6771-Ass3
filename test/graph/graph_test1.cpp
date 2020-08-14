#include "gdwg/graph.hpp"

#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <iostream>
#include <memory>
#include <sstream>

TEST_CASE("graph_test1") {
	auto g = gdwg::graph<int, std::string>{};
	CHECK(g.empty());
}
