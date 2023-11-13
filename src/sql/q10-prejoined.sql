SELECT l_c_custkey,
       l_c_name,
       sum(l_extendedprice * (1 - l_discount)) AS revenue,
       l_c_acctbal,
       l_o_c_n_name,
       l_c_address,
       l_c_phone,
       l_c_comment
FROM lineitem
WHERE l_o_orderdate >= CAST('1993-10-01' AS date)
  AND l_o_orderdate < CAST('1994-01-01' AS date)
  AND l_returnflag = 'R'
GROUP BY l_c_custkey,
         l_c_name,
         l_c_acctbal,
         l_c_phone,
         l_o_c_n_name,
         l_c_address,
         l_c_comment
ORDER BY revenue DESC LIMIT 20;
