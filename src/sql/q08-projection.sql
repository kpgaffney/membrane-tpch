SELECT extract(year FROM l_o_orderdate) AS o_year,
       l_extendedprice,
       l_discount,
       l_s_n_name                       AS nation
FROM lineitem;
