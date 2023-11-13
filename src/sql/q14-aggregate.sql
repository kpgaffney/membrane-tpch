SELECT 100.00 * sum(
        CASE
            WHEN l_p_type LIKE 'PROMO%' THEN
                l_extendedprice * (1 - l_discount)
            ELSE
                0
            END) / sum(l_extendedprice * (1 - l_discount)) AS promo_revenue
FROM selproj;
