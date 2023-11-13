SELECT rowid
FROM lineitem
WHERE l_o_orderdate >= CAST('1993-10-01' AS date)
  AND l_o_orderdate < CAST('1994-01-01' AS date)
  AND l_returnflag = 'R';
