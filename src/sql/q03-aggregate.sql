SELECT l_orderkey,
       sum(l_extendedprice * (1 - l_discount)) AS revenue,
       l_o_orderdate,
       l_o_shippriority
FROM selproj
GROUP BY l_orderkey,
         l_o_orderdate,
         l_o_shippriority
ORDER BY revenue DESC,
         l_o_orderdate LIMIT 10;
