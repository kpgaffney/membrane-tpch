SELECT o_year,
       sum(
               CASE
                   WHEN nation = 'BRAZIL' THEN
                       volume
                   ELSE
                       0
                   END) / sum(volume) AS mkt_share
FROM (SELECT extract(year FROM l_o_orderdate)   AS o_year,
             l_extendedprice * (1 - l_discount) AS volume,
             l_s_n_name                         AS nation
      FROM lineitem
      WHERE l_o_c_n_r_name = 'AMERICA'
        AND l_o_orderdate BETWEEN CAST('1995-01-01' AS date) AND CAST('1996-12-31' AS date)
        AND l_p_type = 'ECONOMY ANODIZED STEEL') AS all_nations
GROUP BY o_year
ORDER BY o_year;
