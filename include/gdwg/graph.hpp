#ifndef GDWG_GRAPH_HPP
#define GDWG_GRAPH_HPP

#include <map>
#include <ostream>
#include <range/v3/utility.hpp>
#include <set>

namespace gdwg {
	template<concepts::regular N, concepts::regular E>
	requires concepts::totally_ordered<N> //
	   and concepts::totally_ordered<E> //

	   class graph {
	public:
		class iterator;

		struct value_type {
			N from;
			N to;
			E weight;
		};

		struct pair_cmp {
			auto operator()(std::pair<N*, N*> a, std::pair<N*, N*> b) -> bool {
				return *a->first == *b->first ? *a->second < *b->second : *a->first < *b->first;
			}
			auto operator()(std::pair<N*, N*> a, std::pair<N*, N*> b) const -> bool {
				return *a.first == *b.first ? *a.second < *b.second : *a.first < *b.first;
			}
		};

		// Your member functions go here

		// ======================================
		//              Constructors
		// ======================================
		graph() noexcept {
			this->nodes_ = std::set<std::unique_ptr<N>>();
			this->edges_ = std::map<std::pair<N*, N*>, std::set<std::unique_ptr<E>>, pair_cmp>();
		}
		graph(std::initializer_list<N> il) {
			this->nodes_ = std::set<std::unique_ptr<N>>();
			this->edges_ = std::map<std::pair<N*, N*>, std::set<std::unique_ptr<E>>, pair_cmp>();
			for (auto temp = il.begin(); temp != il.end(); temp++) {
				this->insert_node(*temp);
			}
		}
		// Needs to be finished
		template<ranges::forward_iterator I, ranges::sentinel_for<I> S>
		requires ranges::indirectly_copyable<I, N*> graph(I first, S last) {
			for (auto temp = first; temp != last; temp++) {
				this->insert_node(*temp);
			}
		}
		// Needs to be finished
		template<ranges::forward_iterator I, ranges::sentinel_for<I> S>
		requires ranges::indirectly_copyable<I, value_type*> graph(I first, S last) {
			for (auto temp = first; temp != last; temp++) {
				this->insert_node(temp->to);
				this->insert_node(temp->from);
				this->insert_edge(temp->from, temp->to, temp->weight);
			}
		}

		graph(graph&& other) noexcept {
			this->nodes_ = std::move(other.nodes_);
			this->edges_ = std::move(other.edges_);
		}

		auto operator=(graph&& other) noexcept -> graph& {
			ranges::swap(this->nodes_, other.nodes_);
			ranges::swap(this->edges_, other.edges_);
			other.clear();
			return *this;
		}

		graph(graph const& other) {
			this->nodes_ = std::set<std::unique_ptr<N>>();
			this->edges_ = std::map<std::pair<N*, N*>, std::set<std::unique_ptr<E>>, pair_cmp>();
			for (auto temp : other.nodes()) {
				this->insert_node(temp);
			}
			for (auto [key, value] : other.all_edges()) {
				for (auto temp : value) {
					this->insert_edge(key.first, key.second, temp);
				}
			}
		}

		auto operator=(graph const& other) -> graph& {
			if (this == other) {
				return *this;
			}
			auto temp = other;
			std::swap(this->nodes_, temp.nodes_);
			std::swap(this->edges_, temp.edges_);

			return *this;
		}

		// ======================================
		//              Modifiers
		// ======================================
		auto insert_node(N const& value) -> bool {
			if (is_node(value)) {
				return false;
			}
			auto temp = value;
			auto new_node = std::make_unique<N>(temp);
			this->nodes_.insert(std::move(new_node));
			return true;
		}

		auto insert_edge(N const& src, N const& dst, E const& weight) -> bool {
			if (!is_node(src) || !is_node(dst)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::insert_edge when either src "
				                         "or dst node does not exist");
			}
			auto my_src = this->find_node_ptr(src);
			auto my_dst = this->find_node_ptr(dst);

			if (edge_exist(my_src, my_dst, weight)) {
				return false;
			}

			auto new_edge = std::make_unique<E>(weight);

			edges_[std::make_pair(my_src, my_dst)].insert(std::move(new_edge));

			return true;
		}

		auto replace_node(N const& old_data, N const& new_data) -> bool {
			if (!is_node(old_data)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::replace_node on a node that "
				                         "doesn't exist");
			}
			if (is_node(new_data)) {
				return false;
			}
			auto smart_ptr = find_node_ptr(old_data);
			*smart_ptr = new_data;

			return true;
		}

		auto merge_replace_node(N const& old_data, N const& new_data) -> void {
			if (!is_node(old_data) || !is_node(new_data)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::merge_replace_node on old or "
				                         "new data if they don't exist in the graph");
			}
			auto old_data_ptr = find_node_ptr(old_data);
			auto new_data_ptr = find_node_ptr(new_data);
			auto need_replace = std::vector<std::pair<N*, N*>>();
			for (auto temp = edges_.begin(); temp != edges_.end(); temp++) {
				auto key = temp->first;
				if (*key.first == new_data || *key.second == new_data) {
					need_replace.push_back(key);
				}
			}

			for (auto temp : need_replace) {
				auto modified_key = edges_.extract(temp);
				if (modified_key.key().first == new_data_ptr) {
					modified_key.key().first = old_data_ptr;
				}
				else {
					modified_key.key().second = old_data_ptr;
				}
				edges_.insert(std::move(modified_key));
			}

			erase_node(new_data);

			*old_data_ptr = new_data;
		}

		auto erase_node(N const& value) -> bool {
			if (!is_node(value)) {
				return false;
			}

			auto it = edges_.begin();

			while (it != edges_.end()) {
				auto key = it->first;
				if (*key.first == value || *key.second == value) {
					it = edges_.erase(it);
				}
				else {
					it++;
				}
			}

			auto it2 = nodes_.begin();

			while (it2 != nodes_.end()) {
				if (**it2 == value) {
					it2 = nodes_.erase(it2);
				}
				else {
					it2++;
				}
			}

			return true;
		}

		auto erase_edge(N const& src, N const& dst, E const& weight) -> bool {
			if (!is_node(src) || !is_node(dst)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::erase_edge on src or dst if "
				                         "they don't exist in the graph");
			}

			if (!is_connected(src, dst)) {
				return false;
			}
			auto src_ptr = find_node_ptr(src);
			auto dst_ptr = find_node_ptr(dst);

			auto it = edges_.at(std::make_pair(src_ptr, dst_ptr)).begin();

			while (it != edges_.at(std::make_pair(src_ptr, dst_ptr)).end()) {
				if (**it == weight) {
					edges_.at(std::make_pair(src_ptr, dst_ptr)).erase(it);
					edges_.at(std::make_pair(src_ptr, dst_ptr)).end();
					if (edges_.at(std::make_pair(src_ptr, dst_ptr)).empty()) {
						edges_.erase(std::make_pair(src_ptr, dst_ptr));
					}
					return true;
				}

				it++;
			}
			return false;
		}

		auto erase_edge(iterator i) -> iterator {
			/*
			This was my initial attempt on this function, but it doesnt let me use
			i.outer_->second.erase(i.inner) with a warning saying that the set.erase(iterator) function
			is not marked const. If you have enough time is it alright if you give me a hint on why
			this is not working.

			auto new_set_it = i.outer_->second.erase(i.inner_); //I got stopped in this line
			auto new_inner = i.inner_;
			auto new_outer = i.outer_;
			if (i.outer_->second.end() == i_inner){
			   new_outer = new_outer++;
			   new_inner = new_outer->second.begin();
			}
			return iterator(edges_, new_outer, new_inner); */

			// Very Naive Method, does not satisfy time complexity
			if (i == this->end()) {
				return this->end();
			}
			auto next = i;
			next++;
			bool next_is_end = next == this->end();

			auto next_value = next == this->end() ? *i : *next;

			erase_edge(std::get<0>(*i), std::get<1>(*i), std::get<2>(*i));
			return next_is_end
			          ? this->end()
			          : find(std::get<0>(next_value), std::get<1>(next_value), std::get<2>(next_value));
		}

		auto erase_edge(iterator i, iterator s) -> iterator {
			auto ret_it = i;

			while (ret_it != s) {
				ret_it = erase_edge(ret_it);
			}
			return ret_it;
		}

		auto clear() noexcept -> void {
			nodes_.clear();
			edges_.clear();
		}

		// ======================================
		//              Accessors
		// ======================================
		[[nodiscard]] auto is_node(N const& value) -> bool {
			return static_cast<bool>(find_node_ptr(value) != nullptr);
		}

		[[nodiscard]] auto is_node(N const& value) const -> bool {
			return static_cast<bool>(find_node_ptr(value) != nullptr);
		}

		[[nodiscard]] auto empty() -> bool {
			return static_cast<bool>(nodes_.empty() && edges_.empty());
		}

		[[nodiscard]] auto is_connected(N const& src, N const& dst) -> bool {
			if (!is_node(src) || !is_node(dst)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::is_connected if src or dst "
				                         "node don't exist in the graph");
			}
			return !edges_[std::make_pair(find_node_ptr(src), find_node_ptr(dst))].empty();
		}

		[[nodiscard]] auto nodes() -> std::vector<N> {
			auto new_vector = std::vector<N>();
			for (auto& temp : nodes_) {
				new_vector.push_back(*temp.get());
			}
			std::sort(new_vector.begin(), new_vector.end());
			return new_vector;
		}

		[[nodiscard]] auto nodes() const -> std::vector<N> {
			auto new_vector = std::vector<N>();
			for (auto& temp : nodes_) {
				new_vector.push_back(*temp.get());
			}
			std::sort(new_vector.begin(), new_vector.end());
			return new_vector;
		}
		[[nodiscard]] auto all_edges() -> std::map<std::pair<N, N>, std::set<E>> {
			auto new_map = std::map<std::pair<N, N>, std::set<E>>();
			for (auto& [key, values] : edges_) {
				auto set_weights = std::set<E>();
				for (auto& temp : values) {
					set_weights.insert(*temp);
				}
				new_map[std::make_pair(*key.first, *key.second)] = set_weights;
			}
			return new_map;
		}

		[[nodiscard]] auto all_edges() const -> std::map<std::pair<N, N>, std::set<E>> {
			auto new_map = std::map<std::pair<N, N>, std::set<E>>();
			for (auto& [key, values] : edges_) {
				auto set_weights = std::set<E>();
				for (auto& temp : values) {
					set_weights.insert(*temp);
				}
				new_map[std::make_pair(*key.first, *key.second)] = set_weights;
			}
			return new_map;
		}

		[[nodiscard]] auto weights(N const& from, N const& to) -> std::vector<E> {
			if (!is_node(from) || !is_node(to)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::weights if src or dst node "
				                         "don't exist in the graph");
			}
			auto result = std::vector<E>();
			auto my_src = this->find_node_ptr(from);
			auto my_dst = this->find_node_ptr(to);
			for (auto& temp : edges_[std::make_pair(my_src, my_dst)]) {
				result.push_back(*temp.get());
			}
			std::sort(result.begin(), result.end());
			return result;
		}

		[[nodiscard]] auto find(N const& src, N const& dst, E const& weight) -> iterator {
			for (auto temp = begin(); temp != end(); temp++) {
				if (std::get<0>(*temp) == src && std::get<1>(*temp) == dst
				    && std::get<2>(*temp) == weight) {
					return temp;
				}
			}
			return end();
		}

		[[nodiscard]] auto connections(N const& src) -> std::vector<N> {
			if (!is_node(src)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::connections if src doesn't "
				                         "exist in the graph");
			}
			auto result = std::set<N>();
			for (auto& [key, value] : edges_) {
				if (*key.first == src) {
					result.insert(*key.second);
				}
			}
			auto final_result = std::vector<N>(result.begin(), result.end());
			std::sort(final_result.begin(), final_result.end());

			return final_result;
		}
		[[nodiscard]] auto connections(N const& src) const -> std::vector<N> {
			if (!is_node(src)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::connections if src doesn't "
				                         "exist in the graph");
			}
			auto result = std::set<N>();
			for (auto& [key, value] : edges_) {
				if (*key.first == src) {
					result.insert(*key.second);
				}
			}
			auto final_result = std::vector<N>(result.begin(), result.end());
			std::sort(final_result.begin(), final_result.end());

			return final_result;
		}

		// ======================================
		//              Range Access
		// ======================================
		[[nodiscard]] auto begin() const -> iterator {
			auto inner = edges_.empty() ? decltype(edges_.begin()->second.begin()){}
			                            : edges_.begin()->second.begin();
			return iterator(edges_, edges_.begin(), inner);
		}

		[[nodiscard]] auto end() const -> iterator {
			return iterator(edges_, edges_.end(), {});
		}

		// ======================================
		//              Comparisons
		// ======================================
		[[nodiscard]] auto operator==(graph const& other) const -> bool {
			return static_cast<bool>(this->nodes() == other.nodes()
			                         && this->all_edges() == other.all_edges());
		}

		// ======================================
		//              Extractor
		// ======================================
		friend auto operator<<(std::ostream& os, graph const& g) -> std::ostream& {
			auto g_nodes = g.nodes();
			auto g_edges = g.all_edges();

			for (auto node : g_nodes) {
				os << node << " (" << std::endl;
				auto g_conn = g.connections(node);
				for (auto connects : g_conn) {
					for (auto edge : g_edges[std::make_pair(node, connects)]) {
						os << "  " << connects << " | " << edge << std::endl;
					}
				}
				os << ")" << std::endl;
			}

			return os;
		}
		// ======================================
		//              Iterators
		// ======================================

		// I reused some of code from rope_q33 and rope_q54 from tutorials for the iterators section
		class iterator {
			using outer_iterator =
			   typename std::map<std::pair<N*, N*>, std::set<std::unique_ptr<E>>>::const_iterator;
			using inner_iterator = typename std::set<std::unique_ptr<E>>::const_iterator;

		public:
			using value_type = ranges::common_tuple<N, N, E>;
			using difference_type = std::ptrdiff_t;
			using iterator_category = std::bidirectional_iterator_tag;

			// Iterator constructor
			iterator() = default;

			// Iterator source
			auto operator*() -> ranges::common_tuple<N const&, N const&, E const&> {
				return ranges::common_tuple<N const&, N const&, E const&>{*outer_->first.first,
				                                                          *outer_->first.second,
				                                                          **inner_};
			};

			// Iterator traversal
			auto operator++() -> iterator& {
				if (inner_ != outer_->second.end()) {
					++inner_;
					if (inner_ != outer_->second.end()) {
						return *this;
					}
				}
				++outer_;
				inner_ = outer_ == pointee_->end() ? inner_iterator() : outer_->second.begin();
				return *this;
			};
			auto operator++(int) -> iterator {
				auto temp = *this;
				++*this;
				return temp;
			}
			auto operator--() -> iterator& {
				if (inner_ == inner_iterator()) {
					outer_ = ranges::prev(pointee_->end());
					inner_ = ranges::prev(outer_->second.end());
					return *this;
				}

				if (inner_ != outer_->second.begin()) {
					--inner_;
					return *this;
				}

				--outer_;
				inner_ = ranges::prev(outer_->second.end());
				return *this;
			}
			auto operator--(int) -> iterator {
				auto temp = *this;
				--*this;
				return temp;
			}

			// Iterator comparison
			auto operator==(iterator const& other) const -> bool = default;

		private:
			std::map<std::pair<N*, N*>, std::set<std::unique_ptr<E>>, pair_cmp> const* pointee_ =
			   nullptr;
			outer_iterator outer_;
			inner_iterator inner_;
			friend class graph;
			explicit iterator(
			   std::map<std::pair<N*, N*>, std::set<std::unique_ptr<E>>, pair_cmp> const& pointee,
			   outer_iterator outer,
			   inner_iterator inner) noexcept
			: pointee_(&pointee)
			, outer_(outer)
			, inner_(inner) {}
		};

	private:
		std::set<std::unique_ptr<N>> nodes_;
		std::map<std::pair<N*, N*>, std::set<std::unique_ptr<E>>, pair_cmp> edges_;

		auto find_node_ptr(N node) -> N* {
			auto node_ptr = [node](auto& node_value) { return node == *node_value; };
			auto result = find_if(nodes_.begin(), nodes_.end(), node_ptr);
			if (result == nodes_.end()) {
				return nullptr;
			}
			auto& temp = *result;
			return temp.get();
		}

		auto find_node_ptr(N node) const -> N* {
			auto node_ptr = [node](auto& node_value) { return node == *node_value; };
			auto result = find_if(nodes_.begin(), nodes_.end(), node_ptr);
			if (result == nodes_.end()) {
				return nullptr;
			}
			auto& temp = *result;
			return temp.get();
		}

		auto edge_exist(N* src, N* dst, E weight) -> bool {
			for (auto& temp : edges_[std::make_pair(src, dst)]) {
				if (weight == *temp.get()) {
					return true;
				}
			}
			return false;
		}
	};

} // namespace gdwg

#endif // GDWG_GRAPH_HPP
