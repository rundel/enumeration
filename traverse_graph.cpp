#include <Rcpp.h>

// [[Rcpp::plugins(cpp11)]]

void print_vector(std::vector<int> v, std::string sep = " ") {
  for(auto&& x : v)
    Rcpp::Rcout << x << sep;
  
  Rcpp::Rcout << "\n";
}

std::vector<int> setdiff(std::vector<int> const& x, std::vector<int> const& y) {
  std::vector<int> res(x.size());
  auto it = std::set_difference(x.begin(), x.end(), y.begin(), y.end(), res.begin());
  res.resize(it-res.begin());
  
  return res;
}

void walk_graph(std::vector< std::vector<int> > const& g, std::vector<int> const& pop, 
                int lower_pop, int upper_pop, std::map<std::vector<int>,bool> &res,
                std::vector<int> nodes, std::vector<int> history, int cur_pop, int depth)
{
  for(int node : nodes) {
    int new_pop = cur_pop + pop[node];
    
    std::vector<int> new_history = history;
    new_history.push_back(node);
    std::sort(new_history.begin(), new_history.end());
    
    if (res.find(new_history) != res.end())
      continue;
    
    
    if (new_pop >= lower_pop & new_pop <= upper_pop) {
      
      res[new_history] = true;
      Rcpp::Rcout << "Pop [" << new_pop << "] : ";
      print_vector(new_history);
    }
    
    std::vector<int> new_nodes(nodes);
    
    new_nodes.insert(new_nodes.end(), g[node].begin(), g[node].end());
    std::sort(new_nodes.begin(), new_nodes.end());
    auto last = std::unique(new_nodes.begin(), new_nodes.end());
    new_nodes.erase(last, new_nodes.end());

    new_nodes = setdiff(new_nodes, new_history);
    
    if (new_pop <= upper_pop)
      walk_graph(g, pop, lower_pop, upper_pop, res, new_nodes, new_history, new_pop, ++depth);
    
    res[new_history] = false;
  }
}

// [[Rcpp::export]]
std::vector<std::vector<int> > traverse_graph(std::vector< std::vector<int> > g, std::vector<int> pop,
                    int lower_pop, int upper_pop)
{
  // Fix indexing
  for(int i=0; i<g.size(); i++) {
    for(int j=0; j<g[i].size(); j++) {
      g[i][j]--;
    }
  }
  
  std::vector<int> history = {0};
  std::map<std::vector<int>,bool> res;
  
  walk_graph(g, pop, lower_pop, upper_pop, res, g[0], history, pop[0], 0);
  
  std::vector<std::vector<int> > districts;
  for(auto& r : res) {
    if (r.second)
      districts.push_back(r.first);
  }
  
  return districts;
}
