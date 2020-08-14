#include "gdwg/graph.hpp"

#include <algorithm>
#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <memory>
#include <range/v3/iterator/operations.hpp>
#include <sstream>
#include <type_traits>

TEST_CASE("Main Graph Tests") {
	// ======================================
	//              Constructors
	// ======================================

	auto empty_constructor = gdwg::graph<int, std::string>();
	auto string_list = std::initializer_list<std::string>{"hello", "how", "are", "you?"};
	auto list_constructor = gdwg::graph<std::string, int>(string_list);
	auto node_it_constructor = gdwg::graph<std::string, int>(string_list.begin(), string_list.end());
	auto value_type_vector = std::vector<gdwg::graph<int, int>::value_type>{
	   {4, 1, -4},
	   {3, 2, 2},
	   {2, 4, 2},
	   {2, 1, 1},
	   {6, 2, 5},
	   {6, 3, 10},
	   {1, 5, -1},
	   {3, 6, -8},
	   {4, 5, 3},
	   {5, 2, 7},
	};
	auto node_edge_it_constructor =
	   gdwg::graph<int, int>(value_type_vector.begin(), value_type_vector.end());

	auto copy_constructor = list_constructor;
	auto copy_constructor2 = gdwg::graph<std::string, int>(list_constructor);

	auto temp_constructor = node_edge_it_constructor;
	auto temp_constructor2 = node_edge_it_constructor;
	auto move_constructor = std::move(temp_constructor);
	auto move_constructor2 = gdwg::graph<int, int>(std::move(temp_constructor2));

	SECTION("Empty Constructor Test") {
		CHECK(empty_constructor.empty());
	}

	SECTION("List Constructor Test") {
		auto sorted_string_list = static_cast<std::vector<std::string>>(string_list);
		std::sort(sorted_string_list.begin(), sorted_string_list.end());
		CHECK(list_constructor.nodes() == sorted_string_list);
	}

	SECTION("Node Iterator Constructor Test") {
		CHECK(node_it_constructor == list_constructor);
	}

	SECTION("Node Edge Iterator Constructor Test") {
		auto sort_nodes = std::vector<int>{1, 2, 3, 4, 5, 6};
		std::sort(sort_nodes.begin(), sort_nodes.end());
		CHECK(node_edge_it_constructor.nodes() == sort_nodes);
	}

	SECTION("Copy Constructor Test") {
		CHECK(copy_constructor == copy_constructor2);
		CHECK(copy_constructor2 == list_constructor);
	}

	SECTION("Move Constructor Test") {
		CHECK(move_constructor == move_constructor2);
		CHECK(node_edge_it_constructor.nodes() == move_constructor2.nodes());
		CHECK(node_edge_it_constructor.all_edges() == move_constructor2.all_edges());
		CHECK(node_edge_it_constructor == move_constructor2);
	}

	// ======================================
	//              Modifiers
	// ======================================

	auto const insert_values = std::vector<gdwg::graph<std::string, int>::value_type>{
	   {"hello", "are", 8},
	   {"hello", "are", 2},
	   {"how", "you?", 1},
	   {"how", "hello", 4},
	   {"are", "you?", 3},
	};

	auto insert_node_graph = gdwg::graph<std::string, int>();
	insert_node_graph.insert_node("hello");
	insert_node_graph.insert_node("how");
	insert_node_graph.insert_node("are");
	insert_node_graph.insert_node("you?");

	auto insert_edge_graph = insert_node_graph;
	insert_edge_graph.insert_edge("hello", "are", 8);
	insert_edge_graph.insert_edge("hello", "are", 2);
	insert_edge_graph.insert_edge("how", "you?", 1);
	insert_edge_graph.insert_edge("how", "hello", 4);
	insert_edge_graph.insert_edge("are", "you?", 3);

	auto insert_edge_control =
	   gdwg::graph<std::string, int>(insert_values.begin(), insert_values.end());

	auto replace_node_graph = insert_edge_graph;
	replace_node_graph.replace_node("how", "Aven is tired");

	auto duplicate_merge_list = std::vector<gdwg::graph<std::string, int>::value_type>{
	   {"A", "B", 1},
	   {"A", "C", 2},
	   {"A", "D", 3},
	   {"B", "B", 1},
	};
	auto basic_merge_list = std::vector<gdwg::graph<std::string, int>::value_type>{
	   {"A", "B", 1},
	   {"A", "C", 2},
	   {"A", "D", 3},
	   {"B", "B", 1},
	};

	auto control_merge_list = std::vector<gdwg::graph<std::string, int>::value_type>{
	   {"B", "C", 2},
	   {"B", "D", 3},
	   {"B", "B", 1},
	};

	auto control_erase_list = std::vector<gdwg::graph<std::string, int>::value_type>{
	   {"A", "B", 1},
	   {"A", "C", 2},
	   {"A", "D", 3},
	};
	auto control_erase_graph =
	   gdwg::graph<std::string, int>(control_erase_list.begin(), control_erase_list.end());
	auto basic_merge_graph =
	   gdwg::graph<std::string, int>(basic_merge_list.begin(), basic_merge_list.end());
	auto duplicate_merge_graph =
	   gdwg::graph<std::string, int>(duplicate_merge_list.begin(), duplicate_merge_list.end());
	auto control_merge_graph =
	   gdwg::graph<std::string, int>(control_merge_list.begin(), control_merge_list.end());
	basic_merge_graph.merge_replace_node("A", "B");
	duplicate_merge_graph.merge_replace_node("A", "B");

	auto erase_node_graph =
	   gdwg::graph<std::string, int>(duplicate_merge_list.begin(), duplicate_merge_list.end());
	erase_node_graph.erase_node("A");

	auto erase_edge_graph =
	   gdwg::graph<std::string, int>(duplicate_merge_list.begin(), duplicate_merge_list.end());
	erase_edge_graph.erase_edge("B", "B", 1);

	auto erase_edge_it_graph =
	   gdwg::graph<std::string, int>(duplicate_merge_list.begin(), duplicate_merge_list.end());
	;
	auto last_erase_it = erase_edge_it_graph.end();
	last_erase_it--;
	erase_edge_it_graph.erase_edge(last_erase_it);

	auto erase_edge_it_range_graph =
	   gdwg::graph<std::string, int>(duplicate_merge_list.begin(), duplicate_merge_list.end());

	auto clear_graph = duplicate_merge_graph;
	clear_graph.clear();

	SECTION("Insert Node Test") {
		CHECK(insert_node_graph.nodes() == std::vector<std::string>{"are", "hello", "how", "you?"});
		CHECK(!insert_node_graph.insert_node("hello"));
	}

	SECTION("Insert Edge Test") {
		CHECK(insert_edge_graph == insert_edge_control);
		CHECK(!insert_edge_graph.insert_edge("are", "you?", 3));
		CHECK_THROWS_WITH(insert_edge_graph.insert_edge("john", "you?", 3),
		                  "Cannot call gdwg::graph<N, E>::insert_edge when either src or dst node "
		                  "does not exist");
		CHECK_THROWS_WITH(insert_edge_graph.insert_edge("you?", "hill", 3),
		                  "Cannot call gdwg::graph<N, E>::insert_edge when either src or dst node "
		                  "does not exist");
		CHECK_THROWS_WITH(insert_edge_graph.insert_edge("john", "hill", 3),
		                  "Cannot call gdwg::graph<N, E>::insert_edge when either src or dst node "
		                  "does not exist");
	}

	SECTION("Replace Node Test") {
		CHECK(replace_node_graph.connections("Aven is tired") == insert_edge_graph.connections("how"));
		auto temp_replace = replace_node_graph;
		auto temp_insert_edge = insert_edge_graph;
		temp_replace.erase_node("Aven is tired");
		temp_insert_edge.erase_node("how");
		CHECK(temp_replace == temp_insert_edge);
		CHECK(!replace_node_graph.replace_node("Aven is tired", "you?"));
		CHECK_THROWS_WITH(replace_node_graph.replace_node("tony", "tim"),
		                  "Cannot call gdwg::graph<N, E>::replace_node on a node that doesn't exist");
	}

	SECTION("Merge Replace Node Test") {
		CHECK(basic_merge_graph == duplicate_merge_graph);
		CHECK(basic_merge_graph == control_merge_graph);
		CHECK_THROWS_WITH(basic_merge_graph.merge_replace_node("Z", "B"),
		                  "Cannot call gdwg::graph<N, E>::merge_replace_node on old or new data if "
		                  "they don't exist in the graph");
		CHECK_THROWS_WITH(basic_merge_graph.merge_replace_node("B", "I"),
		                  "Cannot call gdwg::graph<N, E>::merge_replace_node on old or new data if "
		                  "they don't exist in the graph");
	}

	SECTION("Erase Node Value Test") {
		CHECK(erase_node_graph.nodes() == std::vector<std::string>{"B", "C", "D"});
		CHECK_THROWS_WITH(erase_node_graph.is_connected("A", "B"),
		                  "Cannot call gdwg::graph<N, E>::is_connected if src or dst node don't "
		                  "exist in the graph");
		CHECK(!erase_node_graph.erase_node("A"));
	}

	SECTION("Erase Edge Value Test") {
		CHECK(control_erase_graph == erase_edge_graph);
		CHECK_THROWS_WITH(erase_node_graph.erase_edge("A", "Z", 2),
		                  "Cannot call gdwg::graph<N, E>::erase_edge on src or dst if they don't "
		                  "exist in the graph");
		CHECK(!erase_edge_graph.erase_edge("A", "C", 10));
	}

	SECTION("Erase Edge Iterator Test") {
		CHECK(erase_edge_it_graph == erase_edge_graph);
		CHECK(erase_edge_it_graph.erase_edge(erase_edge_it_graph.end()) == erase_edge_it_graph.end());
	}

	SECTION("Erase Edge Iterator Range Test") {
		auto ret_it = erase_edge_it_range_graph.erase_edge(erase_edge_it_range_graph.begin(),
		                                                   erase_edge_it_range_graph.end());
		CHECK(ret_it == erase_edge_it_range_graph.end());
		CHECK(erase_edge_it_range_graph.connections("A").empty());
		CHECK(erase_edge_it_range_graph.connections("B").empty());
		CHECK(erase_edge_it_range_graph.connections("C").empty());
		CHECK(erase_edge_it_range_graph.connections("D").empty());
	}

	SECTION("Clear Test") {
		CHECK(clear_graph.empty());
	}

	// ======================================
	//              Accessors
	// ======================================

	auto is_node_graph =
	   gdwg::graph<std::string, std::string>{std::initializer_list<std::string>{"A", "B"}};
	SECTION("Is Node Test") {
		CHECK(is_node_graph.is_node("A"));
		CHECK(!is_node_graph.is_node("Z"));
	}

	auto empty_test_graph = empty_constructor;
	auto not_empty_graph = is_node_graph;
	SECTION("Empty Test") {
		CHECK(empty_test_graph.empty());
		CHECK(!not_empty_graph.empty());
	}

	auto nodes_test_graph1 = gdwg::graph<std::string, std::string>();
	auto nodes_test_graph2 = is_node_graph;
	SECTION("Nodes Test") {
		CHECK(nodes_test_graph1.nodes().empty());
		CHECK(nodes_test_graph2.nodes() == std::vector<std::string>{"A", "B"});
	}

	auto is_connected_graph = is_node_graph;
	SECTION("Is Connected Test") {
		CHECK(!is_connected_graph.is_connected("A", "B"));
		is_connected_graph.insert_edge("A", "B", "John Smith");
		CHECK(is_connected_graph.is_connected("A", "B"));
		CHECK_THROWS_WITH(is_connected_graph.is_connected("Jimmy", "Butler"),
		                  "Cannot call gdwg::graph<N, E>::is_connected if src or dst node don't "
		                  "exist in the graph");
	}
	auto weights_test_graph =
	   gdwg::graph<std::string, int>(insert_values.begin(), insert_values.end());
	SECTION("Weights Test") {
		CHECK(weights_test_graph.weights("hello", "are") == std::vector<int>{2, 8});
		CHECK_THROWS_WITH(weights_test_graph.weights("john", "smit"),
		                  "Cannot call gdwg::graph<N, E>::weights if src or dst node don't exist in "
		                  "the graph");
	}

	auto find_test_graph = gdwg::graph<std::string, int>(insert_values.begin(), insert_values.end());
	SECTION("Find Test") {
		CHECK(find_test_graph.find("are", "you?", 3) == find_test_graph.begin());
		CHECK(find_test_graph.find("Danny", "Johnson", 9) == find_test_graph.end());
	}

	auto connections_test_graph =
	   gdwg::graph<std::string, int>(insert_values.begin(), insert_values.end());
	SECTION("Connection Test") {
		CHECK(connections_test_graph.connections("how") == std::vector<std::string>{"hello", "you?"});
		CHECK_THROWS_WITH(connections_test_graph.connections("UNSW"),
		                  "Cannot call gdwg::graph<N, E>::connections if src doesn't exist in the "
		                  "graph");
	}

	// ======================================
	//              Iterators
	// ======================================
	auto range_test_graph = gdwg::graph<std::string, int>(insert_values.begin(), insert_values.end());
	auto begin_values = ranges::common_tuple<std::string, std::string, int>{"are", "you?", 3};
	auto end_values = ranges::common_tuple<std::string, std::string, int>{"how", "you?", 1};
	SECTION("Iterator Tests") {
		CHECK(*range_test_graph.begin() == begin_values);
		auto last_it = range_test_graph.end();
		last_it--;
		CHECK(*last_it == end_values);
		last_it++;
		CHECK(last_it == range_test_graph.end());
	}

	// ======================================
	//              Extractor
	// ======================================
	auto extractor_graph = gdwg::graph<int, int>(value_type_vector.begin(), value_type_vector.end());
	auto out = std::ostringstream{};
	out << extractor_graph;
	auto const expected_output = std::string_view(R"(1 (
  5 | -1
)
2 (
  1 | 1
  4 | 2
)
3 (
  2 | 2
  6 | -8
)
4 (
  1 | -4
  5 | 3
)
5 (
  2 | 7
)
6 (
  2 | 5
  3 | 10
)
)");
	SECTION("Extractor Test") {
		CHECK(out.str() == expected_output);
	}
}
