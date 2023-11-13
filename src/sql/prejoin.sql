ALTER TABLE lineitem RENAME TO lineitem_old;

CREATE TABLE lineitem
(
    l_orderkey        INTEGER        NOT NULL,
    l_partkey         INTEGER        NOT NULL,
    l_suppkey         INTEGER        NOT NULL,
    l_linenumber      INTEGER        NOT NULL,
    l_quantity        DECIMAL(15, 2) NOT NULL,
    l_extendedprice   DECIMAL(15, 2) NOT NULL,
    l_discount        DECIMAL(15, 2) NOT NULL,
    l_tax             DECIMAL(15, 2) NOT NULL,
    l_returnflag      VARCHAR        NOT NULL,
    l_linestatus      VARCHAR        NOT NULL,
    l_shipdate        DATE           NOT NULL,
    l_commitdate      DATE           NOT NULL,
    l_receiptdate     DATE           NOT NULL,
    l_shipinstruct    VARCHAR        NOT NULL,
    l_shipmode        VARCHAR        NOT NULL,
    l_comment         VARCHAR        NOT NULL,
    l_c_custkey       INTEGER        NOT NULL,
    l_c_mktsegment    VARCHAR        NOT NULL,
    l_c_name          VARCHAR        NOT NULL,
    l_c_acctbal       DECIMAL(15, 2) NOT NULL,
    l_c_address       VARCHAR        NOT NULL,
    l_c_phone         VARCHAR        NOT NULL,
    l_c_comment       VARCHAR        NOT NULL,
    l_o_orderdate     DATE           NOT NULL,
    l_o_shippriority  INTEGER        NOT NULL,
    l_o_orderpriority VARCHAR        NOT NULL,
    l_p_brand         VARCHAR        NOT NULL,
    l_p_type          VARCHAR        NOT NULL,
    l_p_size          INTEGER        NOT NULL,
    l_p_container     VARCHAR        NOT NULL,
    l_o_c_n_name      VARCHAR        NOT NULL,
    l_o_c_n_r_name    VARCHAR        NOT NULL,
    l_s_n_name        VARCHAR        NOT NULL
);

INSERT INTO lineitem
SELECT l_orderkey,
       l_partkey,
       l_suppkey,
       l_linenumber,
       l_quantity,
       l_extendedprice,
       l_discount,
       l_tax,
       l_returnflag,
       l_linestatus,
       l_shipdate,
       l_commitdate,
       l_receiptdate,
       l_shipinstruct,
       l_shipmode,
       l_comment,
       c_custkey,
       c_mktsegment,
       c_name,
       c_acctbal,
       c_address,
       c_phone,
       c_comment,
       o_orderdate,
       o_shippriority,
       o_orderpriority,
       p_brand,
       p_type,
       p_size,
       p_container,
       c_nation.n_name,
       r_name,
       s_nation.n_name,
FROM lineitem_old,
     customer,
     orders,
     part,
     supplier,
     nation c_nation,
     nation s_nation,
     region
WHERE l_orderkey = o_orderkey
  AND o_custkey = c_custkey
  AND c_nationkey = c_nation.n_nationkey
  AND c_nation.n_regionkey = r_regionkey
  AND l_suppkey = s_suppkey
  AND s_nationkey = s_nation.n_nationkey
  AND l_partkey = p_partkey;

DROP TABLE lineitem_old;
