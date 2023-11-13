SELECT o_year,
       sum(
               CASE
                   WHEN nation = 'BRAZIL' THEN
                       volume
                   ELSE
                       0
                   END) / sum(volume) AS mkt_share
FROM (SELECT o_year,
             l_extendedprice * (1 - l_discount) AS volume,
             nation
      FROM selproj) AS all_nations
GROUP BY o_year
ORDER BY o_year;
