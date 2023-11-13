CREATE TABLE selproj AS
SELECT extract(year FROM l_o_orderdate) AS o_year,
       l_extendedprice,
       l_discount,
       l_s_n_name                       AS nation
FROM lineitem
WHERE l_o_c_n_r_name = 'AMERICA'
  AND l_o_orderdate BETWEEN CAST('1995-01-01' AS date) AND CAST('1996-12-31' AS date)
  AND l_p_type = 'ECONOMY ANODIZED STEEL';
