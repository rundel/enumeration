library(sf)
library(dplyr)
library(purrr)
library(rlang)

Rcpp::sourceCpp("traverse_graph.cpp")

d = sf::read_sf("data/Hoke_Cumberland_shapefile.shp") %>%
  select(vtd, pop) %>%
  arrange(desc(pop))

pop = as.integer(d$pop)
total_pop = sum(pop)
lower_pop = as.integer(total_pop / 2 - 10000)
upper_pop = as.integer(total_pop / 2 + 10000)

g = st_touches(d)

ctr = st_centroid(d) %>% st_coordinates()


districts = traverse_graph(g, pop, lower_pop, upper_pop)
  
tmp = districts %>% 
  sample() %>%
  map(
    function(x) {
      (1:nrow(d) %in% (x+1))
    }
  ) %>%
  do.call(cbind, .) %>%
  cbind(d, .) %>%
  select(-vtd, -pop)

cols = names(tmp)[-ncol(tmp)] # remove geometry


pb = dplyr::progress_estimated(length(cols))
types = map(
  cols,
  function(col) {
    p = tmp %>% select(X1) %>% filter(X1 == FALSE) %>% st_union()
    types = p %>% st_geometry_type()
    
    if (!"MULTIPOLYGON" %in% types)
      plot(p)
    
    pb$tick()$print()
    types
  }
)


m = sample(m)
