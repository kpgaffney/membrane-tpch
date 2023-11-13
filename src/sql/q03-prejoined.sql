SELECT l_orderkey,
       sum(l_extendedprice * (1 - l_discount)) AS revenue,
       l_o_orderdate,
       l_o_shippriority
FROM lineitem
WHERE l_c_mktsegment = 'BUILDING'
  AND l_o_orderdate < CAST('1995-03-15' AS date)
  AND l_shipdate > CAST('1995-03-15' AS date)
GROUP BY l_orderkey,
         l_o_orderdate,
         l_o_shippriority
ORDER BY revenue DESC,
         l_o_orderdate LIMIT 10;
