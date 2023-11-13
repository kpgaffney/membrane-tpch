CREATE TABLE selproj AS
SELECT l_orderkey,
       l_extendedprice,
       l_discount,
       l_o_orderdate,
       l_o_shippriority
FROM lineitem
WHERE l_c_mktsegment = 'BUILDING'
  AND l_o_orderdate < CAST('1995-03-15' AS date)
  AND l_shipdate > CAST('1995-03-15' AS date);
