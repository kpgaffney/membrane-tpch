SELECT l_o_c_n_name,
       sum(l_extendedprice * (1 - l_discount)) AS revenue
FROM selproj
GROUP BY l_o_c_n_name
ORDER BY revenue DESC;
