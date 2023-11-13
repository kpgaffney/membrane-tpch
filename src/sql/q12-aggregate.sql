SELECT l_shipmode,
       sum(
               CASE
                   WHEN l_o_orderpriority = '1-URGENT'
                       OR l_o_orderpriority = '2-HIGH' THEN
                       1
                   ELSE
                       0
                   END) AS high_line_count,
       sum(
               CASE
                   WHEN l_o_orderpriority <> '1-URGENT'
                       AND l_o_orderpriority <> '2-HIGH' THEN
                       1
                   ELSE
                       0
                   END) AS low_line_count
FROM selproj
GROUP BY l_shipmode
ORDER BY l_shipmode;
