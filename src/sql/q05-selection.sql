SELECT rowid
FROM lineitem
WHERE l_o_c_n_r_name = 'ASIA'
  AND l_o_orderdate >= CAST('1994-01-01' AS date)
  AND l_o_orderdate < CAST('1995-01-01' AS date);
