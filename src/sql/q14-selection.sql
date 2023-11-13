SELECT rowid
FROM lineitem
WHERE l_shipdate >= date '1995-09-01'
  AND l_shipdate < CAST('1995-10-01' AS date);
