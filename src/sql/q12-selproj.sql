CREATE TABLE selproj AS
SELECT l_shipmode, l_o_orderpriority
FROM lineitem
WHERE l_shipmode IN ('MAIL', 'SHIP')
  AND l_commitdate < l_receiptdate
  AND l_shipdate < l_commitdate
  AND l_receiptdate >= CAST('1994-01-01' AS date)
  AND l_receiptdate < CAST('1995-01-01' AS date);
