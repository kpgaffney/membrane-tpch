SELECT l_o_c_n_name,
       sum(l_extendedprice * (1 - l_discount)) AS revenue
FROM lineitem
WHERE l_o_c_n_r_name = 'ASIA'
  AND l_o_orderdate >= CAST('1994-01-01' AS date)
  AND l_o_orderdate < CAST('1995-01-01' AS date)
GROUP BY l_o_c_n_name
ORDER BY revenue DESC;
