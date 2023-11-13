SELECT 100.00 * sum(
        CASE
            WHEN l_p_type LIKE 'PROMO%' THEN
                l_extendedprice * (1 - l_discount)
            ELSE
                0
            END) / sum(l_extendedprice * (1 - l_discount)) AS promo_revenue
FROM lineitem
WHERE l_shipdate >= date '1995-09-01'
  AND l_shipdate < CAST('1995-10-01' AS date);
